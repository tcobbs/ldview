#include "TCSortedStringArray.h"
#include "mystring.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

TCSortedStringArray::TCSortedStringArray(size_t allocated,
										 bool caseSensitive)
				  :TCStringArray(allocated, caseSensitive)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TCSortedStringArray");
#endif
}

TCSortedStringArray::~TCSortedStringArray(void)
{
}

size_t TCSortedStringArray::addString(const char* newString)
{
	ptrdiff_t i;
	size_t step = (count + 1) / 2;
	int (*compare)(const char*, const char*);
	int comparison;

	if (caseSensitive)
	{
		compare = strcmp;
	}
	else
	{
		compare = strcasecmp;
	}
	if (count)
	{
		i = step - 1;
		while (step > 1 && i >= 0 && i < (ptrdiff_t)count)
		{
			char *item = (char*)items[i];

			comparison = compare(item, newString);
			step = (step + 1) / 2;
			if (comparison < 0)
			{
				i += step;
				if (i >= (ptrdiff_t)count)
				{
					i = count - 1;
				}
			}
			else if (comparison > 0)
			{
				i -= step;
				if (i < 0)
				{
					i = 0;
				}
			}
			else
			{
				return -1;
			}
		}
		if (i < 0)
		{
			i = 0;
		}
		else if (i >= (ptrdiff_t)count)
		{
			i = count;
		}
		else
		{
			do
			{
				char *item = (char*)items[i];

				comparison = compare(item, newString);
				if (comparison < 0)
				{
					i++;
				}
				else if (comparison == 0)
				{
					return -1;
				}
			}
			while (comparison < 0 && i < (ptrdiff_t)count);
		}
	}
	else
	{
		i = 0;
	}
	insertString(newString, i);
	return i;
}

void TCSortedStringArray::insertString(const char* newString, size_t index)
{
	TCStringArray::insertString(newString, index);
}

bool TCSortedStringArray::replaceString(const char* newString, size_t index)
{
	return TCStringArray::replaceString(newString, index);
}

ptrdiff_t TCSortedStringArray::indexOfString(const char* string)
{
	ptrdiff_t i;
	size_t step = (count + 1) / 2;
	int (*compare)(const char*, const char*);
	int comparison;

	if (caseSensitive)
	{
		compare = strcmp;
	}
	else
	{
		compare = strcasecmp;
	}
	if (count)
	{
		i = step - 1;
		while (step > 1 && i >= 0 && i < (ptrdiff_t)count)
		{
			char *item = (char*)items[i];

			comparison = compare(item, string);
			step = (step + 1) / 2;
			if (comparison < 0)
			{
				i += step;
				if (i >= (ptrdiff_t)count)
				{
					i = count - 1;
				}
			}
			else if (comparison > 0)
			{
				i -= step;
				if (i < 0)
				{
					i = 0;
				}
			}
			else
			{
				return i;
			}
		}
		if (i >= 0 && i < (ptrdiff_t)count)
		{
			do
			{
				comparison = compare((char*)items[i], string);
				if (comparison < 0)
				{
					i++;
				}
				else if (comparison == 0)
				{
					return i;
				}
			}
			while (comparison < 0 && i < (ptrdiff_t)count);
		}
	}
	return -1;
}

TCObject *TCSortedStringArray::copy(void) const
{
	TCStringArray *newSortedStringArray =
		new TCSortedStringArray(count, caseSensitive);

	copyContents(newSortedStringArray);
	return newSortedStringArray;
}
