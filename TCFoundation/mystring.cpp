#include "mystring.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

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

char *strnstr(const char *s1, const char *s2, size_t n, int skipZero)
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

char *strcasestr(const char *s1, const char *s2)
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

int arrayContainsString(char** array, int count, const char* string)
{
	int i;

	for (i = 0; i < count; i++)
	{
		if (strcmp(array[i], string) == 0)
		{
			return 1;
		}
	}
	return 0;
}

int arrayContainsPrefix(char** array, int count, const char* prefix)
{
	int i;
	int prefixLength = strlen(prefix);

	for (i = 0; i < count; i++)
	{
		if (strncmp(array[i], prefix, prefixLength) == 0)
		{
			return 1;
		}
	}
	return 0;
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

int stringHasCaseInsensitivePrefix(const char* string, const char* prefix)
{
	int i;

	for (i = 0; string[i] && prefix[i] &&
		toupper(string[i]) == toupper(prefix[i]); i++)
	{
	}
	return prefix[i] == 0;
}

int stringHasPrefix(const char* string, const char* prefix)
{
	int i;

	for (i = 0; string[i] && prefix[i] && string[i] == prefix[i]; i++)
	{
	}
	return prefix[i] == 0;
}

int stringHasCaseInsensitiveSuffix(const char* string, const char* suffix)
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

int stringHasSuffix(const char* string, const char* suffix)
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

void convertStringToUpper(char* string)
{
	int length = strlen(string);
	int i;

	for (i = 0; i < length; i++)
	{
		string[i] = (char)toupper(string[i]);
	}
}

void convertStringToLower(char* string)
{
	int length = strlen(string);
	int i;

	for (i = 0; i < length; i++)
	{
		string[i] = (char)tolower(string[i]);
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
		char *oldSpot = strstr(string, oldSubstring);

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
