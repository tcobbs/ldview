#ifndef __TCJPEGIMAGEFORMAT_H__
#define __TCJPEGIMAGEFORMAT_H__
#ifdef _AIX
#include <stdlib.h>
#endif

#include <TCFoundation/TCImageFormat.h>
#include <setjmp.h>

struct jpeg_decompress_struct;
struct jpeg_compress_struct;
struct jpeg_error_mgr;
struct jpeg_common_struct;

class TCExport TCJpegImageFormat : public TCImageFormat
{
public:
	TCJpegImageFormat(void);

	virtual bool checkSignature(const TCByte *data, long length);
	virtual bool checkSignature(FILE *file);
	virtual bool loadData(TCImage *image, TCByte *data, long length);
	virtual bool loadFile(TCImage *image, FILE *file);
	virtual bool saveFile(TCImage *image, FILE *file);

	void errorExit(jpeg_common_struct &cinfo);
protected:
	virtual ~TCJpegImageFormat(void);
	virtual void dealloc(void);
	bool setup(jpeg_decompress_struct &cinfo, jpeg_error_mgr &jerr);
	bool setup(jpeg_compress_struct &cinfo, jpeg_error_mgr &jerr);


	unsigned long imageWidth;
	unsigned long imageHeight;
	TCImage *image;
	jmp_buf jumpBuf;
};

#endif // __TCJPEGIMAGEFORMAT_H__
