#include <cassert>
#include "ms_xls.h"
#include "xlm.h"
#include "./offcrypto.h"
#include "./helper.h"
#include "./ms_oleds.h"

const char16_t* sn_book = u"Book";
const char16_t* sn_workbook = u"WorkBook";

int parse_xlm_filepass_record(uint8_t* data, uint32_t size, bool is_little_endian, bin_encryption_t* encrypt) {
    if (data == nullptr || size < 4 || nullptr == encrypt) {
        return BQ::INVALID_ARG;
    }

    uint16_t encryption_type = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)data, is_little_endian);
    if (0x0000 == encryption_type) {
        encrypt->type = encryinfo_bin_xor_m1;
        auto xor_ = &encrypt->pass.xor_;
        xor_->key = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + 2), is_little_endian);
        xor_->verification_bytes = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + 4), is_little_endian);
        return BQ::OK;
    }

    uint32_t cur_offset = sizeof(encryption_type);
    return crypto_binary().parse_rc4_encryption_header(data + cur_offset, size - cur_offset, is_little_endian, encrypt);
}

/**
* @brief Scan for XLM (Excel 4.0) macro sheets and images in an OLE2 Workbook stream.
*
* The stream should be encoded with <= BIFF8
*/
int parse_workbook(uint8_t* data, uint32_t size, ms_workbook_t* xlm, bool little_endian) {
    if (data == nullptr || size == 0 || xlm == 0) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    biff::record_header_t rcd;

    for (uint32_t pos = 0, bof_count = 0, record_count = 0; pos + sizeof(biff::record_header_t) < size; ) {
        rcd.opcode = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + pos), little_endian);
        rcd.length = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + pos + sizeof(rcd.opcode)), little_endian);
        pos += sizeof(biff::record_header_t);

        if (rcd.length > BIFF8_MAX_RECORD_DATA_LENGTH || pos + rcd.length >= size) {
            break;
        }

        uint8_t* content = data + pos;
        uint32_t content_size = rcd.length;

        switch (rcd.opcode) {
        case biff::OPC_BOF: {
            if (content_size < 8) {
                break;
            }
            if (biff::BT_MACRO_SHEET == XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(content + 2), little_endian)) {
                xlm->has_macrosheet = true;
            }
            break;
        }
        case biff::OPC_FilePass: {
            xlm->encrypted = true;

            nret = parse_xlm_filepass_record(content, content_size, little_endian, &xlm->crypt);
            if (nret != BQ::OK) {
                break;
            }
            break;
        }
        case biff::OPC_MsoDrawingGroup: {
            xlm->has_drawing = true;
            break;
        }
        }

        pos += content_size;
        if (nret != BQ::OK || (xlm->has_drawing && xlm->encrypted && xlm->has_macrosheet)) {
            break;
        }
    }

    return nret;
}

int msxls::parse(const cfb_ctx_t *ctx, const olefile_t *olefile, msxls_t *xls) {
    if (olefile == nullptr || xls == nullptr) {
        return BQ::INVALID_ARG;
    }

    for (uint32_t i = 0; i < olefile->entry_count; i++) {
        auto cur_entry = (olefile->entry_tree + i);
        if (cur_entry == nullptr || cur_entry->depth > 1) {
            continue;
        }
        if (0 == ucs::icmp<char16_t>(cur_entry->item->name, sn_book) || 0 == ucs::icmp<char16_t>(cur_entry->item->name, sn_workbook)) {
            xls->workbook.did = i;
            break;
        }
    }
    if (xls->workbook.did == 0) {
        return BQ::ERR_FILE_FORMAT;
    }

    uint8_t buffer[8] = {0};
    int nret = ole::read_stream(olefile, xls->workbook.did, buffer, sizeof(buffer), 0);
    if (nret != BQ::OK) {
        return nret;
    }

    xls->workbook.biff_version = biff::version(buffer, sizeof(buffer), 0, msxls::is_little_endian);
    if (xls->workbook.biff_version == biff::unknown) {
        return BQ::ERR_FORMAT_XLM;
    }

    do {
        uint8_t* data = 0;
        uint32_t size = 0;

        ON_SCOPE_EXIT([&] { if (data) free(data); });
        nret = ole::read_stream(olefile, xls->workbook.did, -1, &data, &size);
        if (nret != BQ::OK) {
            break;
        }

        nret = parse_workbook(data, size, &xls->workbook, is_little_endian);
        if (nret != BQ::OK) {
            break;
        }
    } while (false);

    return nret;
}

int msxls::decrypt(const cfb_ctx_t* ctx, const msxls_t* xls, const char16_t* passwd, const char16_t* name, uint8_t* data, uint32_t size) {
    if (ctx == 0 || xls == 0 || xls->file == 0 || 0 == data || 0 == size || !xls->encrypted()) {
        return BQ::INVALID_ARG;
    }

    if (0 != ucs::icmp(name, sn_workbook) || 0 == ucs::icmp(name, sn_book)) {
        return BQ::NOT_IMPLEMENT;
    }

    int nret = BQ::OK;
    do {
        if (xls->workbook.crypt.type == encryinfo_unknown) {
            nret = BQ::ERR_OFFICE_CRYPT_INFO;
            break;
        }
        passwd = passwd ? passwd : excel_passwd_default;

        nret = crypto_binary().passwd_verify(&xls->workbook.crypt, passwd);
        if (nret != BQ::OK) {
            break;
        }

        nret = xlm().decrypt_xlm_inplace(&xls->workbook.crypt, passwd, data, size, is_little_endian);
        if (nret != BQ::OK) {
            return nret;
        }
    } while (false);

    return nret;
}

