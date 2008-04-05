#ifndef __TCBMPIMAGEFORMAT_H__
#define __TCBMPIMAGEFORMAT_H__

#include <TCFoundation/TCImageFormat.h>

#define BMP_FILE_HEADER_SIZE 14
#define BMP_INFO_HEADER_SIZE 40

class TCExport TCBmpImageFormat : public TCImageFormat
{
	public:
		TCBmpImageFormat(void);

		virtual bool checkSignature(const TCByte *data, long length);
		virtual bool checkSignature(FILE *file);
		virtual bool loadData(TCImage *image, TCByte *data, long length);
		virtual bool loadFile(TCImage *image, FILE *file);
		virtual bool saveFile(TCImage *image, FILE *file);
	protected:
		virtual ~TCBmpImageFormat(void);
		virtual void dealloc(void);
		static bool readValue(FILE *file, unsigned short &value);
		static bool readValue(FILE *file, unsigned long &value);
		static bool readValue(FILE *file, long &value);
		static bool readFileHeader(TCImage *image, FILE *file);
		static bool readInfoHeader(TCImage *image, FILE *file);
		virtual bool readImageData(TCImage *image, FILE *file);
		static bool writeValue(FILE *file, unsigned short value);
		static bool writeValue(FILE *file, unsigned long value);
		static bool writeValue(FILE *file, long value);
		static bool writeFileHeader(TCImage *image, FILE *file);
		static bool writeInfoHeader(TCImage *image, FILE *file);
		virtual bool writeImageData(TCImage *image, FILE *file);
};

#endif // __TCBMPIMAGEFORMAT_H__
