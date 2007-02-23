#include "mystring.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#ifndef WIN32
#include <stdlib.h>
#include <wchar.h>
#endif

static int debugLevel = 0;

char *copyString(const char *string, int pad)
{
	if (string)
	{
		return strcpy(new char[strlen(string) + 1 + pad], string);
	}
	else
	{
		return NULL;
	}
}

wchar_t *copyString(const wchar_t *string, int pad)
{
	if (string)
	{
		return wcscpy(new wchar_t[wcslen(string) + 1 + pad], string);
	}
	else
	{
		return NULL;
	}
}

#ifndef __APPLE__

char *strnstr(const char *s1, const char *s2, size_t n)
{
	return strnstr2(s1, s2, n, 0);
}

char *strcasestr(const char *s1, const char *s2) __THROW
{
	char* spot;
	int len1 = strlen(s1);
	int len2 = strlen(s2);

	for (spot = (char*)s1; spot - s1 <= len1 - len2; spot++)
	{
		if (strncasecmp(spot, s2, len2) == 0)
		{
			return spot;
		}
	}
	return NULL;
}

#endif // !__APPLE__

char *strnstr2(const char *s1, const char *s2, size_t n, int skipZero)
{
	char* spot;
	int len2 = strlen(s2);

	for (spot = (char*)s1; (*spot != 0 || skipZero) &&
	     (unsigned)(spot-s1) < n; spot++)
	{
		if (strncmp(spot, s2, len2) == 0)
		{
			return spot;
		}
	}
	return NULL;
}

char *strncasestr(const char *s1, const char *s2, size_t n, int skipZero)
{
	char* spot;
	int len2 = strlen(s2);

	for (spot = (char*)s1; (*spot != 0 || skipZero) &&
	     (unsigned)(spot - s1) < n; spot++)
	{
		if (strncasecmp(spot, s2, len2) == 0)
		{
			return spot;
		}
	}
	return NULL;
}

//int stringHasPrefix(const char *s1, const char *s2)
//{
//	return strncmp(s1, s2, strlen(s2)) == 0;
//}

void printStringArray(char** array, int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		printf("<%s>\n", array[i]);
	}
}

char **copyStringArray(char** array, int count)
{
	if (array)
	{
		int i;
		char **newArray = new char*[count];

		for (i = 0; i < count; i++)
		{
			newArray[i] = copyString(array[i]);
		}
		return newArray;
	}
	else
	{
		return NULL;
	}
}

void deleteStringArray(char** array, int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		delete array[i];
	}
	delete array;
}

bool arrayContainsString(char** array, int count, const char* string)
{
	int i;

	for (i = 0; i < count; i++)
	{
		if (strcmp(array[i], string) == 0)
		{
			return true;
		}
	}
	return false;
}

bool arrayContainsPrefix(char** array, int count, const char* prefix)
{
	int i;
	int prefixLength = strlen(prefix);

	for (i = 0; i < count; i++)
	{
		if (strncmp(array[i], prefix, prefixLength) == 0)
		{
			return true;
		}
	}
	return false;
}

char** componentsSeparatedByString(const char* string, const char* separator,
								   int &count)
{
	int i;
	char* spot = (char*)string;
	char* tokenEnd = NULL;
	int separatorLength = strlen(separator);
	char** components;
	char* stringCopy;

	if (strlen(string) == 0)
	{
		count = 0;
		return NULL;
	}
	for (i = 0; (spot = strstr(spot, separator)) != NULL; i++)
	{
		spot += separatorLength;
	}
	count = i + 1;
	components = new char*[count];
	stringCopy = new char[strlen(string) + 1];
	strcpy(stringCopy, string);
	tokenEnd = strstr(stringCopy, separator);
	if (tokenEnd)
	{
		*tokenEnd = 0;
	}
	spot = stringCopy;
	for (i = 0; i < count; i++)
	{
		if (spot)
		{
			components[i] = new char[strlen(spot) + 1];
			strcpy(components[i], spot);
		}
		else
		{
			components[i] = new char[1];
			components[i][0] = 0;
		}
		if (tokenEnd)
		{
			spot = tokenEnd + separatorLength;
		}
		else
		{
			spot = NULL;
		}
		if (spot)
		{
			tokenEnd = strstr(spot, separator);
			if (tokenEnd)
			{
				*tokenEnd = 0;
			}
		}
		else
		{
			tokenEnd = NULL;
		}
	}
	delete stringCopy;
	return components;
}

