#ifndef __MYSTRING_H__
#define __MYSTRING_H__

#include <TCFoundation/TCDefines.h>
#include <TCFoundation/TCStlIncludes.h>

#include <string.h>
#ifdef _AIX
#include <strings.h>
#endif

#ifdef _QT
#include <Qt/qstring.h>
#endif // _QT

#ifdef _OSMESA
#include <stdarg.h>
#endif // _OSMESA

#ifdef TC_NO_UNICODE
#define ucsprintf sprintf
#define ucstrcmp strcmp
#define ucstrstr strstr
#define ucstrcpy strcpy
#define ucstrcat strcat
#define ucstrncpy strncpy
#define ucstrlen strlen
#define sucscanf sscanf
typedef std::string ucstring;
#else // TC_NO_UNICODE
#define ucstrcmp wcscmp
#define ucstrstr wcsstr
#define ucstrcpy wcscpy
#define ucstrcat wcscat
#define ucstrncpy wcsncpy
#define ucstrlen wcslen
#define sucscanf swscanf
typedef std::wstring ucstring;
#endif // TC_NO_UNICODE

TCExport char *copyString(const char *string, size_t pad = 0);
TCExport wchar_t *copyString(const wchar_t *string, size_t pad = 0);

#ifndef __APPLE__
TCExport char *strnstr(const char *s1, const char *s2, size_t n);
#ifndef __USE_GNU
TCExport char *strcasestr(const char *s1, const char *s2) __THROW;
#endif // !__USE_GNU
#endif // !__APPLE__
TCExport char *strnstr2(const char *s1, const char *s2, size_t n,
						int skipZero);
TCExport char *strncasestr(const char *s1, const char *s2, size_t n, int 
skipZero = 0);

//int stringHasPrefix(const char *s1, const char *s2);

TCExport void printStringArray(char** array, int count);
TCExport char **copyStringArray(char** array, int count);
template<class T> inline TCExport void deleteStringArray(T** array, int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		delete array[i];
	}
	delete array;
}

TCExport bool arrayContainsString(char** array, int count, const char* string);
TCExport bool arrayContainsPrefix(char** array, int count, const char* prefix);
TCExport char **componentsSeparatedByString(const char* string,
											const char* separator, int& count);
TCExport wchar_t **componentsSeparatedByString(const wchar_t* string,
											   const wchar_t* separator,
											   int& count);
TCExport char *componentsJoinedByString(char** array, int count,
	const char* separator);
TCExport bool stringHasPrefix(const char* string, const char* prefix);
TCExport bool stringHasCaseInsensitivePrefix(const char* string,
	const char* prefix);
TCExport bool stringHasCaseInsensitivePrefix(const wchar_t* string,
	const wchar_t* prefix);
TCExport bool stringHasSuffix(const char* string, const char* suffix);
TCExport bool stringHasSuffix(const wchar_t* string, const wchar_t* suffix);
TCExport bool stringHasCaseInsensitiveSuffix(const char* string,
	const char* suffix);
TCExport char* convertStringToUpper(char*);
TCExport char* convertStringToLower(char*);
std::string lowerCaseString(const std::string &src);
std::string upperCaseString(const std::string &src);

TCExport char* filenameFromPath(const char*);
TCExport ucstring filenameFromPath(const ucstring &path);
TCExport char* directoryFromPath(const char*);
TCExport ucstring directoryFromPath(const ucstring &path);
TCExport bool isRelativePath(const char*);
TCExport bool isDirectoryPath(const char* path);
TCExport bool isDirectoryPath(const std::string &path);
TCExport char* findRelativePath(const char* cwd, const char* path);
TCExport char* cleanedUpPath(const char* path);
TCExport void combinePath(const std::string &left, const std::string &right,
	std::string &combinedPath);
TCExport void combinePath(const char *left, const char *right,
	std::string &combinedPath);

TCExport char* findExecutable(const char* executable);

TCExport void stripCRLF(char*);
TCExport void stripCRLF(wchar_t*);
TCExport void stripTrailingWhitespace(char*);
TCExport void stripTrailingWhitespace(wchar_t*);
TCExport void stripLeadingWhitespace(char*);
TCExport void stripLeadingWhitespace(wchar_t*);
TCExport void stripTrailingPathSeparators(char*);
TCExport void replaceStringCharacter(char*, char, char, int = 1);
TCExport char *stringByReplacingSubstring(const char* string,
										  const char* oldSubstring,
										  const char* newSubstring,
										  bool repeat = true);
TCExport int countStringLines(const char*);

bool getCurrentDirectory(std::string &dir);
bool setCurrentDirectory(const std::string &dir);
bool createDirectory(const std::string &dir);
TCExport bool ensurePath(const std::string &path);

TCExport void setDebugLevel(int value);
TCExport int getDebugLevel(void);
TCExport void debugPrintf(const char *format, ...);
TCExport void debugPrintf(int level, const char *format, ...);
TCExport void indentPrintf(int indent, const char *format, ...);
TCExport void consolePrintf(const char *format, ...);
TCExport void consolePrintf(const wchar_t *format, ...);

TCExport void processEscapedString(char *string);
TCExport void processEscapedString(wchar_t *string);
TCExport char *createEscapedString(const char *string);

void stringtowstring(std::wstring &dst, const std::string &src);
void mbstowstring(std::wstring &dst, const char *src, int length = -1);
void wstringtostring(std::string &dst, const std::wstring &src);
void wcstostring(std::string &dst, const wchar_t *src, int length = -1);
#ifdef NO_WSTRING
unsigned long wcstoul(const wchar_t *start, wchar_t **end, int base);
#endif // NO_WSTRING
ucstring stringtoucstring(const std::string &src);
void stringtoucstring(ucstring &dst, const std::string &src);
UCSTR mbstoucstring(const char *src, int length = -1);
char *ucstringtombs(CUCSTR src, int length = -1);
char *ucstringtoutf8(CUCSTR src, int length = -1);
UCSTR utf8toucstring(const char *src, int length = -1);

#ifdef WIN32

void runningWithConsole(bool bRealConsole = false);

#else // WIN32

char *prettyLongLongString(long long);
long long longLongFromString(char*);

#endif // WIN32

typedef std::vector<ucstring> ucstringVector;

void mbstoucstring(ucstring &dst, const char *src, int length = -1);
int sucprintf(UCSTR buffer, size_t maxLen, CUCSTR format, ...);
int vsucprintf(UCSTR buffer, size_t maxLen, CUCSTR format, va_list argPtr);
std::string ftostr(double value, int precision = 6);
ucstring ftoucstr(double value, int precision = 6);
std::string ltostr(long value);
ucstring ltoucstr(long value);

#define COUNT_OF(ar) (sizeof(ar) / sizeof(ar[0]))

#endif
