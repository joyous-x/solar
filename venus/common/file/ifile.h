#pragma once
#include <stdint.h>

class ianybase {
public:
    virtual int addref() = 0;
    virtual int release() = 0;
};

class ifile : public ianybase {
public:
    /*
    * [in ] offset: reading will begin at the position offset
    * [in/out] buf: buffer
    * [in ] size: buffer size
    * [out] readed: the length of the buffer used
    * [return] int: return 0 if success, otherwise fail
    */
    virtual int read(int64_t offset, uint8_t* buf, uint32_t size, uint32_t* readed) = 0;

    /*
    * [in ] offset: writing will begin at the position offset
    * [in ] buf: buffer
    * [in ] size: buffer size
    * [return] int: return 0 if success, otherwise fail
    */
    virtual int write(int64_t offset, const uint8_t* buf, uint32_t size) = 0;

    /*
    * [return] return bytes of the file if success, otherwise nullptr. 
    *    the pointer returned MUST not be released and it is unpredictable to keep this pointer after writing.
    */
    virtual const uint8_t* mapping() = 0;

    /*
    * [return] return -1 if fail, otherwise the stream's size
    */
    virtual int64_t size() = 0;

    /*
    * @describe  the file's alias.
    *    if don't have an alias, just return the name
    */
    virtual const char* alias() = 0;

    /*
    * @describe return file's fullpath
    */
    virtual const char* fullpath() = 0;
};

/*
* @description: describe a file object
*/
typedef struct __st_file_obj {
    ifile*               file;
    uint32_t             filetype;
    uint32_t             attr;
    const __st_file_obj* container;

    __st_file_obj() : file(0), filetype(0), attr(0), container(0) { }
    __st_file_obj(const __st_file_obj& v) 
        : file(v.file), filetype(v.filetype), attr(v.attr), container(v.container) { if (file) file->addref(); }
    __st_file_obj(ifile* i, uint32_t t, uint32_t a, const __st_file_obj* c)
        : file(i), filetype(t), attr(a), container(c) { if (file) file->addref(); }
    void operator = (const __st_file_obj& v) {
        file = v.file;
        filetype = v.filetype;
        attr = v.attr; 
        container = v.container;
        if (file) file->addref();
    }
    virtual ~__st_file_obj() { if (file) { file->release(); file = 0; } }
} file_obj_t;

/*
* @description: create a new file
*
* @return    a file named dir/[parent->alias() + '_'](name | alias | random_str)
*/
typedef ifile* (*file_creator_t)(const file_obj_t* parent, const char* dir, const char* name, const char* alias, const char* mode);

class izip : public ianybase {
public:
    virtual int32_t item_cnt() = 0;
    virtual const char* item_utf8path(int32_t id_based_zero) = 0;
    virtual int32_t get_item(int32_t id_based_zero, uint8_t** data, uint32_t* size) = 0;
    virtual int32_t add_item(int32_t id_based_zero, const uint8_t* data, uint32_t size) = 0;
    virtual int32_t del_item(int32_t id_based_zero) = 0;
};

/*
* @description: create a zip parser
*/
typedef izip* (*zip_creator_t)(ifile* file, bool write_mode, void* user_data);