char* componentsJoinedByString(char** array, int count, const char* separator)
{
	int length = 0;
	int i;
	int separatorLength = strlen(separator);
	char* string;

	for (i = 0; i < count; i++)
	{
		length += strlen(array[i]);
		if (i < count - 1)
		{
			length += separatorLength;
		}
	}
	string = new char[length + 1];
	string[0] = 0;
	for (i = 0; i < count - 1; i++)
	{
		strcat(string, array[i]);
		strcat(string, separator);
	}
	if (count)
	{
		strcat(string, array[count - 1]);
	}
	return string;
}

bool stringHasCaseInsensitivePrefix(const char* string, const char* prefix)
{
	int i;

	for (i = 0; string[i] && prefix[i] &&
		toupper(string[i]) == toupper(prefix[i]); i++)
	{
	}
	return prefix[i] == 0;
}

bool stringHasCaseInsensitivePrefix(const wchar_t* string,
	const wchar_t* prefix)
{
	int i;

	for (i = 0; string[i] && prefix[i] &&
		toupper(string[i]) == toupper(prefix[i]); i++)
	{
	}
	return prefix[i] == 0;
}

bool stringHasPrefix(const char* string, const char* prefix)
{
	int i;

	for (i = 0; string[i] && prefix[i] && string[i] == prefix[i]; i++)
	{
	}
	return prefix[i] == 0;
}

bool stringHasCaseInsensitiveSuffix(const char* string, const char* suffix)
{
	int i;
	int len1 = strlen(string);
	int len2 = strlen(suffix);

	for (i = 0; i < len1 && i < len2 &&
		toupper(string[len1 - i - 1]) == toupper(suffix[len2 - i - 1]); i++)
	{
	}
	return i == len2;
}

bool stringHasSuffix(const char* string, const char* suffix)
{
	int i;
	int len1 = strlen(string);
	int len2 = strlen(suffix);

	for (i = 0; i < len1 && i < len2 &&
		string[len1 - i - 1] == suffix[len2 - i - 1]; i++)
	{
	}
	return i == len2;
}

bool stringHasSuffix(const wchar_t* string, const wchar_t* suffix)
{
	int i;
	int len1 = wcslen(string);
	int len2 = wcslen(suffix);

	for (i = 0; i < len1 && i < len2 &&
		string[len1 - i - 1] == suffix[len2 - i - 1]; i++)
	{
	}
	return i == len2;
}

char* convertStringToUpper(char* string)
{
	int length = strlen(string);
	int i;

	for (i = 0; i < length; i++)
	{
		string[i] = (char)toupper(string[i]);
	}
	return string;
}

char* convertStringToLower(char* string)
{
	int length = strlen(string);
	int i;

	for (i = 0; i < length; i++)
	{
		string[i] = (char)tolower(string[i]);
	}
	return string;
}

char* findExecutable(const char* executable)
{
	char *path = getenv("PATH");
	char *retValue = NULL;
	int pathCount;
	char **pathComponents = componentsSeparatedByString(path, ":", pathCount);
	int i;

	for (i = 0; i < pathCount && retValue == NULL; i++)
	{
		FILE *file;

		retValue = copyString(pathComponents[i], 7);
		strcat(retValue, "/");
		strcat(retValue, executable);
		file = fopen(retValue, "r");
		if (file)
		{
			fclose(file);
		}
		else
		{
			delete retValue;
			retValue = NULL;
		}
	}
	deleteStringArray(pathComponents, pathCount);
	return retValue;
}

