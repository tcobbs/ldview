#ifndef __TCIMAGEFORMAT_H__
#define __TCIMAGEFORMAT_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCImage.h>

class TCExport TCImageFormat : public TCObject
{
	public:
		TCImageFormat(void);

		virtual bool checkSignature(const TCByte *data, long length) = 0;
		virtual bool checkSignature(const char *filename);
		virtual bool checkSignature(FILE *file) = 0;
		virtual bool loadData(TCImage *image, TCByte *data, long length) = 0;
		virtual bool loadFile(TCImage *image, const char *filename);
		virtual bool loadFile(TCImage *image, FILE *file) = 0;
		virtual bool saveFile(TCImage *image, const char *filename,
			TCImageProgressCallback progressCallback, void *progressUserData);
		virtual bool saveFile(TCImage *image, FILE *file,
			TCImageProgressCallback progressCallback, void *progressUserData) =
			0;
		char *getName(void) { return name; }
	protected:
		virtual ~TCImageFormat(void);
		virtual void dealloc(void);
		virtual bool callProgressCallback(
			TCImageProgressCallback progressCallback, char *message,
			float progress, void *progressUserData);

		char *name;
};

#endif // __TCIMAGEFORMAT_H__
