#include <cassert>
#include "ms_doc.h"
#include "../utils/hash.h"
#include "../office/ole_utils.h"
#include "../office/offcrypto_assist.h"
#include "./helper.h"
#include "./ms_oleds.h"

/*
* nFib is Fib2->nFibNew or Fib->base.nFib
*/
std::string msdoc_version(unsigned short nFib) {
    std::string ans;
    switch (nFib) {
    case 314: ans = "Word2005"; break;
    case 274: ans = "Word2005_Compat2003"; break;
    case 268: ans = "Word2003"; break;
    case 257: ans = "Word2002"; break;
    case 217: ans = "Word2000"; break;
    case 193: ans = "Word97"; break;
    case 104: ans = "Word95_Or_Word6"; break;
    case 105: ans = "Word95Beta"; break;
    case 101: ans = "WinWord6.0"; break;
    case 45:  ans = "WinWord2.0"; break;
    case 33:  ans = "WinWord1.0"; break;
    default:  ans = "Unknown"; break;
    }
    return ans;
}

int msdoc::parse(const cfb_ctx_t *ctx, const olefile_t *olefile, msdoc_t *doc) {
    if (olefile == nullptr || doc == nullptr) {
        return BQ::INVALID_ARG;
    }

    const char16_t *sn_wordocument = u"WordDocument";
    const char16_t *sn_objectpool = u"ObjectPool";
    const char16_t *sn_0table = u"0table";
    const char16_t *sn_1table = u"1table";
    const char16_t *sn_encryption = u"encryption";
    uint32_t did_0table = 0, did_1table = 0;

    for (uint32_t i = 0; i < olefile->entry_count; i++) {
        auto cur_entry = (olefile->entry_tree + i);
        if (cur_entry == nullptr || cur_entry->depth > 1) {
            continue;
        }
        if (doc->did_objectpool && doc->did_wordocument && did_0table && did_1table && doc->did_encryption) {
            break;
        }
        if (!did_0table && 0 == ucs::icmp<char16_t>(cur_entry->item->name, sn_0table)) {
            did_0table = i;
            continue;
        }
        if (!did_1table && 0 == ucs::icmp<char16_t>(cur_entry->item->name, sn_1table)) {
            did_1table = i;
            continue;
        }
        if (!doc->did_wordocument && 0 == ucs::icmp<char16_t>(cur_entry->item->name, sn_wordocument)) {
            doc->did_wordocument = i;
            continue;
        }
        if (!doc->did_objectpool && 0 == ucs::icmp<char16_t>(cur_entry->item->name, sn_objectpool)) {
            doc->did_objectpool = i;
            continue;
        }
        if (!doc->did_encryption && 0 == ucs::icmp<char16_t>(cur_entry->item->name, sn_encryption)) {
            doc->did_encryption = i;
            continue;
        }
    }
    if (doc->did_wordocument == 0) {
        return BQ::ERR_FILE_FORMAT;
    }

    fib_t &fib = doc->fib;
    uint32_t fib_min_size = (uint8_t *) &fib.rgfclcb - (uint8_t *) &fib;

    int nret = ole::read_stream(olefile, doc->did_wordocument, (uint8_t *) &fib, fib_min_size, 0);
    if (nret != BQ::OK) {
        return nret;
    }

    fib.base.ident = XsUtils::byteorder2host<uint16_t>(fib.base.ident, is_little_endian);
    fib.base.fib = XsUtils::byteorder2host<uint16_t>(fib.base.fib, is_little_endian);
    (void) fib.base.unused;
    fib.base.lid = XsUtils::byteorder2host<uint16_t>(fib.base.lid, is_little_endian);
    fib.base.pn_next = XsUtils::byteorder2host<uint16_t>(fib.base.pn_next, is_little_endian);
    fib.base.flags = XsUtils::byteorder2host<uint16_t>(fib.base.flags, is_little_endian);
    fib.base.fib_back = XsUtils::byteorder2host<uint16_t>(fib.base.fib_back, is_little_endian);
    fib.base.key = XsUtils::byteorder2host<uint32_t>(fib.base.key, is_little_endian);
    (void) fib.base.envr;
    (void) fib.base.flag_b;
    doc->did_table = doc->fib.base.is_1table() ? did_1table : did_0table;

    if (0xA5EC != fib.base.ident || (fib.base.fib_back != 0x00BF && fib.base.fib_back != 0x00C1) || doc->did_table == 0) {
        return BQ::ERR_FORMAT;
    }

    /*
    * TODO：
    *   当 fib.base.ident > 0xA5DC && fib.base.ident != 0xA699 时，有可能是 WORD7, 此时
    *   在 WordDocument 流的 0x118 偏移处，连续读取两个 uint32_t , 如果这两个值都不为 0 ，则可能有风险
    */

    do {
        if (!fib.base.encrypted()) {
            break;
        }
        if (fib.base.obfuscated()) {
            doc->crypt.type = encryinfo_bin_xor_m2;
            doc->crypt.pass.xor_m2 = fib.base.key;
            break;
        }

        uint8_t *data = 0;
        uint32_t size = 0;

        ON_SCOPE_EXIT([&] { if (data) free(data); });
        nret = ole::read_stream(olefile, doc->did_table, -1, &data, &size);
        if (nret != BQ::OK) {
            break;
        }

        assert(size > fib.base.key);
        nret = crypto_binary().parse_rc4_encryption_header(data, size, is_little_endian, &doc->crypt);
    } while (false);

    return nret;
}

