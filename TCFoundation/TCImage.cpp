#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mystring.h"

#include "TCImage.h"
#include "TCPngImageFormat.h"
#include "TCBmpImageFormat.h"

TCImageFormatArray *TCImage::imageFormats = NULL;

TCImage::TCImageCleanup TCImage::imageCleanup;

TCImage::TCImageCleanup::~TCImageCleanup(void)
{
	if (TCImage::imageFormats)
	{
		TCImage::imageFormats->release();
		TCImage::imageFormats = NULL;
	}
}

TCImage::TCImage(void)
		:imageData(NULL),
		 dataFormat(TCRgb8),
		 bytesPerPixel(3),
		 width(0),
		 height(0),
		 lineAlignment(1),
		 flipped(false),
		 formatName(NULL),
		 userImageData(false),
		 comment(NULL)
{
	initStandardFormats();
#ifdef _LEAK_DEBUG
	strcpy(className, "TCImage");
#endif
}

TCImage::~TCImage(void)
{
}

void TCImage::dealloc(void)
{
	if (!userImageData)
	{
		delete imageData;
	}
	delete formatName;
	delete comment;
	TCObject::dealloc();
}

void TCImage::setDataFormat(TCImageDataFormat format)
{
	dataFormat = format;
	switch (dataFormat)
	{
	case TCRgb8:
		bytesPerPixel = 3;
		break;
	case TCRgba8:
		bytesPerPixel = 4;
		break;
	}
	syncImageData();
}

void TCImage::setSize(int xSize, int ySize)
{
	width = xSize;
	height = ySize;
	syncImageData();
}

void TCImage::getSize(int &xSize, int &ySize)
{
	xSize = width;
	ySize = height;
}

void TCImage::setLineAlignment(int value)
{
	lineAlignment = value;
	syncImageData();
}

void TCImage::setFlipped(bool value)
{
	if (value != flipped)
	{
		flipped = value;
	}
}

void TCImage::setImageData(TCByte *value)
{
	if (!userImageData)
	{
		delete imageData;
	}
	imageData = value;
	if (imageData)
	{
		userImageData = true;
	}
	else
	{
		userImageData = false;
	}
}

void TCImage::allocateImageData(void)
{
	if (!userImageData)
	{
		delete imageData;
	}
	if (width > 0 && height > 0)
	{
		imageData = new TCByte[roundUp(width * bytesPerPixel, lineAlignment) *
			height];
	}
	else
	{
		imageData = NULL;
	}
}

int TCImage::getRowSize(void)
{
	return roundUp(width * bytesPerPixel, lineAlignment);
}

int TCImage::roundUp(int value, int nearest)
{
	return (value + nearest - 1) / nearest * nearest;
}

void TCImage::syncImageData(void)
{
	if (imageData)
	{
		allocateImageData();
	}
}

void TCImage::addImageFormat(TCImageFormat *imageFormat, bool release)
{
	imageFormats->addObject(imageFormat);
	if (release)
	{
		imageFormat->release();
	}
}

void TCImage::initStandardFormats(void)
{
	if (!imageFormats)
	{
		imageFormats = new TCImageFormatArray;
		addImageFormat(new TCPngImageFormat, true);
		addImageFormat(new TCBmpImageFormat, true);
	}
}

void TCImage::setFormatName(const char *value)
{
	delete formatName;
	formatName = copyString(value);
}

bool TCImage::loadData(TCByte *data, long length)
{
	TCImageFormat *imageFormat = formatForData(data, length);

	if (imageFormat)
	{
		if (imageFormat->loadData(this, data, length))
		{
			setFormatName(imageFormat->getName());
			return true;
		}
	}
	return false;
}

bool TCImage::loadFile(const char *filename)
{
	TCImageFormat *imageFormat = formatForFile(filename);

	if (imageFormat)
	{
		if (imageFormat->loadFile(this, filename))
		{
			setFormatName(imageFormat->getName());
			return true;
		}
	}
	return false;
}

bool TCImage::saveFile(const char *filename,
					   TCImageProgressCallback progressCallback,
					   void *progressUserData)
{
	TCImageFormat *imageFormat = formatWithName(formatName);

	if (imageFormat)
	{
		return imageFormat->saveFile(this, filename, progressCallback,
			progressUserData);
	}
	return false;
}

TCImageFormat *TCImage::formatForFile(const char *filename)
{
	TCImageFormat *retValue = NULL;
	FILE *file = fopen(filename, "rb");

	if (file)
	{
		retValue = formatForFile(file);
		fclose(file);
	}
	return retValue;
}

TCImageFormat *TCImage::formatForFile(FILE *file)
{
	int i;
	int count = imageFormats->getCount();

	for (i = 0; i < count; i++)
	{
		TCImageFormat *imageFormat = imageFormats->objectAtIndex(i);
		if (imageFormat->checkSignature(file))
		{
			return imageFormat;
		}
	}
	return NULL;
}

TCImageFormat *TCImage::formatForData(const TCByte *data, long length)
{
	int i;
	int count = imageFormats->getCount();

	for (i = 0; i < count; i++)
	{
		TCImageFormat *imageFormat = imageFormats->objectAtIndex(i);
		if (imageFormat->checkSignature(data, length))
		{
			return imageFormat;
		}
	}
	return NULL;
}

TCImageFormat *TCImage::formatWithName(char *name)
{
	if (name)
	{
		int i;
		int count = imageFormats->getCount();

		for (i = 0; i < count; i++)
		{
			TCImageFormat *imageFormat = imageFormats->objectAtIndex(i);
			if (imageFormat->getName() && strcmp(name, imageFormat->getName())
				== 0)
			{
				return imageFormat;
			}
		}
	}
	return NULL;
}

TCImage *TCImage::createSubImage(int x, int y, int cx, int cy)
{
	TCImage *newImage;
	int i;
	int rowSize;
	int newRowSize;

	if (x + cx > width || y + cy > height)
	{
		return NULL;
	}
	newImage = new TCImage;
	newImage->dataFormat = dataFormat;
	newImage->bytesPerPixel = bytesPerPixel;
	newImage->width = cx;
	newImage->height = cy;
	newImage->lineAlignment = lineAlignment;
	newImage->flipped = flipped;
	newImage->setFormatName(formatName);
	newImage->allocateImageData();
	rowSize = getRowSize();
	newRowSize = newImage->getRowSize();
	for (i = 0; i < cy; i++)
	{
		if (flipped)
		{
			memcpy(newImage->imageData + (cy - i - 1) * newRowSize,
				imageData + (height - y - i - 1) * rowSize + x * bytesPerPixel,
				newRowSize);
		}
		else
		{
			memcpy(newImage->imageData + i * newRowSize,
				imageData + (y + i) * rowSize + x * bytesPerPixel, newRowSize);
		}
	}
	return newImage;
}

void TCImage::setComment(const char *value)
{
	if (value != comment)
	{
		delete comment;
		comment = copyString(value);
	}
}
