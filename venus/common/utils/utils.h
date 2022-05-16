#ifndef __COMMON_UTILS_H__
#define __COMMON_UTILS_H__

extern "C" {
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <string.h>
    #include <wchar.h>
};

#include <cctype>
#include <cmath>
#include <cstdarg>
#include <codecvt>
#include <locale>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>
#include <regex>

#if defined(_WIN32) || defined(_WIN64)
	#ifdef _MSC_VER
		#include <io.h>
		#define stat64 _stat64
		#define STDCALL __stdcall
    #else
        #define STDCALL // __attribute__((__stdcall__))
	#endif
    #include <stdarg.h>
#else
	#include <dirent.h>

	#define STDCALL // __attribute__((__stdcall__))
#endif

#define MESSAGE_DEBUG(p_ctx, format, ...) if((p_ctx) && (p_ctx->loger)) { (p_ctx)->loger->debug(format, ##__VA_ARGS__); }
#define MESSAGE_WARN(p_ctx, format, ...) if((p_ctx) && (p_ctx->loger)) { (p_ctx)->loger->warn(format, ##__VA_ARGS__); }
#define MESSAGE_FATAL(p_ctx, format, ...) if((p_ctx) && (p_ctx->loger)) { (p_ctx)->loger->fatal(format, ##__VA_ARGS__); }

class TimeHelper {
public:
    static struct tm stamp2tm(time_t stamp) {
        struct tm st_time;
        st_time = *localtime(&stamp);
        return st_time;
    }

    static std::string tm2str(const struct tm &st_time) {
        char strBuf[128] = {0};
        strftime(strBuf, 128, "%Y-%m-%d %H:%M:%S", &st_time);
        return std::string(strBuf);
    }

    static struct tm tran2tm(int year, int month, int day, int hour, int minute, int second) {
        struct tm st_time;
        st_time.tm_year = year - 1900;
        st_time.tm_mon = month > 0 ? month - 1 : 0;
        st_time.tm_mday = day;
        st_time.tm_hour = hour;
        st_time.tm_min = minute;
        st_time.tm_sec = second;
        return st_time;
    }
};

class FileUtils {
public:
    static int stat(const char *pcsPath, struct stat64 *stat) {
        if (nullptr == pcsPath || nullptr == stat) {
            return -1;
        }
        return stat64(pcsPath, stat);
    }

    static int enum_dir(const char *pcsPath, std::vector<std::string> &files) {
        if (pcsPath == nullptr) {
            return -1;
        }

        struct stat64 statBuf;
        if ((stat(pcsPath, &statBuf) == 0) && (statBuf.st_mode & S_IFDIR)) {
#if (_WIN32 || _WIN64)
            return enum_files_for_win(pcsPath, files);
#else
            return enum_files_for_linux(pcsPath, nullptr, files, 0, -1);
#endif
        } else {
            files.push_back(std::string(pcsPath));
        }
        return 0;
    }

    static int
    enum_files_for_linux(const char *pcRoot, const char *pcSubDir, std::vector<std::string> &relativesPaths, int depth,
                         int max_depth = -1) {
        int nRet = -1;
#if !(_WIN32 || _WIN64)
        std::string curDirPath, subDirPath;
        curDirPath += (pcRoot != nullptr ? std::string(pcRoot) : "");
        subDirPath += (pcSubDir != nullptr ? std::string(pcSubDir) : "");

        if (curDirPath.length() > 0 && curDirPath.length() - 1 != curDirPath.find_last_of("/\\")) {
            curDirPath += "/";
        }
        curDirPath += subDirPath;

        DIR *pDir = opendir(curDirPath.c_str());
        if (pDir == NULL) {
            return nRet;
        }

        struct dirent *ent = nullptr;
        while ((ent = readdir(pDir)) != NULL) {
            if (ent->d_type & DT_DIR) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                    continue;
                }
                if (max_depth > 0 && max_depth < depth) {
                    continue;
                }

                nRet = enum_files_for_linux(pcRoot, (subDirPath + "/" + std::string(ent->d_name)).c_str(),
                                            relativesPaths, depth + 1, max_depth);
                if (nRet != 0) {
                    //> TODO:
                }
                continue;
            }

