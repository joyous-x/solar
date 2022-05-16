#include <cassert>
#include "offcrypto_assist.h"
#include "../3rds/base64.h"
#include "../3rds/tinyxml2_9.0.0/tinyxml2.h"
#include "../utils/scope_guard.h"
#include "ole_utils.h"

int crypto_ds::is_ole_encrypted(const olefile_t* ole, crypto_dataspace_t* data) {
    if (ole == nullptr) {
        return BQ::INVALID_ARG;
    }

    entry_tree_item_t* encryption_info = nullptr;
    entry_tree_item_t* encrypted_pkg = nullptr;
    entry_tree_item_t* dataspace_node = nullptr;
    entry_tree_item_t* transform_node = nullptr;

    for (uint32_t i = 0; i < ole->entry_count; i++) {
        entry_tree_item_t* cur_node = ole->entry_tree + i;
        if (cur_node == nullptr || cur_node->depth != 1 || nullptr == cur_node->item) {
            continue;
        }

        if (encryption_info && encrypted_pkg && dataspace_node && transform_node) {
            break;
        }

        if (0 == ucs::icmp<char16_t>(cur_node->item->name, u"EncryptionInfo")) {
            encryption_info = cur_node;
        } else if (0 == ucs::icmp<char16_t>(cur_node->item->name, u"EncryptedPackage")) {
            encrypted_pkg = cur_node;
        } else if (0 != ucs::icmp<char16_t>(cur_node->item->name, u"\006DataSpaces")) {
            dataspace_node = cur_node;

            for (auto next_node = dataspace_node->child_first; next_node; next_node = next_node->slibing_next) {
                if (0 == ucs::icmp(next_node->item->name, u"TransformInfo")) {
                    transform_node = next_node;
                    break;
                }
            }
        }
    }

    bool has_encrypted = (nullptr != encryption_info && nullptr != encrypted_pkg);
    if (has_encrypted && data) {
        data->encrypt_info_did = ((uint8_t*)encryption_info - (uint8_t*)ole->entry_tree) / sizeof(entry_tree_item_t);
        data->encrypt_pkg_did = ((uint8_t*)encrypted_pkg - (uint8_t*)ole->entry_tree) / sizeof(entry_tree_item_t);
        if (dataspace_node && transform_node) {
            data->ds_did = ((uint8_t*)dataspace_node - (uint8_t*)ole->entry_tree) / sizeof(entry_tree_item_t);
            data->ds_tran_did = ((uint8_t*)transform_node - (uint8_t*)ole->entry_tree) / sizeof(entry_tree_item_t);

            for (auto next_node = dataspace_node->child_first; nullptr != next_node; next_node = next_node->slibing_next) {
                if (0 == ucs::icmp(next_node->item->name, u"DataSpaceInfo")) {
                    data->ds_info_did = ((uint8_t*)next_node - (uint8_t*)ole->entry_tree) / sizeof(entry_tree_item_t);
                }
                else if (0 == ucs::icmp(next_node->item->name, u"DataSpaceMap")) {
                    data->ds_map_did = ((uint8_t*)next_node - (uint8_t*)ole->entry_tree) / sizeof(entry_tree_item_t);
                }
                else if (0 == ucs::icmp(next_node->item->name, u"Version")) {
                    data->ds_ver_did = ((uint8_t*)next_node - (uint8_t*)ole->entry_tree) / sizeof(entry_tree_item_t);
                }
            }
        }
    }
    return BQ::OK;
}