int extract_workbook(const cfb_ctx_t *ctx, const msxls_t *xls, uint8_t *data, uint32_t size, ms_workbook_t *workbook, ifilehandler *cb) {
    if (ctx == nullptr || data == nullptr || size == 0 || xls == nullptr || xls->file == nullptr) {
        return BQ::INVALID_ARG;
    }

    auto ole = (const olefile_t*)xls;
    auto cur_did = xls->workbook.did;
    auto cur_streamname = ole->entries ? (ole->entries + xls->workbook.did)->name : u"";
    int nret = BQ::OK;

    do {
        if (ctx->file_creator == nullptr) {
            break;
        }

        ifile* drawings = nullptr;
        ON_SCOPE_EXIT([&]() { drawings&& drawings->release(); });

        if (ctx->extract_xlm_drawings) {
            std::string streamname = XsUtils::u16s2utf(cur_streamname);
            drawings = (ctx->file_creator)(ole, ctx->temp_dirpath, 0, (streamname + "_drawgroup").c_str(), 0);
            if (nullptr == drawings) {
                nret = BQ::ERR_FILE_CREATOR;
                break;
            }
        }

        nret = xlm().parse_xlm(ctx, data, size, ole, workbook, drawings);
        if (nret != BQ::OK) {
            break;
        }

        for (size_t i = 0; i < workbook->boundsheet_cnt; i++) {
            auto bs = workbook->boundsheets + i;
            if (nullptr == bs->file) {
                continue;
            }

            embedded_object_t fileobj{ file_obj_t{bs->file, ft_xl4_macros, 0, ole}, cur_did, bs->lbPlyPos };
            if (0 > cb->handle(ctx, (const file_obj_t*)&fileobj)) {
                nret = BQ::ERR_USER_INTERRUPT;
            }
        }

        if (nret != BQ::OK) {
            break;
        }

        if (drawings) {
            /* extract images */
            embedded_object_t fileobj{ file_obj_t{drawings, ft_xl4_drawingroup, 0, ole}, cur_did, 0 };
            /*
             * A drawing group was extracted, now we need to find all the images inside.
             * If we fail to extract images, that's fine.
             */
            xlm().extract_images_from_drawing_group(ctx, (file_obj_t*)&fileobj, cb);

            drawings = nullptr;
        }
    } while (false);

    return nret;
}

int msxls::extract(const cfb_ctx_t *ctx, msxls_t *xls, ifilehandler *cb_file, const char16_t *passwd) {
    if (xls == nullptr || xls->file == nullptr || cb_file == nullptr || xls->workbook.did == 0) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    auto ole = (const olefile_t*)xls;

    if (ctx && false == ctx->try_decrypt && xls->workbook.encrypted) {
        return BQ::ERR_ENCRYPTED;
    }

    /* xlm */
    do {
        if (ctx && false == ctx->extract_xlm) {
            break;
        }

        if (xls->workbook.has_macrosheet == false) {
            break;
        }

        uint8_t* data = 0;
        uint32_t size = 0;

        ON_SCOPE_EXIT([&] { if (data) free(data); });
        nret = ole::read_stream(ole, xls->workbook.did, -1, &data, &size);
        if (nret != BQ::OK) {
            break;
        }

        if (xls->workbook.encrypted) {
            auto entry = ole->entries + xls->workbook.did;
            if (entry == nullptr) {
                break;
            }

            nret = decrypt(ctx, xls, passwd, entry->name, data, size);
            if (nret != BQ::OK) {
                break;
            }
        }

        nret = extract_workbook(ctx, xls, data, size, &xls->workbook, cb_file);
        if (nret != BQ::OK) {
            break;
        }

        break;
    } while (false);

    if (nret != BQ::OK) {
        return nret;
    }

    /* MBD */
    do {
        if (ctx && false == ctx->extract_mbd) {
            break;
        }

        if (ole->entry_tree == nullptr) {
            break;
        }

        std::vector<mbd_item_t> mbdpool;
        for (auto cur_stg = ole->entry_tree->child_first; cur_stg; cur_stg = cur_stg->slibing_next) {
            if (cur_stg->item->obj_type != ole_dir_entry_storage || ucs::nicmp(cur_stg->item->name, u"MBD", 3)) {
                continue;
            }

            mbd_item_t obj_item;
            nret = ms_embedded().parse_xls_mbd_stg(ctx, ole, cur_stg, &obj_item, is_little_endian);
            if (nret != BQ::OK) {
                MESSAGE_DEBUG(ctx, "[msxls::extract] parse %s as mbd failed(%08X) \n", XsUtils::u16s2utf(cur_stg->item->name).c_str(), nret)
                continue;
            }
            mbdpool.push_back(obj_item);
        }

        nret = BQ::OK;
        for (auto iter = mbdpool.begin(); iter != mbdpool.end(); iter++) {
            if (nullptr == iter->object.obj.file) {
                continue;
            }

            if (0 > cb_file->handle(ctx, (const file_obj_t*)&iter->object)) {
                nret = BQ::ERR_USER_INTERRUPT;
            }

            if (nret != BQ::OK) {
                break;
            }
        }
    } while (false);

    return nret;
}