            relativesPaths.push_back(subDirPath + "/" + std::string(ent->d_name));
        }
        closedir(pDir);
#endif
        return nRet;
    }

    static int enum_files_for_win(const char *pcDir, std::vector<std::string> &files) {
        int nRet = 0;
#if (_WIN32 || _WIN64)
        std::string dirPath(pcDir);
        struct _finddata_t stFindData;
        intptr_t findHandle = _findfirst(std::string(dirPath + "*.*").c_str(), &stFindData);
        if (findHandle < 0) {
            return -1;
        }

        do {
            if (strcmp(stFindData.name, ".") == 0 || 0 == strcmp(stFindData.name, "..")) {
                continue;
            }
            if (stFindData.attrib & _A_SUBDIR) {
                nRet = enum_files_for_win(std::string(std::string(pcDir) + stFindData.name + "\\").c_str(), files);
            }
            else {
                files.push_back(std::string(pcDir) + stFindData.name);
            }
        } while (0 == _findnext(findHandle, &stFindData));

        _findclose(findHandle);
#endif
        return nRet;
    }
};

class XsUtils {
public:
    static int message(int level, const char *format, ...) {
        int nret = 0;
        va_list valist;
        va_start(valist, format);
        nret = vprintf(format, valist);
        va_end(valist);
        return nret;
    }

    static bool is_host_little_endian() {
        uint8_t buf[] = {0x34, 0x12};
        return *(uint16_t *) buf == 0x1234;
    }

    static float entropy(const uint8_t *bytes, size_t size) {
        const int MAX_BUF = 256;
        int byteProp[MAX_BUF] = {0};
        for (auto cur = bytes; cur < bytes + size; cur++) {
            byteProp[*cur]++;
        }
        float entropy = 0.0;
        for (int i = 0; i < MAX_BUF; i++) {
            if (byteProp[i] == 0) continue;
            entropy -= (float) byteProp[i] / size * log2f((float) byteProp[i] / size);
        }
        return entropy;
    }

    static float chi2(const uint8_t *bytes, size_t size) {
        const int MAX_BUF = 256;
        int byteProp[MAX_BUF] = {0};
        for (auto cur = bytes; cur < bytes + size; cur++) {
            byteProp[*cur]++;
        }
        float chi2 = 0.0;
        float per_bin = (float) size / MAX_BUF;
        for (int i = 0; i < MAX_BUF; i++) {
            if (byteProp[i] == 0) continue;
            chi2 += (float) pow(byteProp[i] - per_bin, 2) / per_bin;
        }
        return chi2;
    }

    static std::string hex2str(const uint8_t *bytes, uint32_t size) {
        std::string thumbprint = "";
        char digestChars[3] = {0};
        for (uint32_t i = 0; i < size; i++) {
            auto x = *(bytes + i);
            digestChars[2] = 0;
            sprintf(digestChars, "%02x", x);
            thumbprint += digestChars;
        }
        return thumbprint;
    }

    static std::string int2str(int x) {
        std::stringstream ss;
        ss << x;
        return ss.str();
    }

    template<typename T>
    static std::string int2hex(T i, bool flag_0x = true) {
        std::stringstream stream;
        if (flag_0x) {
            stream << "0x";
        }
        stream << std::setw(sizeof(T) * 2) << std::setfill('0') << std::hex << i;
        return stream.str();
    }

    static std::u16string utf2u16s(std::string str) {
        return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(str);
    }

    static std::string u16s2utf(std::u16string str16) {
        return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(str16);
    }

    static std::u32string utf2u32s(std::string str) {
        return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(str);
    }

    static std::string u32s2utf(std::u32string str32) {
        return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.to_bytes(str32);
    }

    static std::wstring utf2uni(std::string str) {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>{}.from_bytes(str);
    }

    static std::string uni2utf(std::wstring wstr) {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>{}.to_bytes(wstr);
    }

    static std::u16string wstr2u16s(const std::wstring &wstr) {
        std::u16string u16str(wstr.begin(), wstr.end());
        return u16str;
    }

