#include "TCStringArray.h"
#include "mystring.h"

#include <string.h>
#include <stdio.h>

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

TCStringArray::TCStringArray(size_t allocated, bool caseSensitive)
			  :TCArray<>(allocated),
			   caseSensitive(caseSensitive)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TCStringArray");
#endif
}

TCStringArray::TCStringArray(const TCCharStarStar items, size_t numItems,
							 bool caseSensitive)
			  :TCArray<>(numItems),
			   caseSensitive(caseSensitive)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TCStringArray");
#endif
	size_t i;

	for (i = 0; i < numItems; i++)
	{
		addString(items[i]);
	}
}

TCStringArray::~TCStringArray(void)
{
}

void TCStringArray::dealloc(void)
{
	for (size_t i = 0; i < count; i++)
	{
		delete[] (char*)(items[i]);
	}
	TCArray<>::dealloc();
}

size_t TCStringArray::addString(const char* newString)
{
	addItem(copyString(newString));
	return count - 1;
}

void TCStringArray::insertString(const char* newString, size_t index)
{
	insertItem(copyString(newString), index);
}

bool TCStringArray::replaceString(const char* newString, size_t index)
{
	if (index < count)
	{
		delete[] (char*)(items[index]);
		items[index] = (void*)copyString(newString);
		return true;
	}
	else
	{
		return false;
	}
}

bool TCStringArray::appendString(const char* newSuffix, size_t index)
{
	if (index < count)
	{
		char *currentString = stringAtIndex(index);
		char *newString = new char[strlen(currentString) + strlen(newSuffix) +
			1];

		strcpy(newString, currentString);
		strcat(newString, newSuffix);
		delete[] currentString;
		items[index] = newString;
		return true;
	}
	else
	{
		return false;
	}
}

ptrdiff_t TCStringArray::indexOfString(const char* string)
{
	for (size_t i = 0; i < count; i++)
	{
		if (string && items[i])
		{
			if ((caseSensitive && strcmp((char*)items[i], string) == 0) ||
				(!caseSensitive && strcasecmp((char*)items[i], string) == 0))
			{
				return i;
			}
		}
		else if (!string && !items[i])
		{
			return i;
		}
	}
	return -1;
}

bool TCStringArray::removeString(const char* string)
{
	return removeStringAtIndex(indexOfString(string));
}

bool TCStringArray::removeStringAtIndex(size_t index)
{
	if (index < count)
	{
		delete[] stringAtIndex(index);
		return removeItemAtIndex(index);
	}
	else
	{
		return false;
	}
}

void TCStringArray::removeAll(void)
{
	while (count)
	{
		removeStringAtIndex(count - 1);
	}
}

char* TCStringArray::operator[](size_t index)
{
	return stringAtIndex(index);
}

const char* TCStringArray::operator[](size_t index) const
{
	return stringAtIndex(index);
}

const char* TCStringArray::stringAtIndex(size_t index) const
{
	return (const char*)((TCStringArray *)this)->itemAtIndex(index);
}

char* TCStringArray::stringAtIndex(size_t index)
{
	return (char*)itemAtIndex(index);
}

bool TCStringArray::readFile(const char* filename)
{
	FILE* file = ucfopen(filename, "r");

	if (file)
	{
		char buf[1024];

		while (1)
		{
			size_t length;

			if (!fgets(buf, 1024, file))
			{
				fclose(file);
				return true;
			}
			length = strlen(buf);
			if (buf[length - 1] != '\n')
			{
				fprintf(stderr, "Line too long.\n");
				return false;
			}
			buf[--length] = 0;
			if (length && buf[length - 1] == '\r')
			{
				buf[--length] = 0;
			}
			addString(buf);
		}
	}
	else
	{
		return false;
	}
}

void TCStringArray::copyContents(TCStringArray *otherStringArray) const
{
	size_t i;

	for (i = 0; i < count; i++)
	{
		otherStringArray->items[i] = copyString((char*)items[i]);
		otherStringArray->count = count;
	}
}

TCObject *TCStringArray::copy(void) const
{
	TCStringArray *newStringArray = new TCStringArray(count, caseSensitive);

	copyContents(newStringArray);
	return newStringArray;
}
