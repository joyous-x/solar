#pragma once
#include <vector>
#include "./ole.h"
#include "./cfb_base.h"
#include "../utils/utils.h"
#include "../utils/scope_guard.h"

class OleUtils {
    friend ole;
public:
    static inline bool little_endian(const ole2_header_t* header) { return (header) && (header)->byte_order == 0xFFFE; }

    /**
     * @brief check the office header whether legal or not
     *
     * @param header                The ole2 header metadata (an ole2-specific context struct)
     * @return bool
     */
    static bool check_ole_header(const ole2_header_t* header);

    /**
     * @brief walk sat table for the sid chain
     *
     * @param sat_table             the sat table
     * @param sat_count             element count in the sat table
     * @param start_sid             travel will start at this sid
     * @param[out] chain            sid chain
     * @param[in ] little_endian    bytes order in sat_table
     * @return int
     */
    static int make_sid_chain(const uint32_t* sat_table, uint32_t sat_count, uint32_t start_sid
        , std::vector<uint32_t>& chain, bool* little_endian = 0);

public:
    /**
     * @brief walk an ole2 property tree, calling the handler for each file found
     *
     * @param pfile                 the file interface need to be parsed
     * @param header                the ole2 header metadata (an ole2-specific context struct)
     * @param[out] msat                the msat table
     * @return int
     */
    static int make_msat_table(ifile* file, ole2_header_t* header, uint32_t** table, uint32_t* count);

    /**
     * @brief walk an ole2 property tree, calling the handler for each file found
     *
     * @param ctx                   the workflow context
     * @param pfile                 the file interface need to be parsed
     * @param header                the ole2 header metadata (an ole2-specific context struct)
     * @param msat_table            the msat table
     * @param msat_count            element count in the msat table
     * @param[out] table            the sat table
     * @param[out] count            element count in the sat table
     * @return int
     */
    static int make_sat_table(ifile* file, ole2_header_t* header, const uint32_t* msat_table
        , uint32_t msat_count, uint32_t** table, uint32_t* count);

    /**
     * @brief walk an ole2 property tree, calling the handler for each file found
     *
     * @param pfile                 the file interface need to be parsed
     * @param header                the ole2 header metadata (an ole2-specific context struct)
     * @param sat_table             the msat table
     * @param sat_count             element count in the msat table
     * @param[out] table            the ssat table
     * @param[out] count            element count in the ssat table
     * @return int
     */
    static int make_ssat_table(olefile_t* ole, uint32_t** table, uint32_t* count);

    /**
     * @brief walk sat table for the directory entries
     *
     * @param ctx                   the workflow context
     * @param pfile                 the file interface need to be parsed
     * @param header                the ole2 header metadata (an ole2-specific context struct)
     * @param sat_table             the sat table
     * @param sat_count             element count in the sat table
     * @param start_sid             travel will start at this sid
     * @param[out] chain            sid chain
     * @return int
     */
    static int make_directory_entries(ifile* file, ole2_header_t* header, const uint32_t* sat_table
        , uint32_t sat_count, ole2_directory_entry_t** entries, uint32_t* count);

public:
    /**
     * @brief correct byte order
     *
     * @param[in out] header
     * @param is_input_little_endian
     * @return int
     */
    static int correct_byte_order(ole2_header_t* header, bool is_input_little_endian = true);
    /**
     * @brief correct byte order
     *
     * @param[in out] header
     * @param is_input_little_endian
     * @return int
     */
    static int correct_byte_order(ole2_directory_entry_t* entry, bool is_input_little_endian = true);
    /**
     * @brief correct byte order
     *
     * @param[in out] entry
     * @param size
     * @param is_input_little_endian
     * @return int
     */
    static int correct_byte_order(int32_t* entry, int32_t size, bool is_input_little_endian = true);

protected:
    /**
     * @brief memset office sector( break up the sid chain)
     * @param ole
     * @param sid              read will start at this sid
     * @param mini_sector
     * @param val
     * @return int
     */
    static int memset_sector(olefile_t* ole, uint32_t sid, bool mini_sector, uint8_t val = 0);

    /**
     * @brief set office directory entry
     *
     * @param ole
     * @param did
     * @param name             new name ending with '\0'
     * @param stream_bytes     set the stream_bytes field of ole2_directory_entry_t
     * @param child_did        set the child_did field of ole2_directory_entry_t
     * @param l_sib_did        set the l_sib_did field of ole2_directory_entry_t
     * @param r_sib_did        set the r_sib_did field of ole2_directory_entry_t
     * @return int
     */
    static int set_directory_entry(olefile_t* ole, uint32_t did, const char16_t* name, uint64_t* stream_bytes, uint32_t* child_did, uint32_t* l_sib_did, uint32_t* r_sib_did);
};
