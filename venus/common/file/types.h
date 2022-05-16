#ifndef __TYPES_H__
#define __TYPES_H__

#if defined(_CSTDINT_) || defined(_STDINT_H)
    

#else
    typedef signed char          int8_t;
    typedef unsigned char        uint8_t;
    typedef short int            int16_t;
    typedef unsigned short int   uint16_t;

	#ifdef _MSC_VER
		typedef long                 int32_t;
		typedef unsigned long        uint32_t;
	#else
		typedef int                  int32_t;
		typedef unsigned int         uint32_t;
	#endif  // _MSC_VER

	#ifdef _MSC_VER
		/**
		* @attention   VS2005��֧��long long, ��֮ǰ�İ汾��֧��
		**/
		typedef __int64              int64_t;
		typedef unsigned __int64     uint64_t;
	#else
		typedef long long            int64_t;
		typedef unsigned long long   uint64_t;
	#endif

	// _WIN64��VCԤ�����, __LP64__��GCCԤ�����
	#if defined(_WIN64) || defined(__LP64__)
	#else
	#endif
#endif

#endif // __TYPES_H__