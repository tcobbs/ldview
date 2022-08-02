#ifndef __LDLIBRARYUPDATEINFO_H__
#define __LDLIBRARYUPDATEINFO_H__

#include <TCFoundation/TCObject.h>

typedef enum
{
	LDLibraryFullUpdate,
	LDLibraryPartialUpdate,
	LDLibraryBaseUpdate,
	LDLibraryUnknownUpdate
} LDLibraryUpdateType;

typedef enum
{
	LDLibraryZipFormat,
	LDLibraryExeFormat,
	LDLibraryUnknownFormat
} LDLibraryUpdateFormat;

class LDLibraryUpdateInfo : public TCObject
{
public:
	LDLibraryUpdateInfo(void);
	bool parseUpdateLine(const char *updateLine);
	bool isFullUpdate(void) { return m_updateType == LDLibraryFullUpdate; }
	bool isPartialUpdate(void)
	{
		return m_updateType == LDLibraryPartialUpdate;
	}
	LDLibraryUpdateType getUpdateType(void) const { return m_updateType; }
	LDLibraryUpdateFormat getFormat(void) const { return m_format; }
	const char *getName(void) const { return m_name; }
	const char *getDate(void) const { return m_date; }
	const char *getUrl(void) const { return m_url; }
	int getSize(void) const { return m_size; }
	virtual int compare(const TCObject *other) const;
protected:
	virtual ~LDLibraryUpdateInfo(void);
	virtual void dealloc(void);

	LDLibraryUpdateType m_updateType;
	LDLibraryUpdateFormat m_format;
	char *m_name;
	char *m_date;
	char *m_url;
	int m_size;
};

#endif // __LDLIBRARYUPDATEINFO_H__
