#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mystring.h"

#include "TCPngImageFormat.h"

TCPngImageFormat::TCPngImageFormat(void)
	:commentData(NULL),
	commentDataCount(0)
{
	name = "PNG";
#ifdef _LEAK_DEBUG
	strcpy(className, "TCPngImageFormat");
#endif
}

TCPngImageFormat::~TCPngImageFormat(void)
{
}

void TCPngImageFormat::dealloc(void)
{
	if (commentDataCount)
	{
		deleteStringArray(commentData, commentDataCount);
	}
	TCImageFormat::dealloc();
}

bool TCPngImageFormat::checkSignature(const TCByte *data, long length)
{
	if (length >= 8)
	{
		return png_sig_cmp((TCByte *)data, 0, 8) ? false : true;
	}
	else
	{
		return false;
	}
}

bool TCPngImageFormat::checkSignature(FILE *file)
{
	bool retValue = false;
	TCByte header[8];
	long filePos = ftell(file);

	if (fread(header, 1, 8, file) == 8)
	{
		retValue = png_sig_cmp(header, 0, 8) ? false : true;
	}
	fseek(file, filePos, SEEK_SET);
	return retValue;
}

bool TCPngImageFormat::setupProgressive(void)
{
	if (!setup())
	{
		return false;
	}
#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4611 )
#endif // WIN32
	if (setjmp(png_jmpbuf(pngPtr)))
#ifdef WIN32
#pragma warning( pop )
#endif // WIN32
	{
		png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
		return false;
	}
	png_set_progressive_read_fn(pngPtr, this, staticInfoCallback,
		staticRowCallback, NULL);
	return true;
}

void TCPngImageFormat::infoCallback(void)
{
	int bitDepth;
	int colorType;

	png_read_update_info(pngPtr, infoPtr);
	png_get_IHDR(pngPtr, infoPtr, &imageWidth, &imageHeight, &bitDepth,
		&colorType, NULL, NULL, NULL);
	if (bitDepth != 8 || (colorType == PNG_COLOR_TYPE_RGB &&
		colorType == PNG_COLOR_TYPE_RGB_ALPHA))
	{
		png_error(pngPtr, "Unsupported image type.");
	}
	if (colorType == PNG_COLOR_TYPE_RGB)
	{
		image->setDataFormat(TCRgb8);
		pngRowSize = imageWidth * 3;
	}
	else
	{
		image->setDataFormat(TCRgba8);
		pngRowSize = imageWidth * 4;
	}
	image->setSize(imageWidth, imageHeight);
	image->allocateImageData();
}

void TCPngImageFormat::rowCallback(png_bytep rowData, png_uint_32 rowNum)
{
	TCByte *imageData;
	int imageRowSize;

	imageData = image->getImageData();
	imageRowSize = image->getRowSize();
	if (image->getFlipped())
	{
		memcpy(imageData + (imageHeight - rowNum - 1) * imageRowSize,
			rowData, pngRowSize);
	}
	else
	{
		memcpy(imageData + rowNum * imageRowSize, rowData, pngRowSize);
	}
}

void TCPngImageFormat::staticInfoCallback(png_structp pngPtr,
										  png_infop /*infoPtr*/)
{
	TCPngImageFormat *format =
		(TCPngImageFormat *)png_get_progressive_ptr(pngPtr);

	if (format)
	{
		format->infoCallback();
	}
}

void TCPngImageFormat::staticRowCallback(png_structp pngPtr, png_bytep newRow,
										 png_uint_32 rowNum, int /*pass*/)
{
	TCPngImageFormat *format =
		(TCPngImageFormat *)png_get_progressive_ptr(pngPtr);

	if (format)
	{
		format->rowCallback(newRow, rowNum);
	}
}

bool TCPngImageFormat::loadData(TCImage *image, TCByte *data, long length)
{
	if (setupProgressive())
	{
		this->image = image;
#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4611 )
#endif // WIN32
		if (setjmp(png_jmpbuf(pngPtr)))
#ifdef WIN32
#pragma warning( pop )
#endif // WIN32
		{
			png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
			return false;
		}
		png_process_data(pngPtr, infoPtr, data, length);
		png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
		return true;
	}
	return false;
}

bool TCPngImageFormat::setup(void)
{
	pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!pngPtr)
	{
		return false;
	}
	infoPtr = png_create_info_struct(pngPtr);
	if (!infoPtr)
	{
		png_destroy_read_struct(&pngPtr, NULL, NULL);
		return false;
	}
	return true;
}