char* directoryFromPath(const char* path)
{
	if (path)
	{
		const char* slashSpot = strrchr(path, '/');
#ifdef WIN32
		const char* backslashSpot = strrchr(path, '\\');

		if (backslashSpot > slashSpot)
		{
			slashSpot = backslashSpot;
		}
#endif // WIN32
		if (slashSpot)
		{
			int length = slashSpot - path;
			char* directory = new char[length + 1];

			strncpy(directory, path, length);
			directory[length] = 0;
			return directory;
		}
		else
		{
			return copyString("");
		}
	}
	else
	{
		return NULL;
	}
}

char* filenameFromPath(const char* path)
{
	if (path)
	{
		const char* slashSpot = strrchr(path, '/');
#ifdef WIN32
		const char* backslashSpot = strrchr(path, '\\');

		if (backslashSpot > slashSpot)
		{
			slashSpot = backslashSpot;
		}
#endif // WIN32
		if (slashSpot)
		{
			slashSpot++;
			return copyString(slashSpot);
		}
		else
		{
			return copyString(path);
		}
	}
	else
	{
		return NULL;
	}
}

void stripCRLF(char* line)
{
	if (line)
	{
		int length = strlen(line);

		while (length > 0 && (line[length-1] == '\r' || line[length-1] == '\n'))
		{
			line[--length] = 0;
		}
	}
}

void stripCRLF(wchar_t* line)
{
	if (line)
	{
		int length = wcslen(line);

		while (length > 0 && (line[length-1] == '\r' || line[length-1] == '\n'))
		{
			line[--length] = 0;
		}
	}
}

void stripLeadingWhitespace(char* string)
{
	char *spot;

	for (spot = string; spot[0] == ' ' || spot[0] == '\t'; spot++)
		;
	if (spot[0])
	{
		if (spot != string)
		{
			memmove(string, spot, strlen(spot) + 1);
		}
	}
	else
	{
		string[0] = 0;
	}
}

void stripLeadingWhitespace(wchar_t* string)
{
	wchar_t *spot;

	for (spot = string; spot[0] == ' ' || spot[0] == '\t'; spot++)
		;
	if (spot[0])
	{
		if (spot != string)
		{
			memmove(string, spot, (wcslen(spot) + 1) * sizeof(wchar_t));
		}
	}
	else
	{
		string[0] = 0;
	}
}

void stripTrailingWhitespace(char* string)
{
	if (string)
	{
		int length = strlen(string);

		while (length > 0 && (string[length - 1] == ' ' ||
			string[length - 1] == '\t'))
		{
			string[--length] = 0;
		}
	}
}

void stripTrailingWhitespace(wchar_t* string)
{
	if (string)
	{
		int length = wcslen(string);

		while (length > 0 && (string[length - 1] == ' ' ||
			string[length - 1] == '\t'))
		{
			string[--length] = 0;
		}
	}
}

void stripTrailingPathSeparators(char* path)
{
	if (path)
	{
		int length = strlen(path);

#ifdef WIN32
		while (length > 0 && (path[length-1] == '\\' || path[length-1] == '/'))
#else
		while (length > 0 && path[length-1] == '/')
#endif
		{
			path[--length] = 0;
		}
	}
}

void replaceStringCharacter(char* string, char oldChar, char newChar, int 
repeat)
{
	char* spot;

	if ((spot = strchr(string, oldChar)) != NULL)
	{
		*spot = newChar;
		if (repeat)
		{
			while ((spot = strchr(spot, oldChar)) != NULL)
			{
				*spot = newChar;
			}
		}
	}
}