int crypto_ds::parse_stream_ds_version(const olefile_t* ole, uint32_t version_did, ds_version_t* version) {
    uint8_t* buffer = 0;
    uint32_t size = 0;

    ON_SCOPE_EXIT([&] { if (buffer) free(buffer); });
    int nret = ole::read_stream(ole, version_did, -1, &buffer, &size);
    if (nret != BQ::OK) {
        return nret;
    }

    uint32_t length = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)buffer, is_little_endian);
    uint32_t padding = (4 - (length % 4)) % 4;
    const char16_t* expected = u"Microsoft.Container.DataSpaces";

    /* FeatureIdentifier, must be "Microsoft.Container.DataSpaces" */
    if (0 != ucs::nicmp((const char16_t*)(buffer + sizeof(uint32_t)), expected, ucs::len<char16_t>(expected))) {
        return BQ::ERR_OFFICE_CRYPT_FORMAT;
    }

    uint32_t ver_offset = length + sizeof(uint32_t) + padding;
    if (size - ver_offset < sizeof(ds_version_t)) {
        return BQ::ERR_FORMAT;
    }

    memcpy((void*)version, buffer + ver_offset, sizeof(ds_version_t));
    version->reader.major = XsUtils::byteorder2host<uint16_t>(version->reader.major, is_little_endian);
    version->reader.minor = XsUtils::byteorder2host<uint16_t>(version->reader.minor, is_little_endian);
    version->updater.major = XsUtils::byteorder2host<uint16_t>(version->updater.major, is_little_endian);
    version->updater.minor = XsUtils::byteorder2host<uint16_t>(version->updater.minor, is_little_endian);
    version->writer.major = XsUtils::byteorder2host<uint16_t>(version->writer.major, is_little_endian);
    version->writer.minor = XsUtils::byteorder2host<uint16_t>(version->writer.minor, is_little_endian);
    assert(version->reader.major == 1 && version->reader.minor == 0);
    assert(version->updater.major == 1 && version->updater.minor == 0);
    assert(version->writer.major == 1 && version->writer.minor == 0);

    return BQ::OK;
}

int crypto_ds::parse_stream_ds_map(const olefile_t* ole, uint32_t ds_map_did, ds_map_t* map) {
    uint8_t* buffer = 0;
    uint32_t size = 0;

    ON_SCOPE_EXIT([&] { if (buffer) free(buffer); });
    int nret = ole::read_stream(ole, ds_map_did, -1, &buffer, &size);
    if (nret != BQ::OK) {
        return nret;
    }

    map->header_length = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)buffer, is_little_endian);
    map->entry_count = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(buffer+4), is_little_endian);

    const uint32_t map_entry_size_min = sizeof(uint32_t) + sizeof(uint32_t);
    for (uint32_t offset = map_entry_size_min, pos_in_entry = 0; offset + 8 < size;) {
        ds_mapentry_t entry_tmp;
        entry_tmp.length = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(buffer + offset), is_little_endian);
        entry_tmp.component_count = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(buffer + offset + 4), is_little_endian);

        if (offset + entry_tmp.length > size) {
            break;
        }

        pos_in_entry = 8;
        for (uint32_t i = 0, comp_name_bytes = 0; pos_in_entry + 8 <= entry_tmp.length && i < entry_tmp.component_count; i++) {
            const uint8_t* cur_comp = buffer + offset + pos_in_entry;
            uint32_t cur_comp_size = entry_tmp.length - pos_in_entry;

            ds_reference_component_t comp_tmp;
            comp_tmp.type = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)cur_comp, is_little_endian);

            comp_name_bytes = 0;
            if (BQ::OK != crypto_helper::parse_unicode_lp_p4(cur_comp + 4, cur_comp_size - 4, comp_tmp.name, &comp_name_bytes)) {
                offset = size;
                break;
            }

            entry_tmp.components.push_back(comp_tmp);
            pos_in_entry += 4 + comp_name_bytes;
        }

        if (BQ::OK != crypto_helper::parse_unicode_lp_p4(buffer + offset + pos_in_entry, entry_tmp.length - pos_in_entry, entry_tmp.name)) {
            break;
        }

        map->entries.push_back(entry_tmp);
        offset += entry_tmp.length;
    }

    return nret;
}

int crypto_ds::parse_stream_ds_definition(const olefile_t* ole, uint32_t ds_info_did, const char16_t* entry, ds_definition_t* definition) {
    if (entry == nullptr || definition == nullptr) {
        return BQ::INVALID_ARG;
    }

    entry_tree_item_t* ds_info_node = ole->entry_tree + ds_info_did;
    if (ds_info_node == nullptr || ds_info_node->item->obj_type != ole_dir_entry_storage) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    const entry_tree_item_t* cur_node = nullptr;

    for (auto next_node = ds_info_node->child_first; nullptr != next_node; next_node = next_node->slibing_next) {
        if (0 == ucs::icmp(next_node->item->name, entry)) {
            cur_node = next_node;
            break;
        }
    }

    if (cur_node == nullptr) {
        return BQ::INVALID_ARG;
    }

    definition->name = entry;
    uint32_t cur_info_did = ((uint8_t*)cur_node - (uint8_t*)ole->entry_tree) / sizeof(entry_tree_item_t);

    do {
        uint8_t* buffer = 0;
        uint32_t size = 0;

        ON_SCOPE_EXIT([&] { if (buffer) free(buffer); });
        nret = ole::read_stream(ole, cur_info_did, -1, &buffer, &size);
        if (nret != BQ::OK) {
            break;
        }

        definition->header_length = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)buffer, is_little_endian);
        definition->transform_count = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(buffer + 4), is_little_endian);

        for (uint32_t pos = 8, trans_bytes = 0; pos + 4 <= size; pos += trans_bytes) {
            std::u16string tmp;
            if (BQ::OK != crypto_helper::parse_unicode_lp_p4(buffer + pos, size - pos, tmp, &trans_bytes)) {
                break;
            }
            definition->transforms.push_back(tmp);
        }
    } while (false);

    return nret;
}

