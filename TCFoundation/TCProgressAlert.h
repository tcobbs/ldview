#ifndef __TCPROGRESSALERT_H__
#define __TCPROGRESSALERT_H__

#include <TCFoundation/TCAlert.h>

class TCProgressAlert : public TCAlert
{
public:
	TCProgressAlert(const char *source, const char *message, float progress,
		TCStringArray *extraInfo = NULL);
	float getProgress(void) { return m_progress; }
	void abort(void) { m_aborted = true; }
	bool getAborted(void) { return m_aborted; }
	const char *getSource(void) { return m_source; }

	static TCULong alertClass(void) { return 1; }
	static void send(const char *source, const char *message, float progress,
		bool *aborted = NULL, TCStringArray *extraInfo = NULL);
	static void send(const char *source, const char *message, float progress,
		TCStringArray *extraInfo);
protected:
	virtual ~TCProgressAlert(void);
	virtual void dealloc(void);

	char *m_source;
	float m_progress;
	bool m_aborted;
};

#endif // __TCPROGRESSALERT_H__
