#pragma once
#include <string>
#include "ifile.h"
#include "../utils/utils.h"

#if (_WIN32 || _WIN64)
#include <Windows.h>
#else
#define _fseeki64 fseeko
#define _ftelli64 ftello
#endif


class SimpleFile : public ifile {
public:
	SimpleFile() = default;
public:
	~SimpleFile();

	static SimpleFile* newfile() {
		return new SimpleFile();
	}

	int open(const char* filepath, const char* alias, const char* mode);
	int close();

public:
	virtual const uint8_t* mapping();
	virtual int read(int64_t offset, uint8_t* buf, uint32_t size, uint32_t* readed);
	virtual int write(int64_t offset, const uint8_t* buf, uint32_t size);
	virtual int64_t size();
	virtual const char* alias();
	virtual const char* fullpath();

	virtual int addref() {
		return ++m_count;
	};
	virtual int release() {
		if (m_count && --m_count == 0) { delete this; }
		return 0;
	};
private:
	int mmap();
	int munmap();

private:
	volatile int m_count{1};
	FILE* m_fptr{ nullptr };
    int64_t m_filesize{ -1 };
	std::string m_filepath;
	std::string m_alias;

    void*		m_pvFile{ nullptr };
	void*		m_pvMap{ nullptr };
#if (_WIN32 || _WIN64)
    HANDLE		m_hFile{ INVALID_HANDLE_VALUE };
	HANDLE		m_hFileMap{ NULL };
#endif
};
