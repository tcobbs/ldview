#ifndef __MYSTRING_H__
#define __MYSTRING_H__

#include <TCFoundation/TCDefines.h>

#include <string.h>

#define copyString(a) ((a) ? strcpy(new char[strlen(a)+1], (a)) : NULL)

TCExport char *strnstr(const char *s1, const char *s2, size_t n,
					   int skipZero = 0);
TCExport char *strncasestr(const char *s1, const char *s2, size_t n, int 
skipZero = 0);
TCExport char *strcasestr(const char *s1, const char *s2);

//int stringHasPrefix(const char *s1, const char *s2);

TCExport void printStringArray(char** array, int count);
TCExport char **copyStringArray(char** array, int count);
TCExport void deleteStringArray(char** array, int count);
TCExport int arrayContainsString(char** array, int count, const char* string);
TCExport int arrayContainsPrefix(char** array, int count, const char* prefix);
TCExport char **componentsSeparatedByString(const char* string, const char* 
separator,
											  int& count);
TCExport char *componentsJoinedByString(char** array, int count, const char* 
separator);
TCExport int stringHasPrefix(const char* string, const char* prefix);
TCExport int stringHasCaseInsensitivePrefix(const char* string, const char* 
prefix);
TCExport int stringHasSuffix(const char* string, const char* suffix);
TCExport int stringHasCaseInsensitiveSuffix(const char* string, const char* 
suffix);
TCExport void convertStringToUpper(char*);
TCExport void convertStringToLower(char*);
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

#ifdef WIN32

#define strcasecmp stricmp
#define strncasecmp strnicmp

#else // WIN32

char *prettyLongLongString(long long);
long long longLongFromString(char*);

#endif // WIN32

#endif
