#pragma once
#include <string>
#include "ifile.h"

class MemFile : public ifile {
public:
	MemFile(const char* fullpath, const char* alias, uint32_t max_size = 10 * 1024 * 1024) : m_max_size(max_size) {
		m_alias = alias == nullptr ? "" : alias;
		m_fullpath = fullpath == nullptr ? "" : fullpath;
	}
	~MemFile() {
		if (m_data) { 
			free(m_data), m_data = nullptr;
		}
		m_size = 0;
	}
public:
	virtual const uint8_t* mapping() { 
		return m_data; 
	}
	virtual int read(int64_t offset, uint8_t* buf, uint32_t size, uint32_t* readed) {
		if (offset >= m_size) { return -1; }
		uint32_t real_readed = std::min<uint32_t>(m_size - (uint32_t)offset, size);
		memcpy(buf, m_data + offset, real_readed);
		if (readed) { *readed = real_readed; }
		return 0;
	}
	virtual int write(int64_t offset, const uint8_t* buf, uint32_t size) { 
		if (offset == -1) offset = m_size;
		if (offset + size > m_max_size || (buf == nullptr && size)) { return -1; }
		if (m_size < offset + size) {
			auto new_data = realloc((void*)m_data, offset + size);
			if (new_data == nullptr) {
				return -1;
			}
			m_data = (uint8_t*)new_data;
			m_size = (uint32_t)offset + size;
		}
		memcpy(m_data + offset, buf, size);
		return 0;
	}
	virtual int64_t size() { 
		return m_size; 
	}
	virtual const char* alias() { 
		return m_alias.empty() ? nullptr : m_alias.c_str(); 
	};
	virtual const char* fullpath() { 
		return m_fullpath.empty() ? nullptr : m_fullpath.c_str(); 
	};
	virtual int addref() { 
		return ++m_count; 
	};
	virtual int release() {
		if (m_count && --m_count == 0) { delete this; }
		return 0;
	};
private:
	volatile int m_count{ 1 };
	uint8_t* m_data{ nullptr };
	uint32_t m_size{ 0 };
	uint32_t m_max_size{ 0 };
	std::string m_alias;
	std::string m_fullpath;
};