int crypto_ds::parse_stream_ds_transform(const olefile_t* ole, uint32_t ds_tran_did, const char16_t* name, ds_transform_t* transform) {
    if (name == nullptr || transform == nullptr) {
        return BQ::INVALID_ARG;
    }

    entry_tree_item_t* ds_node = ole->entry_tree + ds_tran_did;
    if (ds_node == nullptr || ds_node->item->obj_type != ole_dir_entry_storage) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    const entry_tree_item_t* cur_storage_node = nullptr;
    const entry_tree_item_t* cur_stream_node = nullptr;
    const char16_t* cur_stream_name = u"\006Primary";

    for (auto next_node = ds_node->child_first; nullptr != next_node; next_node = next_node->slibing_next) {
        if (0 == ucs::icmp(next_node->item->name, name)) {
            cur_storage_node = next_node;
            break;
        }
    }

    if (cur_storage_node == nullptr) {
        return BQ::INVALID_ARG;
    }

    for (auto next_node = cur_storage_node->child_first; nullptr != next_node; next_node = next_node->slibing_next) {
        if (0 == ucs::icmp(next_node->item->name, cur_stream_name)) {
            cur_stream_node = next_node;
            break;
        }
    }

    if (cur_stream_node == nullptr) {
        return BQ::INVALID_ARG;
    }

    uint32_t cur_stream_did = ((uint8_t*)cur_stream_node - (uint8_t*)ole->entry_tree) / sizeof(entry_tree_item_t);
    transform->name = name;
    auto irmds_info = &transform->transform;
    auto encrypt_tran = &transform->encrypt_tran;
    const uint32_t encryption_tran_info_min_bytes = 16;

    do {
        uint8_t* buffer = 0;
        uint32_t size = 0;

        ON_SCOPE_EXIT([&] { if (buffer) free(buffer); });
        nret = ole::read_stream(ole, cur_stream_did, -1, &buffer, &size);
        if (nret != BQ::OK) {
            break;
        }

        nret = BQ::ERR_FORMAT;
        irmds_info->header.length = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)buffer, is_little_endian);
        irmds_info->header.type = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(buffer + 4), is_little_endian);
        if (size < irmds_info->header.length + sizeof(ds_version_t)) {
            break;
        }

        uint32_t cur_pos = 8;
        uint32_t lp_p4_bytes = 0;
        if (BQ::OK != crypto_helper::parse_unicode_lp_p4(buffer + cur_pos, irmds_info->header.length - cur_pos, irmds_info->header.id, &lp_p4_bytes)) {
            break;
        }
        if (0 != ucs::cmp(irmds_info->header.id.c_str(), u"{FF9A3F03-56EF-4613-BDD5-5A41C1D07246}")) {
            nret = BQ::ERR_OFFICE_CRYPT_INFO_FMT;
            break;
        }

        cur_pos += lp_p4_bytes;
        if (BQ::OK != crypto_helper::parse_unicode_lp_p4(buffer + cur_pos, size - cur_pos, irmds_info->header.name, &lp_p4_bytes)) {
            break;
        }
        assert(0 == ucs::cmp(irmds_info->header.name.c_str(), u"Microsoft.Container.EncryptionTransform"));

        cur_pos += lp_p4_bytes;
        if (size < cur_pos + sizeof(ds_version_t) + 4) {
            break;
        }

        memcpy((void*)&(irmds_info->header.ver), buffer + cur_pos, sizeof(ds_version_t));
        auto version = &irmds_info->header.ver;
        version->reader.major = XsUtils::byteorder2host<uint16_t>(version->reader.major, is_little_endian);
        version->reader.minor = XsUtils::byteorder2host<uint16_t>(version->reader.minor, is_little_endian);
        version->updater.major = XsUtils::byteorder2host<uint16_t>(version->updater.major, is_little_endian);
        version->updater.minor = XsUtils::byteorder2host<uint16_t>(version->updater.minor, is_little_endian);
        version->writer.major = XsUtils::byteorder2host<uint16_t>(version->writer.major, is_little_endian);
        version->writer.minor = XsUtils::byteorder2host<uint16_t>(version->writer.minor, is_little_endian);

        cur_pos += sizeof(ds_version_t);
        irmds_info->extensibility_header.length = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(buffer + cur_pos), is_little_endian);

        /* something is wrong in [MS-OFFCRYPTO] - v20210817. we should try to ignore extensibility_header. */
        if (irmds_info->extensibility_header.length == 0x4) {
            cur_pos += sizeof(uint32_t);

            if (size < cur_pos + encryption_tran_info_min_bytes) {
                break;
            }

            if (BQ::OK != crypto_helper::parse_utf8_lp_p4(buffer + cur_pos, size - cur_pos, encrypt_tran->name, &lp_p4_bytes)) {
                cur_pos -= sizeof(uint32_t);

                if (BQ::OK != crypto_helper::parse_utf8_lp_p4(buffer + cur_pos, size - cur_pos, encrypt_tran->name, &lp_p4_bytes)) {
                    break;
                }
            }
        }
        else {
            if (size < cur_pos + encryption_tran_info_min_bytes) {
                break;
            }

            if (BQ::OK != crypto_helper::parse_utf8_lp_p4(buffer + cur_pos, size - cur_pos, encrypt_tran->name, &lp_p4_bytes)) {
                break;
            }
        }

        cur_pos += lp_p4_bytes;
        encrypt_tran->block_size = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(buffer + cur_pos), is_little_endian);
        encrypt_tran->cipher_mode = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(buffer + cur_pos + 4), is_little_endian);
        encrypt_tran->reserved = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(buffer + cur_pos + 8), is_little_endian);
        if (0x00000004 != encrypt_tran->reserved) {
            /*
            * TODO:
            *   [irmds_info->header.name + irmds_info->header.ver] appears 2 times.
            */
            nret = BQ::ERR_OFFICE_CRYPT_INFO_FMT;
            break;
        }

        nret = BQ::OK;
    } while (false);

    return nret;
}

