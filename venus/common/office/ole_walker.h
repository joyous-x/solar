#pragma once
#include <vector>
#include "ole.h"

class OlePropertyHandler {
public:
    /**
     * @brief checks for VBA presence
     * @return int
     */
    static int walk_through_for_debug(const cfb_ctx_t* ctx, olefile_t* ole, uint32_t directory_id, ifilehandler* handler);

    /**
     * @brief travel for files in office
     * @return int
     */
    static int walk_through_files(const cfb_ctx_t* ctx, olefile_t* ole, uint32_t directory_id, ifilehandler* handler);

protected:
};