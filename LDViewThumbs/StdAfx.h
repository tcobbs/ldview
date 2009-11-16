// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#pragma once

#if _MSC_VER < 1400	// VC < VC 2005
#pragma warning(disable: 4503) // Decorated name truncated
#endif // VC < VC 2005

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <winsock2.h>

#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

#include <shobjidl.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
