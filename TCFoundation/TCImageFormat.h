#ifndef __TCIMAGEFORMAT_H__
#define __TCIMAGEFORMAT_H__

#include <TCFoundation/TCAlertSender.h>
#include <TCFoundation/TCImage.h>

class TCExport TCImageFormat : public TCAlertSender
{
	public:
		TCImageFormat(void);

		virtual bool checkSignature(const TCByte *data, long length) = 0;
		virtual bool checkSignature(const char *filename);
		virtual bool checkSignature(FILE *file) = 0;
		virtual bool loadData(TCImage *image, TCByte *data, long length) = 0;
		virtual bool loadFile(TCImage *image, const char *filename);
		virtual bool loadFile(TCImage *image, FILE *file) = 0;
		virtual bool saveFile(TCImage *image, const char *filename);
		virtual bool saveFile(TCImage *image, FILE *file) = 0;
		char *getName(void) { return name; }
		virtual void setProgressCallback(TCImageProgressCallback value,
			void *userData)
		{
			progressCallback = value;
			progressUserData = userData;
		}
	virtual TCImageOptions *newCompressionOptions(void);
	protected:
		virtual ~TCImageFormat(void);
		virtual void dealloc(void);
		virtual bool callProgressCallback(CUCSTR message, float progress);

		char *name;
		TCImageProgressCallback progressCallback;
		void *progressUserData;
};

#endif // __TCIMAGEFORMAT_H__
