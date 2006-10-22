#ifndef __MYSTRING_H__
#define __MYSTRING_H__

#include <TCFoundation/TCDefines.h>

#include <string.h>
#ifdef _AIX
#include <strings.h>
#endif

TCExport char *copyString(const char *string, int pad = 0);

#ifndef __APPLE__
TCExport char *strnstr(const char *s1, const char *s2, size_t n);
TCExport char *strcasestr(const char *s1, const char *s2) __THROW;
#endif // !__APPLE__
TCExport char *strnstr2(const char *s1, const char *s2, size_t n,
						int skipZero);
TCExport char *strncasestr(const char *s1, const char *s2, size_t n, int 
skipZero = 0);

//int stringHasPrefix(const char *s1, const char *s2);

TCExport void printStringArray(char** array, int count);
TCExport char **copyStringArray(char** array, int count);
TCExport void deleteStringArray(char** array, int count);
TCExport bool arrayContainsString(char** array, int count, const char* string);
TCExport bool arrayContainsPrefix(char** array, int count, const char* prefix);
TCExport char **componentsSeparatedByString(const char* string,
											const char* separator, int& count);
TCExport char *componentsJoinedByString(char** array, int count, const char* 
separator);
TCExport bool stringHasPrefix(const char* string, const char* prefix);
TCExport bool stringHasCaseInsensitivePrefix(const char* string, const char* 
prefix);
TCExport bool stringHasSuffix(const char* string, const char* suffix);
TCExport bool stringHasCaseInsensitiveSuffix(const char* string, const char* 
suffix);
TCExport char* convertStringToUpper(char*);
TCExport char* convertStringToLower(char*);

TCExport char* filenameFromPath(const char*);
TCExport char* directoryFromPath(const char*);

TCExport void stripCRLF(char*);
TCExport void stripTrailingWhitespace(char*);
TCExport void stripLeadingWhitespace(char*);
TCExport void stripTrailingPathSeparators(char*);
TCExport void replaceStringCharacter(char*, char, char, int = 1);
TCExport char *stringByReplacingSubstring(const char* string,
										  const char* oldSubstring,
										  const char* newSubstring,
										  bool repeat = true);
TCExport int countStringLines(const char*);

TCExport void setDebugLevel(int value);
TCExport int getDebugLevel(void);
TCExport void debugPrintf(const char *format, ...);
TCExport void debugPrintf(int level, const char *format, ...);
TCExport void indentPrintf(int indent, const char *format, ...);

TCExport void processEscapedString(char *string);

#ifdef WIN32

#define strcasecmp stricmp
#define strncasecmp strnicmp

#else // WIN32

char *prettyLongLongString(long long);
long long longLongFromString(char*);

#endif // WIN32

#endif
