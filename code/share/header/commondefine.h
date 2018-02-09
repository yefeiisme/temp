#ifndef __COMMON_DEFINE_H_
#define __COMMON_DEFINE_H_

#include <stdint.h>

typedef unsigned long		DWORD;  //%lu
typedef unsigned char		BYTE;	//%hhu
typedef unsigned short		WORD;	//%hu
typedef unsigned int		UINT;	//%u
typedef void*				LPVOID;
typedef int					BATTLETYPE; //%d
#ifdef WIN32
typedef unsigned __int64	uint64;		//%llu
typedef __int64				int64;		//%lld
#else
typedef unsigned long long	uint64;
typedef long long			int64;
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(Address)		{ if( nullptr != (Address) )	delete(Address);	Address = nullptr; }
#endif
#ifndef SAFE_DELETE_ARR
#define SAFE_DELETE_ARR(Address)	{ if( nullptr != (Address) )	delete[](Address);	Address = nullptr; }
#endif

#ifndef STRING_INSURE
#define STRING_INSURE(dest) {dest[sizeof(dest) - 1] = '\0';}
#endif

#ifndef STRING_COPY
#define STRING_COPY(dest, src) {strncpy(dest, src, sizeof(dest)); dest[sizeof(dest) - 1] = '\0';}
#endif

#ifndef STRING_COPY_SIZE
#define STRING_COPY_SIZE(dest, src, size) {strncpy(dest, src, size); dest[size - 1] = '\0';}
#endif

#endif