char *stringByReplacingSubstring(const char* string, const char* oldSubstring,
								 const char* newSubstring, bool repeat)
{
	char *newString = NULL;

	if (repeat)
	{
		int count;
		char **components = componentsSeparatedByString(string, oldSubstring,
			count);

		newString = componentsJoinedByString(components, count,
			newSubstring);
		deleteStringArray(components, count);
	}
	else
	{
		const char *oldSpot = strstr(string, oldSubstring);

		if (oldSpot)
		{
			int oldSubLength = strlen(oldSubstring);
			int newSubLength = strlen(newSubstring);
			int preLength = oldSpot - string;

			newString = new char[strlen(string) + newSubLength - oldSubLength +
				1];
			strncpy(newString, string, preLength);
			strcpy(newString + preLength, newSubstring);
			strcpy(newString + preLength + newSubLength,
				string + preLength + oldSubLength);
		}
		else
		{
			newString = new char[strlen(string) + 1];
			strcpy(newString, string);
		}
	}
	return newString;
}

int countStringLines(const char* string)
{
	int len = strlen(string);
	int count = 0;
	char *spot = (char*)string;

	if (string[len - 1] != '\n')
	{
		count++;
	}
	while ((spot = strchr(spot, '\n')) != NULL)
	{
		count++;
		spot++;
	}
	return count;
}

void setDebugLevel(int value)
{
	debugLevel = value;
}

int getDebugLevel(void)
{
	return debugLevel;
}

void indentPrintf(int indent, const char *format, ...)
{
	int i;
	va_list argPtr;

	for (i = 0; i < indent; i++)
	{
		printf(" ");
	}
	va_start(argPtr, format);
	vprintf(format, argPtr);
	va_end(argPtr);
}

void debugVPrintf(int level, const char *format, va_list argPtr)
{
	if (debugLevel >= level)
	{
		vprintf(format, argPtr);
	}
}

void debugPrintf(const char *format, ...)
{
	va_list argPtr;

	va_start(argPtr, format);
	debugVPrintf(1, format, argPtr);
	va_end(argPtr);
}

void debugPrintf(int level, const char *format, ...)
{
	va_list argPtr;

	va_start(argPtr, format);
	debugVPrintf(level, format, argPtr);
	va_end(argPtr);
}

#ifndef WIN32

char *prettyLongLongString(long long num)
{
	char backwards[256];
	char *forwards;
	int i, j;

	for (i = 0; num; i++)
	{
		if (!((i + 1) % 4))
		{
			backwards[i++] = ',';
		}
		backwards[i] = (num % 10) + '0';
		num /= 10;
	}
	if (i == 0)
	{
		backwards[0] = '0';
		i = 1;
	}
	backwards[i] = 0;
	forwards = new char[(i + 1) * sizeof(char)];
	for (j = 0, i--; i >= 0; i--, j++)
	{
		forwards[j] = backwards[i];
	}
	forwards[j] = 0;
	return forwards;
}

long long longLongFromString(char* string)
{
	long long val;
	char* tmpString = NULL;
	int length = strlen(string);
	char* spot;

	while ((spot = strchr(string, ',')) != NULL)
	{
		int diff = spot - string;
		int needsDelete = 0;

		if (tmpString)
		{
			needsDelete = 1;
		}
		tmpString = new char[length];
		memcpy(tmpString, string, diff);
		tmpString[diff] = 0;
		strcat(tmpString, string + diff + 1);
		if (needsDelete)
		{
			delete string;
		}
		string = tmpString;
	}
	sscanf(string, "%lld", &val);
	if (tmpString)
	{
		delete tmpString;
	}
	return val;
}

#endif // WIN32

static int escapeReplacement(char ch)
{
	switch (ch)
	{
	case 'a':
		return '\a';
		break;
	case 'b':
		return '\b';
		break;
	case 'f':
		return '\f';
		break;
	case 'n':
		return '\n';
		break;
	case 'r':
		return '\r';
		break;
	case 't':
		return '\t';
		break;
	case 'v':
		return '\v';
		break;
	case '?':
		return '\?';
		break;
	case '\'':
		return '\'';
		break;
	case '"':
		return '"';
		break;
	case '\\':
		return '\\';
		break;
	case '0':
		return '\0';
		break;
	}
	return -1;
}

static int escapeReplacement(wchar_t wch)
{
	return escapeReplacement((char)wch);
}