int msdoc::decrypt(const cfb_ctx_t *ctx, const msdoc_t *doc, const char16_t *passwd, const char16_t *name, uint8_t *data, uint32_t size) {
    if (doc == 0 || doc->file == 0 || 0 == data || 0 == size || !doc->fib.base.encrypted()) {
        return BQ::INVALID_ARG;
    }

    bool little_endian = !doc->big_endian;

    uint32_t offset_wordocument = 0, offset_table = 0, offset_data = 0, blocksize = 0, offset_debug = 0;
    if (doc->crypt.type == encryinfo_bin_rc4) {
        offset_wordocument = DOC_FIB_NOT_ENCRYPTED;
        offset_table = doc->fib.base.key;
        blocksize = DOC_RC4_BLOCK_SIZE;
    } else if (doc->crypt.type == encryinfo_bin_rc4_capi) {
        offset_wordocument = DOC_FIB_NOT_ENCRYPTED;
        offset_table = doc->fib.base.key;
        blocksize = DOC_RC4CAPI_BLOCK_SIZE;
    } else if (doc->crypt.type == encryinfo_bin_xor_m2) {
        offset_wordocument = DOC_FIB_NOT_ENCRYPTED;
    } else {
        return BQ::ERR_CRYPTION_UNKNOWN;
    }

    int nret = crypto_binary().passwd_verify(&doc->crypt, passwd);
    if (nret != BQ::OK) {
        return nret;
    }
    nret = BQ::NOT_IMPLEMENT;

    do {
        if (0 != ucs::icmp(name, u"WordDocument")) {
            break;
        }

        uint8_t cache[DOC_FIB_NOT_ENCRYPTED] = {0};
        memcpy(cache, data, offset_wordocument);

        crypto_binary::block_info_t block;
        block.block_size = blocksize;
        nret = crypto_binary().decrypt_inplace(&doc->crypt, passwd, data, size, &block);
        if (nret != BQ::OK) {
            break;
        }

        memcpy(data, cache, offset_wordocument);
        offset_debug = offset_wordocument;
    } while (false);

    do {
        if (0 != ucs::icmp(name, u"0Table") && 0 != ucs::icmp(name, u"1Table")) {
            break;
        }

        uint8_t *cache = nullptr;
        if (offset_table > 0) {
            cache = (uint8_t *) malloc(offset_table);
            if (cache == nullptr) {
                nret = BQ::ERR_MALLOC;
                break;
            }
            memcpy(cache, data, offset_table);
        }
        ON_SCOPE_EXIT([&]() { if (cache) free(cache); });

        crypto_binary::block_info_t block;
        block.block_size = blocksize;
        nret = crypto_binary().decrypt_inplace(&doc->crypt, passwd, data, size, &block);
        if (nret != BQ::OK) {
            break;
        }

        if (offset_table > 0) {
            memcpy(data, cache, offset_table);
        }
        offset_debug = offset_table;
    } while (false);

    do {
        if (0 != ucs::icmp(name, u"Data")) {
            break;
        }

        crypto_binary::block_info_t block;
        block.block_size = blocksize;
        nret = crypto_binary().decrypt_inplace(&doc->crypt, passwd, data, size, &block);
        if (nret != BQ::OK) {
            break;
        }
    } while (false);

#if _DEBUG
    if (nret == BQ::OK) {
        Hash md5("md5");
        auto msg = md5.add(data + offset_debug, size - offset_debug).finalize().bytes();
        MESSAGE_WARN(ctx, "[msdoc::decrypt] %s %lu bytes, skip header(%lu bytes): %s\r\n",
                         XsUtils::u16s2utf(name).c_str(), size, offset_debug, XsUtils::hex2str(msg, 16).c_str());
    }
#endif
    return nret;
}

