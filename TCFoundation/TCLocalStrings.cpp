#include "TCLocalStrings.h"
#include "TCDictionary.h"
#include "mystring.h"
#include <stdio.h>
#include <ctype.h>

class TCStringObject : public TCObject
{
public:
	TCStringObject(void) : string(NULL) {}
	TCStringObject(const char *string) : string(copyString(string)) {}
	void setString(const char *value)
	{
		if (string != value)
		{
			delete string;
			string = copyString(value);
		}
	}
	const char *getString(void) { return string; }
protected:
	virtual ~TCStringObject(void) {}
	virtual void dealloc(void)
	{
		delete string;
		TCObject::dealloc();
	}

	char *string;
};

TCLocalStrings *TCLocalStrings::currentLocalStrings = NULL;
TCLocalStrings::TCLocalStringsCleanup TCLocalStrings::localStringsCleanup;

TCLocalStrings::TCLocalStringsCleanup::~TCLocalStringsCleanup(void)
{
	if (currentLocalStrings)
	{
		currentLocalStrings->release();
	}
}

TCLocalStrings::TCLocalStrings(void)
{
	stringDict = new TCDictionary;
}

TCLocalStrings::~TCLocalStrings(void)
{
}

void TCLocalStrings::dealloc(void)
{
	TCObject::release(stringDict);
	TCObject::dealloc();
}

bool TCLocalStrings::setStringTable(const char *stringTable, bool replace)
{
	return getCurrentLocalStrings()->instSetStringTable(stringTable, replace);
}

const char *TCLocalStrings::get(const char *key)
{
	return getCurrentLocalStrings()->instGetLocalString(key);
}

bool TCLocalStrings::loadStringTable(const char *filename, bool replace)
{
	FILE *tableFile = fopen(filename, "rb");
	bool retValue = false;

	if (tableFile)
	{
		long fileSize;
		char *stringTable;

		fseek(tableFile, 0, SEEK_END);
		fileSize = ftell(tableFile);
		fseek(tableFile, 0, SEEK_SET);
		stringTable = new char[fileSize + 1];
		if (fread(stringTable, 1, fileSize, tableFile) == (unsigned)fileSize)
		{
			// Null terminate the string table
			stringTable[fileSize] = 0;
			retValue = setStringTable(stringTable, replace);
		}
		delete stringTable;
		fclose(tableFile);
	}
	return retValue;
}

TCLocalStrings *TCLocalStrings::getCurrentLocalStrings(void)
{
	if (!currentLocalStrings)
	{
		currentLocalStrings = new TCLocalStrings;
	}
	return currentLocalStrings;
}

bool TCLocalStrings::instSetStringTable(const char *stringTable, bool replace)
{
	bool sectionFound = false;

	if (replace)
	{
		stringDict->removeAll();
	}
	while (1)
	{
		const char *eol = strchr(stringTable, '\n');

		if (!eol && strlen(stringTable) > 0)
		{
			eol = stringTable + strlen(stringTable);
		}
		if (eol)
		{
			int len = eol - stringTable;
			char *line = new char[len + 1];

			strncpy(line, stringTable, len);
			line[len] = 0;
			stripCRLF(line);
			stripLeadingWhitespace(line);
			if (!sectionFound)
			{
				// We haven't found the [StringTable] section yet
				stripTrailingWhitespace(line);
				if (strcasecmp(line, "[StringTable]") == 0)
				{
					sectionFound = true;
				}
				// Note that we are ignoring all lines until we find the section
			}
			else
			{
				// We're in the [StringTable] section
				if (line[0] == '[' && strchr(line, ']'))
				{
					// We found another section header, which means we are at
					// the end of the [StringTable] section, so we're done
					break;
				}
				else if (line[0] != ';')
				{
					// Comment lines begin with ;
					char *equalSpot = strchr(line, '=');

					if (equalSpot)
					{
						char *value;
						char *key = line;
						TCStringObject *stringObject;
						int keyLen;

						*equalSpot = 0;
						stripTrailingWhitespace(key);
						keyLen = strlen(key);
						if (keyLen)
						{
							bool appended = false;

							value = copyString(equalSpot + 1);
							processEscapedString(value);
//							value = stringByReplacingSubstring(equalSpot + 1,
//								"\\n", "\n");
							if (isdigit(key[keyLen - 1]))
							{
								// If the last character of the key is a digit,
								// then it must be a multi-line key.  So strip
								// off all trailing digits, and append to any
								// existing value.  Note that keys aren't
								// allowed to end in a digit, so even if there
								// is only one line, the key still gets the
								// number stripped off the end.
								while (isdigit(key[keyLen - 1]) && keyLen > 0)
								{
									keyLen--;
									key[keyLen] = 0;
								}
								stringObject = (TCStringObject*)stringDict->
									objectForKey(key);
								if (stringObject)
								{
									// If we've already got data for this key,
									// we need to append to it and note that we
									// did so.
									char *newValue = new char[strlen(value) +
										strlen(stringObject->getString()) + 1];

									strcpy(newValue, stringObject->getString());
									strcat(newValue, value);
									// Note that we don't have to update the
									// dict; we're simply updating the text in
									// the string object already there.
									stringObject->setString(newValue);
									delete newValue;
									appended = true;
								}
							}
							if (!appended)
							{
								stringObject = new TCStringObject(value);
								stringDict->setObjectForKey(stringObject, line);
								stringObject->release();
							}
							delete value;
						}
					}
				}
			}
			delete line;
			if (!eol[0])
			{
				// If there isn't an EOL at the end of the file, we're done now.
				break;
			}
			stringTable += len + 1;
			while (stringTable[0] == '\r' || stringTable[0] == '\n')
			{
				stringTable++;
			}
		}
		else
		{
			break;
		}
	}
	// Note that the load is considered a success if the [StringTable] section
	// is found in the data.
	return sectionFound;
}

const char *TCLocalStrings::instGetLocalString(const char *key)
{
	TCStringObject *stringObject =
		(TCStringObject*)stringDict->objectForKey(key);

	if (stringObject)
	{
		return stringObject->getString();
	}
	else
	{
		return NULL;
	}
}
