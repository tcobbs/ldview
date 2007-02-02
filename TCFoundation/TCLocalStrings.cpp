#include "TCLocalStrings.h"
#include "TCDictionary.h"
#include "TCSortedStringArray.h"
#include "mystring.h"

#include <stdio.h>
#include <stdlib.h>
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
	virtual ~TCStringObject(void);
	virtual void dealloc(void)
	{
		delete string;
		TCObject::dealloc();
	}

	char *string;
};

// I got a compiler warning about this not being inlined when it was in the
// class definition, so I pulled it out.
TCStringObject::~TCStringObject(void)
{
}

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

bool TCLocalStrings::setStringTable(const wchar_t *stringTable, bool replace)
{
	return getCurrentLocalStrings()->instSetStringTable(stringTable, replace);
}

const char *TCLocalStrings::get(const char *key)
{
	return getCurrentLocalStrings()->instGetLocalString(key);
}

const wchar_t *TCLocalStrings::get(const wchar_t *key)
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
		TCByte *fileData;

		fseek(tableFile, 0, SEEK_END);
		fileSize = ftell(tableFile);
		fseek(tableFile, 0, SEEK_SET);
		fileData = new TCByte[fileSize + 1];
		if (fread(fileData, 1, fileSize, tableFile) == (unsigned)fileSize)
		{
			bool bUnicode16 = false;
			bool bBigEndian = true;

			// Todo? 32-bit Unicode?
			if (fileData[0] == 0xFF && fileData[1] == 0xFE)
			{
				// Little Endian Unicode
				bUnicode16 = true;
				bBigEndian = false;
			}
			else if (fileData[0] == 0xFE && fileData[1] == 0xFF)
			{
				// Big Endian Unicode
				bUnicode16 = true;
			}
			if (bUnicode16)
			{
				std::wstring wstringTable;
				int i;
				int count = fileSize / 2;

				wstringTable.reserve(count + 1);
				// Note: skip first 2 bytes, which are the Byte Order Mark.
				for (i = 2; i < fileSize; i += 2)
				{
					int uByte;
					int lByte;

					if (bBigEndian)
					{
						uByte = fileData[i];
						lByte = fileData[i + 1];
					}
					else
					{
						uByte = fileData[i + 1];
						lByte = fileData[i];
					}
					wstringTable.push_back((wchar_t)((uByte << 8) | lByte));
				}
				// wstringTable now contains the string table.
				retValue = setStringTable(wstringTable.c_str(), replace);
			}
			else
			{
				char *stringTable = (char *)fileData;

				// Null terminate the string table
				stringTable[fileSize] = 0;
				retValue = setStringTable(stringTable, replace);
			}
		}
		delete fileData;
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

void TCLocalStrings::mbstowstring(
	std::wstring &dst,
	const char *src,
	int length /*= -1*/)
{
	dst.clear();
	if (src)
	{
		mbstate_t state = { 0 };

		if (length == -1)
		{
			length = strlen(src);
		}
		dst.resize(length);
		// Even though we don't check, we can't pass NULL instead of &state and
		// still be thread-safe.
		mbsrtowcs(&dst[0], &src, length + 1, &state);
	}
}

void TCLocalStrings::wcstostring(
	std::string &dst,
	const wchar_t *src,
	int length /*= -1*/)
{
	dst.clear();
	if (src)
	{
		mbstate_t state = { 0 };

		if (length == -1)
		{
			length = wcslen(src);
		}
		dst.resize(length);
		// Even though we don't check, we can't pass NULL instead of &state and
		// still be thread-safe.
		wcsrtombs(&dst[0], &src, length + 1, &state);
	}
}

void TCLocalStrings::wstringtostring(std::string &dst, const std::wstring &src)
{
	const wchar_t *temp = src.c_str();
	mbstate_t state = { 0 };

	dst.resize(src.length() + 1);
	// Even though we don't check, we can't pass NULL instead of &state and
	// still be thread-safe.
	wcsrtombs(&dst[0], &temp, src.length() + 1, &state);
}

void TCLocalStrings::dumpTable(const char *filename, const char *header)
{
	return getCurrentLocalStrings()->instDumpTable(filename, header);
}

void TCLocalStrings::instDumpTable(const char *filename, const char *header)
{
	FILE *file = fopen(filename, "w");

	if (file)
	{
		TCSortedStringArray *keys = stringDict->allKeys();
		int i;
		int count = keys->getCount();

		if (header)
		{
			fprintf(file, "%s\n", header);
		}
		for (i = 0; i < count; i++)
		{
			const char *key = keys->stringAtIndex(i);
			const char *value = ((TCStringObject *)stringDict->objectForKey(key))->getString();

			fprintf(file, "%s = %s\n", key, value);
		}
		for (WStringWStringMap::iterator it = m_strings.begin(); it != m_strings.end(); it++)
		{
			fprintf(file, "%S = %S\n", it->first.c_str(), it->second.c_str());
		}
		fclose(file);
	}
}