    static std::string unicode2ansi(const std::wstring &wstr) {
        std::string ret;
        std::mbstate_t state = {};
        const wchar_t *src = wstr.data();
        size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
        if (static_cast<size_t>(-1) != len) {
            std::unique_ptr<char[]> buff(new char[len + 1]);
            len = std::wcsrtombs(buff.get(), &src, len, &state);
            if (static_cast<size_t>(-1) != len) {
                ret.assign(buff.get(), len);
            }
        }
        return ret;
    }

    static std::wstring ansi2unicode(const std::string &str) {
        std::wstring ret;
        std::mbstate_t state = {};
        const char *src = str.data();
        size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
        if (static_cast<size_t>(-1) != len) {
            std::unique_ptr<wchar_t[]> buff(new wchar_t[len + 1]);
            len = std::mbsrtowcs(buff.get(), &src, len, &state);
            if (static_cast<size_t>(-1) != len) {
                ret.assign(buff.get(), len);
            }
        }
        return ret;
    }

    static std::string append_backslash(const char *p) {
        std::string str_temp = p ? p : "";
        if (str_temp.at(str_temp.size() - 1) != '\\' && str_temp.at(str_temp.size() - 1) != '/') {
#if (_WIN32 || _WIN64)
            str_temp += "\\";
#else
            str_temp += "/";
#endif
        }
        return str_temp;
    }

    static std::string split_filename(const char *pfilepath) {
        if (pfilepath == nullptr) {
            return "";
        }
        std::string filepath(pfilepath);
        std::string filename = "";
        const size_t last_slash_idx = filepath.find_last_of("\\/");
        if (std::string::npos != last_slash_idx) {
            filename = filepath.substr(last_slash_idx + 1, filepath.size() - last_slash_idx);
        }
        return filename;
    }

    template<typename T>
    static T byteorder2host(T a, bool raw_is_little_endian) {
        if (XsUtils::is_host_little_endian() == raw_is_little_endian) {
            return a;
        }
        size_t size = sizeof(T);
        if (size > 8) {
            throw "invalid type";
        }
        for (uint8_t i = 0, tmp = 0; i < size / 2; i++) {
            tmp = *((uint8_t *) &a + i);
            *((uint8_t *) &a + i) = *((uint8_t *) &a + size - i - 1);
            *((uint8_t *) &a + size - i - 1) = tmp;
        }
        return a;
    }

    template<typename T>
    static T host2byteorder(T a, bool out_little_endian) {
        if (XsUtils::is_host_little_endian() == out_little_endian) {
            return a;
        }
        size_t size = sizeof(T);
        if (size > 8) {
            throw "invalid type";
        }
        for (uint8_t i = 0, tmp = 0; i < size / 2; i++) {
            tmp = *((uint8_t *) &a + i);
            *((uint8_t *) &a + i) = *((uint8_t *) &a + size - i - 1);
            *((uint8_t *) &a + size - i - 1) = tmp;
        }
        return a;
    }

    static std::string random_str(int char_count = 16) {
        char character[] = {"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"};
        std::random_device rd;
        std::mt19937 gen32(rd());

        std::string ret;
        for (int i = 0; i < char_count; i++) {
            ret += character[gen32() % strlen(character)];
        }
        return ret;
    }

    static int clsid2str(const uint8_t* data, uint32_t size, bool little_endian, std::string& out) {
        if (data == nullptr || size != 16) {
            return -1;
        }
        uint32_t a = XsUtils::byteorder2host<int32_t>(*(uint32_t*)data, little_endian);
        uint16_t b = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + 4), little_endian);
        uint16_t c = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + 6), little_endian);
        uint16_t d = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + 8), little_endian);
        out = XsUtils::int2hex<uint32_t>(a, false);
        out += "-";
        out += XsUtils::int2hex<uint16_t>(b, false);
        out += "-";
        out += XsUtils::int2hex<uint16_t>(c, false);
        out += "-";
        out += XsUtils::int2hex<uint16_t>(d, false);
        out += "-";
        out += XsUtils::hex2str(data + 10, size - 10);
        return 0;
    }
};