void processEscapedString(char *string)
{
	int i;
	int len = strlen(string);
	int tmpLen = 0;
	char *tmpString = new char[len + 1];
	int lastSpot = 0;

	// Note we skip the last character, because even if it's a backslash, we
	// can't do anything with it.
	for (i = 0; i < len - 1; i++)
	{
		if (string[i] == '\\')
		{
			int replacement = escapeReplacement(string[i + 1]);

			if (replacement != -1)
			{
				if (i > lastSpot)
				{
					int count = i - lastSpot;

					strncpy(&tmpString[tmpLen], &string[lastSpot], count);
					tmpLen += count;
					lastSpot += count;
				}
				lastSpot += 2;
				tmpString[tmpLen++] = (char)replacement;
				i++;
			}
		}
	}
	strcpy(&tmpString[tmpLen], &string[lastSpot]);
	strcpy(string, tmpString);
	delete tmpString;
}

void processEscapedString(wchar_t *string)
{
	int i;
	int len = wcslen(string);
	int tmpLen = 0;
	wchar_t *tmpString = new wchar_t[len + 1];
	int lastSpot = 0;

	// Note we skip the last character, because even if it's a backslash, we
	// can't do anything with it.
	for (i = 0; i < len - 1; i++)
	{
		if (string[i] == '\\')
		{
			int replacement = escapeReplacement(string[i + 1]);

			if (replacement != -1)
			{
				if (i > lastSpot)
				{
					int count = i - lastSpot;

					wcsncpy(&tmpString[tmpLen], &string[lastSpot], count);
					tmpLen += count;
					lastSpot += count;
				}
				lastSpot += 2;
				tmpString[tmpLen++] = (wchar_t)replacement;
				i++;
			}
		}
	}
	wcscpy(&tmpString[tmpLen], &string[lastSpot]);
	wcscpy(string, tmpString);
	delete tmpString;
}

void mbstowstring(std::wstring &dst, const char *src, int length /*= -1*/)
{
	dst.erase();
	if (src)
	{
		mbstate_t state = { 0 };
		size_t newLength;

		if (length == -1)
		{
			length = strlen(src);
		}
		dst.resize(length);
		// Even though we don't check, we can't pass NULL instead of &state and
		// still be thread-safe.
		newLength = mbsrtowcs(&dst[0], &src, length + 1, &state);
		if (newLength == (size_t)-1)
		{
			dst.resize(wcslen(&dst[0]));
		}
		else
		{
			dst.resize(newLength);
		}
	}
}

void wcstostring(std::string &dst, const wchar_t *src, int length /*= -1*/)
{
	dst.erase();
	if (src)
	{
		mbstate_t state = { 0 };
		size_t newLength;

		if (length == -1)
		{
			length = wcslen(src);
		}
		length *= 2;
		dst.resize(length);
		// Even though we don't check, we can't pass NULL instead of &state and
		// still be thread-safe.
		newLength = wcsrtombs(&dst[0], &src, length, &state);
		if (newLength == (size_t)-1)
		{
			dst.resize(strlen(&dst[0]));
		}
		else
		{
			dst.resize(newLength);
		}
	}
}

void wstringtostring(std::string &dst, const std::wstring &src)
{
	wcstostring(dst, src.c_str(), src.length());
}

void stringtowstring(std::wstring &dst, const std::string &src)
{
	mbstowstring(dst, src.c_str(), src.length());
}

#ifdef _QT
void wcstoqstring(QString &dst, const wchar_t *src, int length /*= -1*/)
{
	int i;
	QChar *temp;

	if (length == -1)
	{
		length = wcslen(src);
	}
	temp = new QChar[length];
	for (i = 0; i < length; i++)
	{
		temp[i] = (QChar)src[i];
	}
	dst.truncate(0);
	dst.insert(0, temp, length);
	delete temp;
}

void wstringtoqstring(QString &dst, const std::wstring &src)
{
	wcstoqstring(dst, src.c_str(), src.length());
}

#endif // _QT
