#ifndef __TCDEFINES_H__
#define __TCDEFINES_H__

#ifndef NO
#define NO 0
#endif
#ifndef YES
#define YES (!NO)
#endif

#ifdef WIN32

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

typedef unsigned char TCByte;
typedef unsigned char TCUChar;
typedef unsigned long TCULong;
typedef unsigned short TCUShort;

#endif // __TCDEFINES_H__
