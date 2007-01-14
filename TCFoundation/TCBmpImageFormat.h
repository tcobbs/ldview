#ifndef __TCBMPIMAGEFORMAT_H__
#define __TCBMPIMAGEFORMAT_H__

#include <TCFoundation/TCImageFormat.h>

class TCExport TCBmpImageFormat : public TCImageFormat
{
	public:
		TCBmpImageFormat(void);

		virtual bool checkSignature(const TCByte *data, long length);
		virtual bool checkSignature(FILE *file);
		virtual bool loadData(TCImage *image, TCByte *data, long length);
		virtual bool loadFile(TCImage *image, FILE *file);
		virtual bool saveFile(TCImage *image, FILE *file,
			TCImageProgressCallback progressCallback,
			void *progressUserData);
	protected:
		virtual ~TCBmpImageFormat(void);
		virtual void dealloc(void);
		virtual bool readValue(FILE *file, unsigned short &value);
		virtual bool readValue(FILE *file, unsigned long &value);
		virtual bool readValue(FILE *file, long &value);
		virtual bool readFileHeader(TCImage *image, FILE *file);
		virtual bool readInfoHeader(TCImage *image, FILE *file);
		virtual bool readImageData(TCImage *image, FILE *file);
		virtual bool writeValue(FILE *file, unsigned short value);
		virtual bool writeValue(FILE *file, unsigned long value);
		virtual bool writeValue(FILE *file, long value);
		virtual bool writeFileHeader(TCImage *image, FILE *file);
		virtual bool writeInfoHeader(TCImage *image, FILE *file);
		virtual bool writeImageData(TCImage *image, FILE *file,
			TCImageProgressCallback progressCallback,
			void *progressUserData);
};

#endif // __TCBMPIMAGEFORMAT_H__