bool TCPngImageFormat::loadFile(TCImage *image, FILE *file)
{
	bool retValue = false;

	pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (pngPtr)
	{
		infoPtr = png_create_info_struct(pngPtr);
		if (infoPtr)
		{
#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4611 )
#endif // WIN32
			if (!setjmp(png_jmpbuf(pngPtr)))
#ifdef WIN32
#pragma warning( pop )
#endif // WIN32
			{
				int bitDepth;
				int colorType;
				unsigned int i;

				png_init_io(pngPtr, file);
				png_read_info(pngPtr, infoPtr);
				png_get_IHDR(pngPtr, infoPtr, &imageWidth,
					&imageHeight, &bitDepth, &colorType, NULL,
					NULL, NULL);
				if (bitDepth == 8 && (colorType == PNG_COLOR_TYPE_RGB ||
					colorType == PNG_COLOR_TYPE_RGB_ALPHA))
				{
					png_bytep row_pointer;
					TCByte *imageData;
					int imageRowSize;
					int pngRowSize;

					if (colorType == PNG_COLOR_TYPE_RGB)
					{
						image->setDataFormat(TCRgb8);
						pngRowSize = imageWidth * 3;
					}
					else
					{
						image->setDataFormat(TCRgba8);
						pngRowSize = imageWidth * 4;
					}
					image->setSize(imageWidth, imageHeight);
					image->allocateImageData();
					imageData = image->getImageData();
					imageRowSize = image->getRowSize();
					row_pointer = new TCByte[pngRowSize];
					for (i = 0; i < imageHeight; i++)
					{
						png_read_row(pngPtr, row_pointer, NULL);
						if (image->getFlipped())
						{
							memcpy(imageData + (imageHeight - i - 1) *
								imageRowSize, row_pointer, pngRowSize);
						}
						else
						{
							memcpy(imageData + i * imageRowSize,
								row_pointer, pngRowSize);
						}
					}
					delete row_pointer;
					retValue = true;
				}
			}
			png_read_end(pngPtr, NULL);
			png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
		}
		else
		{
			png_destroy_read_struct(&pngPtr, NULL, NULL);
		}
	}
	return retValue;
}

bool TCPngImageFormat::saveFile(TCImage *image, FILE *file,
								TCImageProgressCallback progressCallback,
								void *progressUserData)
{
	bool retValue = false;
	bool canceledSave = false;
	png_structp pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL);

	if (pngPtr)
	{
		png_infop infoPtr = png_create_info_struct(pngPtr);

		if (infoPtr)
		{
#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4611 )
#endif // WIN32
			if (!setjmp(png_jmpbuf(pngPtr)))
#ifdef WIN32
#pragma warning( pop )
#endif // WIN32
			{
				int i;
				int rowSize = image->getRowSize();
				int width = image->getWidth();
				int height = image->getHeight();
				int pngColorType = 0;
				TCByte *imageData = image->getImageData();
				bool failed = false;

				if (image->getComment() && strlen(image->getComment()) > 0)
				{
					png_textp textPtr;
					int i;
					int count;

					if (commentDataCount)
					{
						deleteStringArray(commentData, commentDataCount);
					}
					commentData =
						componentsSeparatedByString(image->getComment(),
						":!:!:", commentDataCount);
					if (commentDataCount % 2)
					{
						deleteStringArray(commentData, commentDataCount);
						commentData = new char*[2];
						commentDataCount = 2;
						commentData[0] = copyString("Comment");
						commentData[1] = copyString(image->getComment());
					}
					count = commentDataCount / 2;
					textPtr = new png_text[count];
					for (i = 0; i < count; i++)
					{
						memset(&textPtr[i], 0, sizeof(png_textp));
						textPtr[i].compression = PNG_TEXT_COMPRESSION_NONE;
						textPtr[i].key = commentData[i * 2];
						textPtr[i].text = commentData[i * 2 + 1];
					}
					png_set_text(pngPtr, infoPtr, textPtr, count);
					delete textPtr;
				}
				switch (image->getDataFormat())
				{
				case TCRgb8:
					pngColorType = PNG_COLOR_TYPE_RGB;
					break;
				case TCRgba8:
					pngColorType = PNG_COLOR_TYPE_RGB_ALPHA;
					break;
				default:
					failed = true;
					break;
				}
				if (!failed)
				{
					png_init_io(pngPtr, file);
					png_set_sRGB(pngPtr, infoPtr, PNG_sRGB_INTENT_PERCEPTUAL);
					png_set_IHDR(pngPtr, infoPtr, width, height, 8,
						pngColorType, PNG_INTERLACE_NONE,
						PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
					png_write_info(pngPtr, infoPtr);
					callProgressCallback(progressCallback, "Saving PNG.", 0.0f,
						progressUserData);
					for (i = 0; i < height; i++)
					{
						if (!callProgressCallback(progressCallback, NULL,
							(float)(i) / (float)height, progressUserData))
						{
							canceledSave = true;
							break;
						}
						if (image->getFlipped())
						{
							png_write_row(pngPtr,
								imageData + (height - i - 1) * rowSize);
						}
						else
						{
							png_write_row(pngPtr, imageData + i * rowSize);
						}
					}
					png_write_end(pngPtr, infoPtr);
					callProgressCallback(progressCallback, NULL, 1.0f,
						progressUserData);
					retValue = true;
				}
			}
			png_destroy_write_struct(&pngPtr, &infoPtr);
		}
		else
		{
			png_destroy_write_struct(&pngPtr, (png_infopp)NULL);
		}
	}
	callProgressCallback(progressCallback, NULL, 2.0f, progressUserData);
	return retValue && !canceledSave;
}
