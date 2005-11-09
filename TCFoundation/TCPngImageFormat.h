#ifndef __TCPNGIMAGEFORMAT_H__
#define __TCPNGIMAGEFORMAT_H__

#include <TCFoundation/TCImageFormat.h>

#include <png.h>

class TCExport TCPngImageFormat : public TCImageFormat
{
public:
	TCPngImageFormat(void);

	virtual bool checkSignature(const TCByte *data, long length);
	virtual bool checkSignature(FILE *file);
	virtual bool loadData(TCImage *image, TCByte *data, long length);
	virtual bool loadFile(TCImage *image, FILE *file);
	virtual bool saveFile(TCImage *image, FILE *file,
		TCImageProgressCallback progressCallback,
		void *progressUserData);
protected:
	virtual ~TCPngImageFormat(void);
	virtual void dealloc(void);
	bool setup(void);
	bool setupProgressive(void);
	void infoCallback(void);
	void rowCallback(png_bytep rowData, png_uint_32 rowNum);

	static void staticInfoCallback(png_structp pngPtr, png_infop infoPtr);
	static void staticRowCallback(png_structp pngPtr, png_bytep newRow,
		png_uint_32 rowNum, int pass);

	png_structp pngPtr;
	png_infop infoPtr;
	unsigned long imageWidth;
	unsigned long imageHeight;
	TCImage *image;
	int pngRowSize;
	char **commentData;
	int commentDataCount;
};

#endif // __TCPNGIMAGEFORMAT_H__