int msdoc::extract(const cfb_ctx_t *ctx, msdoc_t *doc, ifilehandler *cb_file, const char16_t *passwd) {
    if (ctx == nullptr || doc == nullptr || doc->file == nullptr || cb_file == nullptr) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    auto ole = (olefile_t*)doc;

    /* ObjectPool */
    do {
        if (ole->entry_tree == nullptr || ctx->extract_objectpool == false) {
            break;
        }

        auto pool_stg = ole->entry_tree + doc->did_objectpool;
        if (pool_stg == nullptr) {
            break;
        }

        std::vector<objectpool_item_t> objectpool;
        for (auto cur_stg = pool_stg->child_first; cur_stg; cur_stg = cur_stg->slibing_next) {
            objectpool_item_t obj_item;
            nret = ms_embedded().parse_doc_objectpool_stg(ctx, ole, cur_stg, &obj_item, is_little_endian);
            if (nret != BQ::OK) {
                continue;
            }
            objectpool.push_back(obj_item);
        }

        nret = BQ::OK;
        for (auto iter = objectpool.begin(); iter != objectpool.end(); iter++) {
            if (nullptr == iter->object.obj.file) {
                continue;
            }

            if (0 > cb_file->handle(ctx, (file_obj_t*)&iter->object)) {
                nret = BQ::ERR_USER_INTERRUPT;
            }

            if (nret != BQ::OK) {
                break;
            }
        }
    } while (false);

    if (nret != BQ::OK) {
        return nret;
    }

    do {
#if _DEBUG
        uint8_t *data = nullptr;
        uint32_t size = 0;
        ON_SCOPE_EXIT([&] { if (data) free(data); });

        nret = ole::read_stream(ole, doc->did_wordocument, -1, &data, &size);
        if (nret != BQ::OK) {
            break;
        }

        if (doc->crypt.type > encryinfo_unknown && doc->crypt.type < encryinfo_ignore) {
            nret = msdoc::decrypt(ctx, doc, passwd, (ole->entries + doc->did_wordocument)->name, data, size);
            if (nret != BQ::OK) {
                break;
            }
        }
#endif
        /* WordDocument : TODO */
    } while (false);

    if (nret != BQ::OK) {
        return nret;
    }

    do {
        /* Table : TODO */
    } while (false);

    do {
        /* Data : TODO */
    } while (false);

    do {
        /* EncryptedSummary : TODO */
    } while (false);

    return nret;
}