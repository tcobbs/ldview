#include "TCImage.h"
#include "TCPngImageFormat.h"
#include "TCBmpImageFormat.h"
#include "TCJpegImageFormat.h"
#include "TCImageOptions.h"
#include "mystring.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
		 comment(NULL),
		 compressionOptions(NULL)
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
	TCObject::release(compressionOptions);
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
		// Note: Due to what may or may not be a bug in the Microsoft linker,
		// image formats CANNOT self-register.  In order for a class to be
		// included from a library into an executable, it has to be referenced
		// from other code that is used from the library directly.  Declaring a
		// global variable that does this doesn't work.
		imageFormats = new TCImageFormatArray;
		addImageFormat(new TCPngImageFormat, true);
		addImageFormat(new TCBmpImageFormat, true);
		addImageFormat(new TCJpegImageFormat, true);
	}
}

void TCImage::setFormatName(const char *value)
{
	if (value != formatName && (value == NULL || formatName == NULL ||
		strcmp(value, formatName) != 0))
	{
		delete formatName;
		formatName = copyString(value);
		TCObject::release(compressionOptions);
		compressionOptions = NULL;
	}
}

bool TCImage::loadData(
	TCByte *data,
	long length,
	TCImageProgressCallback progressCallback /*= NULL*/,
	void *progressUserData /*= NULL*/)
{
	TCImageFormat *imageFormat = formatForData(data, length);

	if (imageFormat)
	{
		imageFormat->setProgressCallback(progressCallback, progressUserData);
		if (imageFormat->loadData(this, data, length))
		{
			setFormatName(imageFormat->getName());
			return true;
		}
	}
	return false;
}

bool TCImage::loadFile(
	const char *filename,
	TCImageProgressCallback progressCallback /*= NULL*/,
	void *progressUserData /*= NULL*/)
{
	TCImageFormat *imageFormat = formatForFile(filename);

	if (imageFormat)
	{
		imageFormat->setProgressCallback(progressCallback, progressUserData);
		if (imageFormat->loadFile(this, filename))
		{
			setFormatName(imageFormat->getName());
			return true;
		}
	}
	return false;
}

bool TCImage::saveFile(
	const char *filename,
	TCImageProgressCallback progressCallback /*= NULL*/,
	void *progressUserData /*= NULL*/)
{
	TCImageFormat *imageFormat = formatWithName(formatName);

	if (imageFormat)
	{
		imageFormat->setProgressCallback(progressCallback, progressUserData);
		if (imageFormat->saveFile(this, filename))
		{
			getCompressionOptions()->save();
		}
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

void TCImage::autoCrop(void)
{
	autoCrop(imageData[0], imageData[1], imageData[2]);
}

void TCImage::autoCrop(TCByte r, TCByte g, TCByte b)
{
	int minx = 0;
	int maxx = width - 1;
	int miny = 0;
	int maxy = height - 1;
	int bytesPerLine;
	bool found = false;
	int x;
	int y;

	bytesPerLine = roundUp(width * bytesPerPixel, 4);
	for (x = 0; x < width && !found; x++)
	{
		int xOffset = x * bytesPerPixel;

		for (y = 0; y < height && !found; y++)
		{
			int offset = xOffset + y * bytesPerLine;

			if (imageData[offset] != r || imageData[offset + 1] != g ||
				imageData[offset + 2] != b)
			{
				found = true;
				minx = x;
			}
		}
	}
	if (found)
	{
		int newWidth;
		int newHeight;
		int newBytesPerLine;
		TCByte *newImageData;

		found = false;
		for (y = 0; y < height && !found; y++)
		{
			int yOffset = y * bytesPerLine;

			for (x = 0; x < width && !found; x++)
			{
				int offset = yOffset + x * bytesPerPixel;

				if (imageData[offset] != r || imageData[offset + 1] != g ||
					imageData[offset + 2] != b)
				{
					found = true;
					miny = y;
				}
			}
		}
		found = false;
		for (x = width - 1; x >= 0 && !found; x--)
		{
			int xOffset = x * bytesPerPixel;

			for (y = 0; y < height && !found; y++)
			{
				int offset = xOffset + y * bytesPerLine;

				if (imageData[offset] != r || imageData[offset + 1] != g ||
					imageData[offset + 2] != b)
				{
					found = true;
					maxx = x;
				}
			}
		}
		found = false;
		for (y = height - 1; y >= 0 && !found; y--)
		{
			int yOffset = y * bytesPerLine;

			for (x = 0; x < width && !found; x++)
			{
				int offset = yOffset + x * bytesPerPixel;

				if (imageData[offset] != r || imageData[offset + 1] != g ||
					imageData[offset + 2] != b)
				{
					found = true;
					maxy = y;
				}
			}
		}
		newWidth = maxx - minx + 1;
		newHeight = maxy - miny + 1;
		newBytesPerLine = roundUp(newWidth * bytesPerPixel, 4);
		newImageData = new TCByte[newHeight * newBytesPerLine];
		for (y = 0; y < newHeight; y++)
		{
			memcpy(&newImageData[y * newBytesPerLine],
				&imageData[(y + miny) * bytesPerLine + minx * bytesPerPixel],
				newBytesPerLine);
		}
		if (!userImageData)
		{
			delete imageData;
		}
		imageData = newImageData;
		userImageData = false;
		width = newWidth;
		height = newHeight;
	}
}

TCImageOptions *TCImage::getCompressionOptions(void)
{
	if (!compressionOptions)
	{
		TCImageFormat *imageFormat = formatWithName(formatName);

		if (imageFormat)
		{
			compressionOptions = imageFormat->newCompressionOptions();
		}
	}
	return compressionOptions;
}
