#ifndef __TCDEFINES_H__
#define __TCDEFINES_H__

#ifdef COCOA
// The following is necessary to get rid of some truly screwed up warnings that
// show up when compiling on the Mac.
#pragma GCC visibility push(default)
#endif // COCOA

#ifdef WIN32
// The following shouldn't be necessary here, but due to bugs in Microsoft's
// precompiled headers, it is.  The warning being disabled below is the one
// that warns about identifiers longer than 255 characters being truncated to
// 255 characters in the debug info.  (Also, "not inlined", and one other.)
#pragma warning(disable : 4786 4702 4710)

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE

#ifdef _WIN32_WINDOWS
#undef _WIN32_WINDOWS
#endif

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINDOWS 0x0600
#define _WIN32_WINNT 0x0600

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
   #define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif // _DEBUG

#include <winsock2.h>
#include <crtdbg.h>

#undef min
#undef max

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

#if defined (__APPLE__) || defined (_OSMESA)
#include <wchar.h>
#endif // __APPLE__ || _OSMESA

// NOTE: the following should be 1, 2, and 4 bytes each.  So on a 64-bit system,
// the following defines aren't appropriate, and something else needs to be
// substituted.

typedef unsigned char TCByte;
typedef unsigned char TCUChar;
typedef unsigned short TCUShort;
typedef unsigned int TCUInt;
#ifdef __U32_TYPE
typedef __U32_TYPE TCULong;
#else // __U32_TYPE
// I know the following seems wrong, but int should always be 32-bit.
typedef unsigned int TCULong;
#endif // __U32_TYPE
typedef char TCChar;
typedef short TCShort;
typedef int TCInt;
// I know the following seems wrong, but int should always be 32-bit.
typedef int TCLong;

// Define LDVIEW_DOUBLES to have LDView use doubles instead of floats.  Comment
// out the definition for floats.
//#define LDVIEW_DOUBLES

// I'm not sure if floats are 64 bits on a 64-bit system or not.  I know that
// TCFloat has to be 32 bits when LDVIEW_DOUBLES isn't defined in order for it
// to work.
#ifdef LDVIEW_DOUBLES
typedef double TCFloat;
#else // LDVIEW_DOUBLES
typedef float TCFloat;
#endif // LDVIEW_DOUBLES

// The following must always be defined to 32 bits.
typedef float TCFloat32;

#ifndef __THROW
#define __THROW
#endif //__THROW

//#define TC_NO_UNICODE

#ifdef TC_NO_UNICODE
typedef char UCCHAR;
typedef char * UCSTR;
typedef const char * CUCSTR;
#define _UC(x) x
#else // TC_NO_UNICODE
typedef wchar_t UCCHAR;
typedef wchar_t * UCSTR;
typedef const wchar_t * CUCSTR;
#define _UC(x) L ## x
#endif // TC_NO_UNICODE

#endif // __TCDEFINES_H__
