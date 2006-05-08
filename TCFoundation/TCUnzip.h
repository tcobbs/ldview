#ifndef __TCUNZIP_H__
#define __TCUNZIP_H__

#include <TCFoundation/TCObject.h>

#ifdef _QT
#include <stdlib.h>
#endif // _QT

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