bool TCLocalStrings::instSetStringTable(const char *stringTable, bool replace)
{
	bool sectionFound = false;
	int lastKeyIndex = -1;
	std::string lastKey;

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
							std::wstring wkey;
							std::wstring wvalue;

							mbstowstring(wkey, key, keyLen);
							value = copyString(equalSpot + 1);
							processEscapedString(value);
							mbstowstring(wvalue, value);
//							value = stringByReplacingSubstring(equalSpot + 1,
//								"\\n", "\n");
							if (isdigit(key[keyLen - 1]))
							{
								int keyIndex;

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
								}
								keyIndex = atoi(&key[keyLen]);
								key[keyLen] = 0;
								if (lastKey != key)
								{
									lastKeyIndex = 0;
								}
								if (lastKey == key &&
									lastKeyIndex + 1 != keyIndex)
								{
									debugPrintf(
										"Key index out of sequence: %s%d\n",
										key, keyIndex);
								}
								lastKeyIndex = keyIndex;
								lastKey = key;
								mbstowstring(wkey, key, keyLen);
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
									// wstring copy constructor broken in VC++
									// 2005?!?!?  The below doesn't work without
									// the .c_str() calls.
									m_strings[wkey.c_str()] += wvalue.c_str();
								}
							}
							if (!appended)
							{
								if (stringDict->objectForKey(line))
								{
									debugPrintf("Local String key \"%s\" "
										"defined multiple times.\n", line);
								}
								stringObject = new TCStringObject(value);
								stringDict->setObjectForKey(stringObject, line);
								stringObject->release();
								// wstring copy constructor broken in VC++
								// 2005?!?!?  The below doesn't work without the
								// .c_str() calls.
								m_strings[wkey.c_str()] = wvalue.c_str();
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

bool TCLocalStrings::instSetStringTable(const wchar_t *stringTable, bool replace)
{
	bool sectionFound = false;
	int lastKeyIndex = -1;
	std::wstring lastKey;

	if (replace)
	{
		stringDict->removeAll();
	}
	while (1)
	{
		const wchar_t *eol = wcschr(stringTable, '\n');

		if (!eol && wcslen(stringTable) > 0)
		{
			eol = stringTable + wcslen(stringTable);
		}
		if (eol)
		{
			int len = eol - stringTable;
			wchar_t *line = new wchar_t[len + 1];

			wcsncpy(line, stringTable, len);
			line[len] = 0;
			stripCRLF(line);
			stripLeadingWhitespace(line);
			if (!sectionFound)
			{
				// We haven't found the [StringTable] section yet
				stripTrailingWhitespace(line);
				if (wcscasecmp(line, L"[StringTable]") == 0)
				{
					sectionFound = true;
				}
				// Note that we are ignoring all lines until we find the section
			}
			else
			{
				// We're in the [StringTable] section
				if (line[0] == '[' && wcschr(line, ']'))
				{
					// We found another section header, which means we are at
					// the end of the [StringTable] section, so we're done
					break;
				}
				else if (line[0] != ';')
				{
					// Comment lines begin with ;
					wchar_t *equalSpot = wcschr(line, '=');

					if (equalSpot)
					{
						wchar_t *value;
						wchar_t *key = line;
						TCStringObject *stringObject;
						int keyLen;

						*equalSpot = 0;
						stripTrailingWhitespace(key);
						keyLen = wcslen(key);
						if (keyLen)
						{
							bool appended = false;
							std::string skey;
							std::string svalue;

							wcstostring(skey, key, keyLen);
							value = copyString(equalSpot + 1);
							processEscapedString(value);
							wcstostring(svalue, value);
//							value = stringByReplacingSubstring(equalSpot + 1,
//								"\\n", "\n");
							if (isdigit(key[keyLen - 1]))
							{
								int keyIndex;

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
								}
								keyIndex = wcstoul(&key[keyLen], NULL, 10);
								key[keyLen] = 0;
								if (lastKey != key)
								{
									lastKeyIndex = 0;
								}
								if (lastKey == key &&
									lastKeyIndex + 1 != keyIndex)
								{
									debugPrintf(
										"Key index out of sequence: %s%d\n",
										key, keyIndex);
								}
								lastKeyIndex = keyIndex;
								lastKey = key;
								wcstostring(skey, key, keyLen);
								stringObject = (TCStringObject*)stringDict->
									objectForKey(skey.c_str());
								if (stringObject)
								{
									// If we've already got data for this key,
									// we need to append to it and note that we
									// did so.
									char *newValue = new char[svalue.size() +
										strlen(stringObject->getString()) + 1];

									strcpy(newValue, stringObject->getString());
									strcat(newValue, svalue.c_str());
									// Note that we don't have to update the
									// dict; we're simply updating the text in
									// the string object already there.
									stringObject->setString(newValue);
									delete newValue;
									appended = true;
									m_strings[key] += value;
								}
							}
							if (!appended)
							{
								if (stringDict->objectForKey(skey.c_str()))
								{
									debugPrintf("Local String key \"%s\" "
										"defined multiple times.\n",
										skey.c_str());
								}
								stringObject =
									new TCStringObject(svalue.c_str());
								stringDict->setObjectForKey(stringObject,
									skey.c_str());
								stringObject->release();
								m_strings[key] = value;
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

const wchar_t *TCLocalStrings::instGetLocalString(const wchar_t *key)
{
	WStringWStringMap::iterator it = m_strings.find(key);

	if (it != m_strings.end())
	{
		return it->second.c_str();
	}
	else
	{
		std::string temp;

		wstringtostring(temp, key);
		debugPrintf("LocalString %s not found!!!!!!\n", temp.c_str());
		// It should really be NULL, but that means a mistake will likely cause
		// a crash.  At least with an empty string it's less likely to crash.
		return L"";
	}
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
		debugPrintf("LocalString %s not found!!!!!!\n", key);
		// It should really be NULL, but that means a mistake will likely cause
		// a crash.  At least with an empty string it's less likely to crash.
		return "";
	}
}
