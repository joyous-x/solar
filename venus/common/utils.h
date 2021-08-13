#ifndef __COMMON_UTILS_H__
#define __COMMON_UTILS_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h> 
#include <math.h>
#include <codecvt>
#include <locale>
#include <iomanip>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#ifdef _MSC_VER
#include <io.h>
	#define stat64(x,y) _stat64(x,y)
	#define stat64 _stat64
#else
#if (_WIN32 || _WIN64)
#else
#include <sys/types.h>
#include <dirent.h>
#endif
#endif


class TimeHelper {
public:
	static struct tm stamp2tm(time_t stamp) {
		struct tm st_time;
		st_time = *localtime(&stamp);
		return st_time;
	}

	static std::string tm2str(const struct tm& st_time) {
		char strBuf[128] = { 0 };
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
	static int stat(const char* pcsPath, struct stat64* stat) {
		if (nullptr == pcsPath || nullptr == stat) {
			return -1;
		}
		return stat64(pcsPath, stat);
	}

	static int enum_dir(const char* pcsPath, std::vector<std::string>& files) {
#if (_WIN32 || _WIN64)
		return enum_files_for_win(pcsPath, files);
#else
		return enum_files_for_linux(pcsPath, nullptr, files, 0, -1);
#endif
	}

	static int enum_files_for_linux(const char* pcRoot, const char* pcSubDir, std::vector<std::string>& relativesPaths, int depth, int max_depth = -1)
	{
		int nRet = -1;
#if !(_WIN32 || _WIN64)
		std::string curDirPath, subDirPath;
		curDirPath += (pcRoot != nullptr ? std::string(pcRoot) : "");
		subDirPath += (pcSubDir != nullptr ? std::string(pcSubDir) : "");

		if (curDirPath.length() > 0 && curDirPath.length() - 1 != curDirPath.find_last_of("/\\")) {
			curDirPath += "/";
		}
		curDirPath += subDirPath;

		DIR* pDir = opendir(curDirPath.c_str());
		if (pDir == NULL) {
			return nRet;
		}

		struct dirent* ent = nullptr;
		while ((ent = readdir(pDir)) != NULL) {
			if (ent->d_type & DT_DIR) {
				if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
					continue;
				}
				if (max_depth > 0 && max_depth < depth) {
					continue;
				}

				nRet = enum_files_for_linux(pcRoot, (subDirPath + "/" + std::string(ent->d_name)).c_str(), relativesPaths, depth + 1, max_depth);
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

	static int enum_files_for_win(const char* pcDir, std::vector<std::string>& files)
	{
#if (_WIN32 || _WIN64)
		std::string dirPath(pcDir);
		struct _finddata_t stFindData;
		intptr_t findHandle = _findfirst(std::string(dirPath + "*.*").c_str(), &stFindData);
		if (findHandle < 0)
		{
			std::cout << "encum files " << std::string(pcDir) << " error: " << findHandle << std::endl;
			return -1;
		}

		do
		{
			if (strcmp(stFindData.name, ".") == 0 || 0 == strcmp(stFindData.name, ".."))
			{
				continue;
			}
			if (stFindData.attrib & _A_SUBDIR)
			{
				enum_files(std::string(std::string(pcDir) + stFindData.name + "\\").c_str(), files);
			}
			else
			{
				files.push_back(std::string(pcDir) + stFindData.name);
			}
		} while (0 == _findnext(findHandle, &stFindData));

		_findclose(findHandle);
#endif
		return 0;
	} 
};

class XsUtils {
public:
	static void message(const std::string& msg, int level = 0) {
		std::cout << std::string(msg) << std::endl;
	}

	static float entropy(const uint8_t* bytes, size_t size) {
		const int MAX_BUF = 256;
		int byteProp[MAX_BUF] = { 0 };
		for (auto cur = bytes; cur < bytes + size; cur++) {
			byteProp[*cur] ++;
		}
		float entropy = 0.0;
		for (int i = 0; i < MAX_BUF; i++) {
			if (byteProp[i] == 0) continue;
			entropy -= (float)byteProp[i] / size * log2f((float)byteProp[i] / size);
		}
		return entropy;
	}

	static float chi2(const uint8_t* bytes, size_t size) {
		const int MAX_BUF = 256;
		int byteProp[MAX_BUF] = { 0 };
		for (auto cur = bytes; cur < bytes + size; cur++) {
			byteProp[*cur] ++;
		}
		float chi2 = 0.0;
		float per_bin = (float)size / MAX_BUF;
		for (int i = 0; i < MAX_BUF; i++) {
			if (byteProp[i] == 0) continue;
			chi2 += pow(byteProp[i] - per_bin, 2) / per_bin;
		}
		return chi2;
	}

	static std::string vechex2str(const std::vector<uint8_t>& bytes) {
		std::string thumbprint = "";
		for (uint8_t x : bytes) {
			char digestChars[3];
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

	template< typename T >
	static std::string int2hex(T i)
	{
		std::stringstream stream;
		stream << "0x" << std::setfill('0') << std::hex << i;
		return stream.str();
	}

	static std::u16string str2u16s(std::string str) {
		return std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t >{}.from_bytes(str);
	}

	static std::string u16s2str(std::u16string str16) {
		return std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t >{}.to_bytes(str16);
	}

	static std::u32string str2u32s(std::string str) {
		return std::wstring_convert< std::codecvt_utf8<char32_t>, char32_t >{}.from_bytes(str);
	}

	static std::string u32s2str(std::u32string str32) {
		return std::wstring_convert< std::codecvt_utf8<char32_t>, char32_t >{}.to_bytes(str32);
	}

	static std::wstring str2wstr(std::string str) {
		return std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t >{}.from_bytes(str);
	}

	static std::string wstr2str(std::wstring wstr) {
		return std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t >{}.to_bytes(wstr);
	}

	static std::u16string wstr2u16s(const std::wstring& wstr) {
		std::u16string u16str(wstr.begin(), wstr.end());
		return u16str;
	}

	static std::string unicode2ansi(const std::wstring& wstr) {
		std::string ret;
		std::mbstate_t state = {};
		const wchar_t* src = wstr.data();
		size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
		if (static_cast<size_t>(-1) != len) {
			std::unique_ptr< char[] > buff(new char[len + 1]);
			len = std::wcsrtombs(buff.get(), &src, len, &state);
			if (static_cast<size_t>(-1) != len) {
				ret.assign(buff.get(), len);
			}
		}
		return ret;
	}
	static std::wstring ansi2unicode(const std::string& str) {
		std::wstring ret;
		std::mbstate_t state = {};
		const char* src = str.data();
		size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
		if (static_cast<size_t>(-1) != len) {
			std::unique_ptr< wchar_t[] > buff(new wchar_t[len + 1]);
			len = std::mbsrtowcs(buff.get(), &src, len, &state);
			if (static_cast<size_t>(-1) != len) {
				ret.assign(buff.get(), len);
			}
		}
		return ret;
	}

};

#endif //> __COMMON_UTILS_H__