int crypto_ds::parse_stream_encryption_info(const olefile_t* ole, uint32_t encryption_info_did, encryption_info_t* encrypt_info) {
    if (encrypt_info == nullptr || ole == nullptr) {
        return BQ::INVALID_ARG;
    }

    entry_tree_item_t* info_node = ole->entry_tree + encryption_info_did;
    if (info_node == nullptr || info_node->item->obj_type != ole_dir_entry_stream) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    do {
        uint8_t* buffer = 0;
        uint32_t size = 0;

        ON_SCOPE_EXIT([&] { if (buffer) free(buffer); });
        nret = ole::read_stream(ole, encryption_info_did, -1, &buffer, &size);
        if (nret != BQ::OK) {
            break;
        }

        if (size < sizeof(encryption_info_base_t) + 4) {
            nret = BQ::ERR_OFFICE_CRYPT_INFO_FMT;
            break;
        }

        memcpy(&encrypt_info->base, buffer, sizeof(encryption_info_base_t));
        encrypt_info->base.version.major = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)buffer, is_little_endian);
        encrypt_info->base.version.minor = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(buffer+2), is_little_endian);
        encrypt_info->base.flags = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(buffer + 4), is_little_endian);

        if (encryinfo_ecma376_std == encrypt_info->type()) {
            uint32_t cur_offset = sizeof(encryption_info_base_t);

            auto encrypt = &encrypt_info->std;
            encrypt->reset();
            encrypt->header_size = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(buffer + cur_offset), is_little_endian);
            cur_offset += 4;

            nret = crypto_helper::read_encryption_header(buffer + cur_offset, size - cur_offset, encrypt->header_size, &encrypt->header, is_little_endian);
            if (nret != BQ::OK) {
                break;
            }
            cur_offset += encrypt->header_size;

            nret = crypto_helper::read_encryption_verifier(buffer + cur_offset, size - cur_offset, &encrypt->verifier, is_little_endian);
            if (nret != BQ::OK) {
                break;
            }
        }
        else if (encryinfo_ecma376_agile == encrypt_info->type()) {
            uint32_t cur_offset = sizeof(encryption_info_base_t);

            auto encrypt = &encrypt_info->agile;
            encrypt->reset();

            tinyxml2::XMLDocument xml_doc;
            nret = xml_doc.Parse((const char*)(buffer + cur_offset), size - cur_offset);
            if (nret != tinyxml2::XML_SUCCESS) {
                nret = BQ::ERR_PARSE_XML;
                break;
            }
            
            const tinyxml2::XMLElement* key_data = xml_doc.RootElement()->FirstChildElement("keyData");
            const tinyxml2::XMLElement* data_integrity = xml_doc.RootElement()->FirstChildElement("dataIntegrity");
            const tinyxml2::XMLElement* encrypted_key = nullptr;
            const tinyxml2::XMLElement* key_encryptors = xml_doc.RootElement()->FirstChildElement("keyEncryptors");
            if (key_encryptors && key_encryptors->FirstChildElement("keyEncryptor")) {
                auto keyEncryptor = key_encryptors->FirstChildElement("keyEncryptor");
                encrypted_key = keyEncryptor->FirstChildElement("encryptedKey");
                if (nullptr == encrypted_key) {
                    encrypted_key = keyEncryptor->FirstChildElement("p:encryptedKey");
                }
            }
            
            if (encrypted_key == nullptr || key_data == nullptr) {
                nret = BQ::ERR_OFFICE_CRYPT_INFO_FMT;
                break;
            }

            auto copy_str = [](char* d, uint32_t size, const char* v)  {
                auto v_size = v ? strlen(v) + 1 : 0;
                if (d && size >= v_size) {
                    memset((void*)d, 0, size);
                    memcpy(d, v, v_size);
                    return 0;
                }
                return -1;
            };

            auto clone_str = [](const char* v) -> char* {
                if (v == nullptr) {
                    return nullptr;
                }
                auto size = strlen(v);
                auto buff = (char*)malloc(size + 1);
                if (buff != nullptr) {
                    memcpy(buff, v, size + 1);
                }
                return buff;
            };

            if (key_data) {
                encrypt->keydata.saltsize = key_data->UnsignedAttribute("saltSize", 0);
                encrypt->keydata.blocksize = key_data->UnsignedAttribute("blockSize", 0);
                encrypt->keydata.keybits = key_data->UnsignedAttribute("keyBits", 0);
                encrypt->keydata.hashsize = key_data->UnsignedAttribute("hashSize", 0);
                copy_str(encrypt->keydata.cipher_algor, sizeof(encrypt->keydata.cipher_algor), key_data->Attribute("cipherAlgorithm"));
                copy_str(encrypt->keydata.cipher_chain, sizeof(encrypt->keydata.cipher_chain), key_data->Attribute("cipherChaining"));
                copy_str(encrypt->keydata.hash_algor, sizeof(encrypt->keydata.hash_algor), key_data->Attribute("hashAlgorithm"));

                auto tmp_val = key_data->Attribute("saltValue");
                if (nullptr != tmp_val) {
                    size_t out_size = 0;
                    encrypt->keydata.salt = base64_decode((const unsigned char*)tmp_val, strlen(tmp_val), &out_size);
                    if (out_size != encrypt->keydata.saltsize) {
                        ON_SCOPE_EXIT([&]() { if (encrypt->keydata.salt) free(encrypt->keydata.salt); encrypt->keydata.salt = nullptr; });
                    }
                }
            }

            if (data_integrity) {
                encrypt->integrity.encrypted_hmac_key = clone_str(data_integrity->Attribute("encryptedHmacKey"));
                encrypt->integrity.encrypted_hmac_val = clone_str(data_integrity->Attribute("encryptedHmacValue"));
            }

            if (encrypted_key) {
                auto& data = encrypt->encryptor;
                data.spin_cnt = encrypted_key->UnsignedAttribute("spinCount", 0);
                data.saltsize = encrypted_key->UnsignedAttribute("saltSize", 0);
                data.blocksize = encrypted_key->UnsignedAttribute("blockSize", 0);
                data.keybits = encrypted_key->UnsignedAttribute("keyBits", 0);
                data.hashsize = encrypted_key->UnsignedAttribute("hashSize", 0);
                copy_str(data.cipher_algor, sizeof(encrypt->encryptor.cipher_algor), encrypted_key->Attribute("cipherAlgorithm"));
                copy_str(data.cipher_chain, sizeof(encrypt->encryptor.cipher_chain), encrypted_key->Attribute("cipherChaining"));
                copy_str(data.hash_algor, sizeof(encrypt->encryptor.hash_algor), encrypted_key->Attribute("hashAlgorithm"));

                auto tmp_val = encrypted_key->Attribute("saltValue");
                if (nullptr != tmp_val) {
                    size_t out_size = 0;
                    data.salt = base64_decode((const unsigned char*)tmp_val, strlen(tmp_val), &out_size);
                    if (out_size != data.saltsize) {
                        ON_SCOPE_EXIT([&]() { if (data.salt) free(data.salt); data.salt = nullptr; });
                    }
                }

                data.encrypted_keyval = clone_str(encrypted_key->Attribute("encryptedKeyValue"));
                data.encrypted_verifier_hash_input = clone_str(encrypted_key->Attribute("encryptedVerifierHashInput"));
                data.encrypted_verifier_hash_value = clone_str(encrypted_key->Attribute("encryptedVerifierHashValue"));
            }
        }
        else {
            nret = BQ::NOT_IMPLEMENT;
            break;
        }
    } while (false);

    return nret;
}

