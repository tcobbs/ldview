#ifndef __SNAPSHOTTAKER_H__
#define __SNAPSHOTTAKER_H__

#include <TCFoundation/TCObject.h>
#if (QT_VERSION >= 0x50100) && defined(QOFFSCREEN)
#include <QtOpenGL>
#endif

class TCAlert;
class SnapshotAlertHandler;
class LDSnapshotTaker;

class SnapshotTaker: public TCObject
{
public:
	SnapshotTaker();
	bool doCommandLine();
	void snapshotCallback(TCAlert *alert);
protected:
	virtual ~SnapshotTaker(void);
	virtual void dealloc(void);
	bool getUseFBO();
	void cleanupContext();
	
	LDSnapshotTaker *ldSnapshotTaker;
	SnapshotAlertHandler *snapshotAlertHandler;
#if (QT_VERSION >= 0x50100) && defined(QOFFSCREEN)
	QOffscreenSurface *qSurf;
	QOpenGLContext *qOglCtx;
	QOpenGLFramebufferObject *qFbo;
#endif
};

#endif // __SNAPSHOTTAKER_H__
