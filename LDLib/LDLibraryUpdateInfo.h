#ifndef __LDLIBRARYUPDATEINFO_H__
#define __LDLIBRARYUPDATEINFO_H__

#include <TCFoundation/TCObject.h>

typedef enum
{
	LDLibraryFullUpdate,
	LDLibraryPartialUpdate,
	LDLibraryUnknownUpdate
} LDLibraryUpdateType;

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
	LDLibraryUpdateType getUpdateType(void) { return m_updateType; }
	const char *getName(void) { return m_name; }
	const char *getDate(void) { return m_date; }
	const char *getExeUrl(void) { return m_exeUrl; }
	const char *getZipUrl(void) { return m_zipUrl; }
	int getExeSize(void) { return m_exeSize; }
	int getZipSize(void) { return m_zipSize; }
	virtual int compare(const TCObject *other) const;
protected:
	virtual ~LDLibraryUpdateInfo(void);
	virtual void dealloc(void);

	LDLibraryUpdateType m_updateType;
	char *m_name;
	char *m_date;
	char *m_exeUrl;
	char *m_zipUrl;
	int m_exeSize;
	int m_zipSize;
};

#endif // __LDLIBRARYUPDATEINFO_H__
