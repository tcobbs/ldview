#if defined(USE_CPP11) || !defined(_NO_BOOST)

#include "LDLibraryUpdateInfo.h"
#include <TCFoundation/mystring.h>
#include <stdio.h>

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

LDLibraryUpdateInfo::LDLibraryUpdateInfo(void)
	:m_updateType(LDLibraryUnknownUpdate),
	m_format(LDLibraryUnknownFormat),
	m_name(NULL),
	m_date(NULL),
	m_url(NULL),
	m_size(-1)
{
}

LDLibraryUpdateInfo::~LDLibraryUpdateInfo(void)
{
}

void LDLibraryUpdateInfo::dealloc(void)
{
	delete[] m_name;
	delete[] m_date;
	delete[] m_url;
	TCObject::dealloc();
}

bool LDLibraryUpdateInfo::parseUpdateLine(const char *updateLine)
{
	int partCount;
	char **updateParts = componentsSeparatedByString(updateLine, "\t",
		partCount);
	bool retValue = false;

	if (updateParts && partCount > 4)
	{
		if (stringHasPrefix(updateParts[0], "COMPLETE"))
		{
			m_updateType = LDLibraryFullUpdate;
		}
		else if (stringHasPrefix(updateParts[0], "UPDATE"))
		{
			m_updateType = LDLibraryPartialUpdate;
		}
		else if (stringHasPrefix(updateParts[0], "BASE"))
		{
			m_updateType = LDLibraryBaseUpdate;
		}
	}
	else
	{
		return false;
	}
	if (m_updateType != LDLibraryUnknownUpdate)
	{
		if (stringHasPrefix(updateParts[1], "ARJ"))
		{
			m_format = LDLibraryExeFormat;
		}
		else if (stringHasPrefix(updateParts[1], "ZIP"))
		{
			m_format = LDLibraryZipFormat;
		}
	}
	if (m_format != LDLibraryUnknownFormat)
	{
		m_date = copyString(updateParts[2]);
	}
	if (m_date && m_date[0])
	{
		m_url = copyString(updateParts[3]);
	}
	if (m_url && m_url[0])
	{
		char *spot = strrchr(m_url, '/');
		
		if (spot)
		{
			m_name = copyString(spot + 1);
			spot = strchr(m_name, '.');
			if (spot)
			{
				*spot = 0;
			}
			else
			{
				m_name[0] = 0;
			}
		}
	}
	if (m_name && m_name[0])
	{
		if (sscanf(updateParts[4], "%d", &m_size) == 1)
		{
			retValue = true;
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

#endif // USE_CPP11 || !_NO_BOOST