class ucs {
public:
    static void trim(std::string& s, const char* cs = " \t\r\v\n") {
        if (s.empty()) return;
        s.erase(0, s.find_first_not_of(cs));
        s.erase(s.find_last_not_of(cs) + 1);
    }

    template<typename _StrType>
    static std::vector<_StrType>& split(const _StrType &s, const _StrType &delim, std::vector<_StrType>& out_vec) {
        auto out = std::back_inserter(out_vec);
        if (delim.empty()) {
            *out++ = s;
            return out_vec;
        }
        size_t a = 0, b = s.find(delim);
        for ( ; b != std::string::npos; a = b + delim.length(), b = s.find(delim, a))
        {
            *out++ = std::move(s.substr(a, b - a));
        }
        *out++ = std::move(s.substr(a, s.length() - a));
        return out_vec;
    }

    template<typename T>
    static size_t len(const T* a) {
        uint32_t csize = sizeof(T);
        if (csize == 1) { return strlen((const char *)a); }
#if defined(_WIN32) || defined(_WIN64)
        if (csize == 2) { return wcslen((const wchar_t *)a); }
        else if (csize == 4) { return std::char_traits<char32_t>::length((const char32_t*)a); }
#elif __linux__
        if (csize == 2) { return std::char_traits<char16_t>::length((const char16_t*)a); }
        else if (csize == 4) { return wcslen((const wchar_t *)a); }
#endif
        throw "unistrcmp : invalid type";
    }

	template<typename T>
	static int cmp(const T* a, const T* b) {
		uint32_t csize = sizeof(T);
		if (csize == 1) { return strcmp((const char *)a, (const char *)b); }
#if defined(_WIN32) || defined(_WIN64)
        if (csize == 2) { return wcscmp((const wchar_t *)a, (const wchar_t *)b); }
		else if (csize == 4) { std::u32string A= (const char32_t*)a, B= (const char32_t*)b; return A==B ? 0 : (A>B ? 1 : -1); }
#elif __linux__
        if (csize == 2) { std::u16string A = (const char16_t*)a, B = (const char16_t*)b; return A==B ? 0 : (A>B ? 1 : -1); }
		else if (csize == 4) { return wcscmp((const wchar_t *)a, (const wchar_t *)b); }
#endif
        throw "unistrcmp : invalid type";
	}

    template<typename T>
    static int icmp(const T* a, const T* b) {
        uint32_t csize = sizeof(T);

#if defined(_WIN32) || defined(_WIN64)
        if (csize == 1) { return stricmp((const char *)a, (const char *)b); }
        else if (csize == 2) { return wcsicmp((const wchar_t *)a, (const wchar_t *)b); }
#elif __linux__
        if (csize == 1) { return strcasecmp((const char *)a, (const char *)b); }
        else if (csize == 2) {
            std::u16string A = (const char16_t*)a, B = (const char16_t*)b;
            std::transform(A.begin(), A.end(), A.begin(), ::tolower);
            std::transform(B.begin(), B.end(), B.begin(), ::tolower);
            return A==B ? 0 : (A>B ? 1 : -1);;
        }
        else if (csize == 4) { return wcscasecmp((const wchar_t *)a, (const wchar_t *)b); }
#endif
        throw "unistrcmp : invalid type";
    }

    template<typename T>
    static int nicmp(const T* a, const T* b, size_t n) {
        uint32_t csize = sizeof(T);
#if defined(_WIN32) || defined(_WIN64)
        if (csize == 1) { return strnicmp((const char *)a, (const char *)b, n); }
        else if (csize == 2) { return wcsnicmp((const wchar_t *)a, (const wchar_t *)b, n); }
#elif __linux__
        if (csize == 1) { return strncasecmp((const char *)a, (const char *)b, n); }
        else if (csize == 2) {
            std::string A = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>().to_bytes((const char16_t*)a);
            std::string B = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>().to_bytes((const char16_t*)b);
            return strncasecmp(A.c_str(), B.c_str(), n);
        }
        else if (csize == 4) { return wcsncasecmp((const wchar_t *)a, (const wchar_t *)b, n); }
#endif
        throw "[] nicmp : invalid type";
    }
};

#endif //> __COMMON_UTILS_H__
