#ifndef __LDLFINDFILEALERT_H__
#define __LDLFINDFILEALERT_H__

#include <TCFoundation/TCAlert.h>
#include <stdlib.h>

class LDLFindFileAlert: public TCAlert
{
public:
	LDLFindFileAlert(const char *filename);
	void setFilename(const char *value);
	const char *getFilename(void) { return m_filename; }
	void setFileFound(bool value) { m_fileFound = value; }
	bool getFileFound(void) { return m_fileFound; }

	static TCULong alertClass(void) { return USER_ALERTS + 1; }
protected:
	virtual ~LDLFindFileAlert(void);
	virtual void dealloc(void);

	char *m_filename;
	bool m_fileFound;
};

#endif // __LDLFINDFILEALERT_H__
