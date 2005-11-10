#ifndef __TCDEFINES_H__
#define __TCDEFINES_H__

#ifndef NO
#define NO 0
#endif
#ifndef YES
#define YES (!NO)
#endif

#ifdef WIN32
// The following shouldn't be necessary here, but due to bugs in Microsoft's
// precompiled headers, it is.  The warning being disabled below is the one
// that warns about identifiers longer than 255 characters being truncated to
// 255 characters in the debug info.
#pragma warning(disable : 4786 4702)

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE

#include <winsock2.h>
#include <crtdbg.h>

#pragma warning( disable : 4514 4127 )

#ifdef _BUILDING_TCFOUNDATION
#define TCExport __declspec(dllexport)
#elif defined _BUILDING_TCFOUNDATION_LIB || defined _TC_STATIC
#define TCExport
#else
#define TCExport __declspec(dllimport)
#endif

#else // WIN32

#define TCExport

#endif // WIN32

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

typedef unsigned char TCByte;
typedef unsigned char TCUChar;
typedef unsigned long TCULong;
typedef unsigned short TCUShort;

#ifndef __THROW
#define __THROW
#endif //__THROW

#endif // __TCDEFINES_H__
