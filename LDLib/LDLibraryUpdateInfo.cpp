#include "LDLibraryUpdateInfo.h"
#include <TCFoundation/mystring.h>

LDLibraryUpdateInfo::LDLibraryUpdateInfo(void)
	:m_updateType(LDLibraryUnknownUpdate),
	m_name(NULL),
	m_date(NULL),
	m_exeUrl(NULL),
	m_zipUrl(NULL),
	m_exeSize(-1),
	m_zipSize(-1)
{
}

LDLibraryUpdateInfo::~LDLibraryUpdateInfo(void)
{
}

void LDLibraryUpdateInfo::dealloc(void)
{
	delete m_name;
	delete m_date;
	delete m_exeUrl;
	delete m_zipUrl;
	TCObject::dealloc();
}

bool LDLibraryUpdateInfo::parseUpdateLine(const char *updateLine)
{
	int partCount;
	char **updateParts = componentsSeparatedByString(updateLine, "\t",
		partCount);
	bool retValue = false;

	if (partCount > 2)
	{
		if (stringHasPrefix(updateParts[0], "FULL"))
		{
			m_updateType = LDLibraryFullUpdate;
		}
		else if (stringHasPrefix(updateParts[0], "UPDATE"))
		{
			m_updateType = LDLibraryPartialUpdate;
		}
		if (m_updateType != LDLibraryUnknownUpdate)
		{
			unsigned len;

			m_exeUrl = copyString(updateParts[2]);
			len = strlen(m_exeUrl);
			if (len >= 12)
			{
				char *filename = m_exeUrl + len - 12;

				if (stringHasCaseInsensitivePrefix(filename, "lcad") &&
					stringHasCaseInsensitiveSuffix(filename, ".exe"))
				{
					m_name = new char[9];
					strncpy(m_name, filename, 8);
					m_name[8] = 0;
				}
				else if (strcasecmp(filename, "complete.exe") == 0)
				{
					m_name = copyString("complete");
				}
			}
			if (m_name)
			{
				retValue = true;
				m_date = copyString(updateParts[1]);
				if (partCount > 3)
				{
					m_zipUrl = copyString(updateParts[3]);
				}
			}
			else
			{
				delete m_exeUrl;
				m_exeUrl = NULL;
			}
		}
	}
	deleteStringArray(updateParts, partCount);
	return retValue;
}

int LDLibraryUpdateInfo::compare(const TCObject *other) const
{
	LDLibraryUpdateInfo *otherInfo = (LDLibraryUpdateInfo *)other;

	return strcmp(m_date, otherInfo->m_date);
}