int crypto_ds::parse_dataspace_encryption(const olefile_t* ole, crypto_dataspace_t* data) {
    if (ole == nullptr || data == nullptr) {
        return BQ::INVALID_ARG;
    }

    int nret = is_ole_encrypted(ole, data);
    if (BQ::OK != nret) {
        return nret;
    }

    if (data->encrypted() == false) {
        return BQ::OK;
    }

    entry_tree_item_t* dataspace_node = ole->entry_tree + data->ds_did;
    entry_tree_item_t* transform_node = ole->entry_tree + data->ds_tran_did;
    entry_tree_item_t* version_node = ole->entry_tree + data->ds_ver_did;

    if (data->ds_ver_did != 0) {
        nret = parse_stream_ds_version(ole, data->ds_ver_did, &data->version);
        if (nret != BQ::OK) {
            return nret;
        }
    }

    if (data->ds_map_did != 0) {
        nret = parse_stream_ds_map(ole, data->ds_map_did, &data->map);
        if (nret != BQ::OK) {
            return nret;
        }
    }

    if (data->map.entries.size()) {
        entry_tree_item_t* ds_node = ole->entry_tree + data->ds_info_did;
        if (ds_node == nullptr || ds_node->item->obj_type != ole_dir_entry_storage) {
            return BQ::ERR_OFFICE_CRYPT_DS_INFO;
        }

        for (auto next_node = ds_node->child_first; nullptr != next_node; next_node = next_node->slibing_next) {
            ds_definition_t cur_definition;
            cur_definition.name = next_node->item->name;
            parse_stream_ds_definition(ole, data->ds_info_did, next_node->item->name, &cur_definition);
            data->definitions.push_back(cur_definition);
        }
    }

    if (data->definitions.size()) {
        entry_tree_item_t* ds_node = ole->entry_tree + data->ds_tran_did;
        if (ds_node == nullptr || ds_node->item->obj_type != ole_dir_entry_storage) {
            return BQ::ERR_OFFICE_CRYPT_DS_INFO;
        }

        for (auto next_node = ds_node->child_first; nullptr != next_node; next_node = next_node->slibing_next) {
            ds_transform_t cur_transform;
            cur_transform.name = next_node->item->name;
            parse_stream_ds_transform(ole, data->ds_tran_did, next_node->item->name, &cur_transform);
            data->transforms.push_back(cur_transform);
        }
    }

    return parse_stream_encryption_info(ole, data->encrypt_info_did, &data->encrypt_info);;
}

