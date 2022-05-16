#include <stdlib.h>
#include "simplefile.h"



SimpleFile::~SimpleFile()
{
    close();
}

int SimpleFile::open(const char* filepath, const char* alias, const char* mode)
{
    m_fptr = fopen(filepath, mode);
    if (nullptr == m_fptr) {
        return -1;
    }
    
    m_filepath = filepath;
    m_alias = alias ? alias : XsUtils::split_filename(filepath);
    m_filesize = size();

    return 0;
}

int SimpleFile::close()
{
    munmap();
    if (nullptr != m_fptr) {
        fclose(m_fptr), m_fptr = nullptr;
    }
    if (nullptr != m_pvMap) {
        free(m_pvMap), m_pvMap = nullptr;
    }
    m_filesize = -1;
    return 0;
}

int SimpleFile::mmap() {
#if (_WIN32 || _WIN64)
    m_hFile = ::CreateFileA(m_filepath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (m_hFile != INVALID_HANDLE_VALUE) {
        // 低字节，为0，取文件真实大小
        m_hFileMap = ::CreateFileMapping(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
        if (m_hFileMap != NULL) {
            // 必为分配粒度的整倍数,windows的粒度为64K
            m_pvFile = ::MapViewOfFile(m_hFileMap, FILE_MAP_READ, 0, 0, 0);
        }
    }

    if (m_pvFile == NULL) {
        munmap();
    }
    return m_pvFile == NULL ? -1 : 0;
#else
    return -1;
#endif
}

int SimpleFile::munmap() {
#if (_WIN32 || _WIN64)
    if (m_pvFile != NULL) {
        ::UnmapViewOfFile(m_pvFile);
        m_pvFile = NULL;
    }
    if (NULL != m_hFileMap) {
        ::CloseHandle(m_hFileMap);
        m_hFileMap = NULL;
    }
    if (INVALID_HANDLE_VALUE != m_hFile) {
        ::CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
#endif
    return 0;
}

const uint8_t* SimpleFile::mapping() {
    if (m_pvFile == NULL) {
        mmap();
    }

    if (m_pvFile != nullptr) {
        return (const uint8_t*)m_pvFile;
    }

    if (!m_pvMap) {
        auto filesize = (uint32_t)size();
        auto buffer = malloc(filesize);
        if (buffer == nullptr) {
            return nullptr;
        }
        
        int nret = read(0, (uint8_t*)buffer, filesize, 0);
        if (nret < 0) {
            free(buffer);
            return nullptr;
        }
        m_pvMap = buffer;
    }

    return (const uint8_t*)m_pvMap;
}

int SimpleFile::read(int64_t offset, uint8_t* buf, uint32_t size, uint32_t* readed)
{
    if (m_fptr == nullptr) {
        return -1;
    }

    int nRet = _fseeki64(m_fptr, offset, SEEK_SET);
    if (nRet != 0) {
        return nRet;
    }

    size_t hasRead = fread(buf, 1, size, m_fptr);
    if (readed != nullptr) {
        *readed = hasRead;
    }
    return hasRead == 0 ? -1 : 0;
}

int SimpleFile::write(int64_t offset, const uint8_t* buf, uint32_t size)
{
    if (m_fptr == nullptr) {
        return -1;
    }

    int nRet = 0;
    if (offset == -1) {
        nRet = _fseeki64(m_fptr, 0, SEEK_END);
    }
    else {
        nRet = _fseeki64(m_fptr, offset, SEEK_SET);
    }
    if (nRet != 0) {
        return nRet;
    }

    if (m_pvMap) { 
        free(m_pvMap), m_pvMap = nullptr;
    }

    auto processed = fwrite(buf, 1, size, m_fptr);
    fflush(m_fptr);

    return (int)(processed - size);
}

int64_t SimpleFile::size()
{
    auto tmp = _ftelli64(m_fptr);

    int nRet = _fseeki64(m_fptr, 0, SEEK_END);
    if (nRet != 0) {
        return -1;
    }
    m_filesize = _ftelli64(m_fptr);

    _fseeki64(m_fptr, tmp, SEEK_SET);

    return m_filesize;
}

const char* SimpleFile::fullpath() {
    return m_filepath.c_str();
}

const char* SimpleFile::alias() {
    return m_alias.c_str();
}
