#ifndef __TCIMAGE_H__
#define __TCIMAGE_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include <stdio.h>

class TCImageFormat;

typedef enum
{
	TCRgb8,
	TCRgba8
} TCImageDataFormat;

typedef bool (*TCImageProgressCallback)(char* message, float progress,
										void* userData);

typedef TCTypedObjectArray<TCImageFormat> TCImageFormatArray;

class TCExport TCImage : public TCObject
{
public:
	TCImage(void);

	virtual void setDataFormat(TCImageDataFormat format);
	TCImageDataFormat getDataFormat(void) { return dataFormat; }
	virtual void setSize(int xSize, int ySize);
	virtual void getSize(int &xSize, int &ySize);
	int getWidth(void) { return width; }
	int getHeight(void) { return height; }
	virtual void setLineAlignment(int value);
	int getLineAlignment(void) { return lineAlignment; }
	virtual void setFlipped(bool value);
	bool getFlipped(void) { return flipped; }
	virtual void allocateImageData(void);
	virtual void setImageData(TCByte *value);
	TCByte *getImageData(void) { return imageData; }
	virtual int getRowSize(void);
//	virtual TCObject *copy(void);
	virtual bool loadData(TCByte *data, long length);
	virtual bool loadFile(const char *filename);
	virtual bool saveFile(const char *filename,
		TCImageProgressCallback progressCallback = NULL,
		void *progressUserData = NULL);
	char *getFormatName(void) { return formatName; }
	virtual void setFormatName(const char *value);
	virtual TCImage *createSubImage(int x, int y, int cx, int cy);
	virtual void setComment(const char *value);
	const char *getComment(void) { return comment; }

	static int roundUp(int value, int nearest);
	static void addImageFormat(TCImageFormat *imageFormat,
		bool release = false);
protected:
	virtual ~TCImage(void);
	virtual void dealloc(void);
	virtual void syncImageData(void);

	static TCImageFormat *formatWithName(char *name);
	static TCImageFormat *formatForData(const TCByte *data, long length);
	static TCImageFormat *formatForFile(const char *filename);
	static TCImageFormat *formatForFile(FILE *file);
	static void initStandardFormats(void);

	TCByte *imageData;
	TCImageDataFormat dataFormat;
	int bytesPerPixel;
	int width;
	int height;
	int lineAlignment;
	bool flipped;
	char *formatName;
	bool userImageData;
	char *comment;

	static TCImageFormatArray *imageFormats;

	static class TCImageCleanup
	{
	public:
		~TCImageCleanup(void);
	} imageCleanup;
	friend class TCImageCleanup;
};

typedef TCTypedObjectArray<TCImage> TCImageArray;

#endif // __TCIMAGE_H__