///////////////////////////////////////////////////////////////////
/// <summary>
///
/// </summary>
///////////////////////////////////////////////////////////////////

int crypto_helper::parse_unicode_lp_p4(const uint8_t* buffer, uint32_t size, std::u16string& val, uint32_t* raw_byte_size, bool is_little_endian) {
    if (buffer == nullptr || size < 4) {
        return BQ::INVALID_ARG;
    }

    uint32_t length = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(buffer), is_little_endian);
    uint32_t padding = (4 - (length % 4)) % 4;
    if (size < sizeof(uint32_t) + length) {
        return BQ::ERR_OFFICE_UNICODE_LP_P4;
    }
    assert(length % 2 == 0);

    char16_t* chars = (char16_t*)malloc(length + 4); /* in case of length % 2 != 0 */
    if (chars == nullptr) {
        return BQ::ERR_MALLOC;
    }
    ON_SCOPE_EXIT([&] { if (chars) free(chars); });

    memset(chars, 0, length + 4);
    memcpy(chars, buffer + 4, length);
    val = chars;
    if (raw_byte_size) {
        *raw_byte_size = length + padding + 4;
    }

    return BQ::OK;
}

int crypto_helper::parse_utf8_lp_p4(const uint8_t* buffer, uint32_t size, std::string& val, uint32_t* raw_byte_size, bool is_little_endian) {
    if (buffer == nullptr || size < 4) {
        return BQ::INVALID_ARG;
    }

    uint32_t length = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(buffer), is_little_endian);
    uint32_t padding = (4 - (length % 4)) % 4;
    if (size < sizeof(uint32_t) + length) {
        return BQ::ERR_OFFICE_UNICODE_LP_P4;
    }

    char* chars = (char*)malloc(length + 4);
    if (chars == nullptr) {
        return BQ::ERR_MALLOC;
    }
    ON_SCOPE_EXIT([&] { if (chars) free(chars); });

    memset(chars, 0, length + 4);
    memcpy(chars, buffer + 4, length);
    val = chars;
    if (raw_byte_size) {
        *raw_byte_size = length + padding + 4;
    }

    return BQ::OK;
}

