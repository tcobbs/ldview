#ifndef __TCALERT_H__
#define __TCALERT_H__

#include <TCFoundation/TCObject.h>

#if defined(_QT) || defined(__APPLE__)
#include <stdlib.h>
#endif // _QT || __APPLE__

#define USER_ALERTS 0x1000

class TCStringArray;

class TCAlert : public TCObject
{
public:
	TCAlert(TCULong alertClass, const char *message,
		TCStringArray *extraInfo = NULL);
	TCULong getAlertClass(void) { return m_alertClass; }
	char *getMessage(void) { return m_message; }
	TCStringArray *getExtraInfo(void) { return m_extraInfo; }
	static TCULong alertClass(void) { return 0; }
protected:
	virtual ~TCAlert(void);
	virtual void dealloc(void);

	TCULong m_alertClass;
	char *m_message;
	TCStringArray *m_extraInfo;
};

#endif // __TCALERT_H__
