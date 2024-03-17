#ifndef __LDLFINDFILEALERT_H__
#define __LDLFINDFILEALERT_H__

#include <TCFoundation/TCAlert.h>
#include <stdlib.h>

class LDLFindFileAlert: public TCAlert
{
public:
	LDLFindFileAlert(const std::string &filename);
	void setFilename(const std::string &value);
	const std::string &getFilename(void) { return m_filename; }
	void setFileFound(bool value) { m_fileFound = value; }
	bool getFileFound(void) { return m_fileFound; }
	void setPartFlag(bool value) { m_part = value; }
	bool getPartFlag(void) { return m_part; }

	static const char *alertClass(void) { return "LDLFindFileAlert"; }
protected:
	virtual ~LDLFindFileAlert(void);

	std::string m_filename;
	bool m_fileFound;
	bool m_part;
};

#endif // __LDLFINDFILEALERT_H__