int crypto_helper::read_encryption_header(const uint8_t* data, uint32_t size, uint32_t header_size, encryption_header_t* header, bool little_endian) {
    uint32_t header_size_min = (const uint8_t*)&header->csp_name - (const uint8_t*)&header->flags;

    if (size < header_size || header_size < header_size_min) {
        return BQ::ERR_OFFICE_CRYPT_INFO_FMT;
    }

    memset(header->csp_name, 0, sizeof(header->csp_name));
    memcpy(header, data, header_size_min);
    header->flags = XsUtils::byteorder2host<uint32_t>(header->flags, little_endian);
    header->size_extra = XsUtils::byteorder2host<uint32_t>(header->size_extra, little_endian);
    header->algid = XsUtils::byteorder2host<uint32_t>(header->algid, little_endian);
    header->algid_hash = XsUtils::byteorder2host<uint32_t>(header->algid_hash, little_endian);
    header->key_size = XsUtils::byteorder2host<uint32_t>(header->key_size, little_endian);
    header->provider_type = XsUtils::byteorder2host<uint32_t>(header->provider_type, little_endian);

    uint32_t csp_name_size_max = std::min<uint32_t>(header_size - header_size_min, sizeof(header->csp_name) - 2);
    memcpy(header->csp_name, data + header_size_min, csp_name_size_max);

    return BQ::OK;
}

int crypto_helper::read_encryption_verifier(const uint8_t* data, uint32_t size, encryption_verifier_t* verifier, bool little_endian) {
    uint32_t verifier_size_min = (const uint8_t*)&verifier->encrypted_verifier_hash - (const uint8_t*)verifier;

    if (size < verifier_size_min) {
        return BQ::ERR_OFFICE_CRYPT_INFO_FMT;
    }

    memset(&verifier->encrypted_verifier_hash, 0, sizeof(verifier->encrypted_verifier_hash));
    memcpy(verifier, data, verifier_size_min);
    verifier->salt_size = XsUtils::byteorder2host<uint32_t>(verifier->salt_size, little_endian);
    verifier->verifier_hash_size = XsUtils::byteorder2host<uint32_t>(verifier->verifier_hash_size, little_endian);

    if (size > verifier_size_min) {
        uint32_t verifier_hash_size = std::min<uint32_t>(size - verifier_size_min, sizeof(verifier->encrypted_verifier_hash));
        memcpy(verifier->encrypted_verifier_hash, data + verifier_size_min, verifier_hash_size);
    }

    return BQ::OK;
}
