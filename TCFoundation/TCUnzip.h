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
};

#endif __TCUNZIP_H__
