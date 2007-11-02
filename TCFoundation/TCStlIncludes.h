#ifndef __TCSTLINCLUDES_H__
#define __TCSTLINCLUDES_H__

#ifdef WIN32
// In Windows, we have to disable a number of warnings in order to use any STL
// classes without getting tons of warnings.  The following warning is shut off
// completely; it's just the warning that identifiers longer than 255 characters
// will be truncated in the debug info.  I really don't care about this.  Note
// that the other warnings are only disabled during the #include of the STL
// headers due to the warning(push) and warning(pop).
#pragma warning(push, 1)	// Minimum warnings during STL includes
// Disable more warnings.  Note that they're reenable at the pop below.
// 4702: Unreachable code.
// 4786: Identifier truncated to 255 characters in debug info.
#pragma warning(disable: 4702 4786)	// Unreachable code warnings pop up also.
#ifndef _DEBUG
#pragma warning(disable: 4710) // "Not inlined" warnings in release mode.
#endif // _DEBUG
#endif // WIN32
#include <string>
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include <stack>

#ifdef WIN32

#if defined(_MSC_VER) && _MSC_VER <= 1200

// MS VC++ 6.0 doesn't have the min/max macros.  The following include from
// Boost gets them defined.
#include <boost/config.hpp>

#endif

#pragma warning(pop)
#endif // WIN32

#ifdef NO_WSTRING
// NOTE: on system without wstring, the std namespace isn't used.
typedef basic_string<wchar_t> wstring;
#endif

#endif //__TCSTLINCLUDES_H__
