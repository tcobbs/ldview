#ifndef __TCUNZIP_H__
#define __TCUNZIP_H__

#include <TCFoundation/TCObject.h>

#ifdef COCOA
#define UNZIP_CMD
#endif // COCOA

#if defined (_QT) || defined (__APPLE__) || defined(_OSMESA)
#include <stdlib.h>
#endif // _QT || __APPLE__ || _OSMESA

class TCUnzip : public TCObject
{
public:
	TCUnzip(void);
	int unzip(const char *filename, const char *outputDir = NULL);
	static bool supported(void);
protected:
	~TCUnzip(void);
	virtual void dealloc(void);
#ifdef WIN32
	int unzipWin32(const char *filename, const char *outputDir);
#else // WIN32
#ifdef UNZIP_CMD
	int unzipExec(const char *filename, const char *outputDir);
	void unzipChildExec(const char *filename, const char *outputDir);
#endif // UNZIP_CMD
#endif // WIN32
};

#endif // __TCUNZIP_H__
