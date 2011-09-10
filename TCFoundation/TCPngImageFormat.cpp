#include "TCPngImageFormat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mystring.h"

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

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
	if (setjmp(jumpBuf))
#ifdef WIN32
#pragma warning( pop )
#endif // WIN32
	{
		png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
		return false;
	}
	png_set_progressive_read_fn(pngPtr, this, staticInfoCallback,
		staticRowCallback, NULL);
	canceled = false;
	return true;
}

void TCPngImageFormat::infoCallback(void)
{
	int bitDepth;
	int colorType;

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
	numPasses = png_set_interlace_handling(pngPtr);
	png_read_update_info(pngPtr, infoPtr);
}

float TCPngImageFormat::passToFraction(int pass)
{
	if (numPasses == 1)
	{
		return 1.0f;
	}
	else
	{
		return (float)(1 << pass) / 64.0f;
	}
}

void TCPngImageFormat::rowCallback(
	png_bytep rowData,
	png_uint_32 rowNum,
	int pass)
{
	TCByte *imageData;
	TCByte *rowSpot;
	int imageRowSize;

	if (!rowData)
	{
		return;
	}
	imageData = image->getImageData();
	imageRowSize = image->getRowSize();
	if (image->getFlipped())
	{
		rowSpot = imageData + (imageHeight - rowNum - 1) * imageRowSize;
	}
	else
	{
		rowSpot = imageData + rowNum * imageRowSize;
	}
	png_progressive_combine_row(pngPtr, rowSpot, rowData);
	if (!callProgressCallback(_UC("LoadingPNGRow"),
		((float)rowNum + 1.0f) / imageHeight) * passToFraction(pass))
	{
		canceled = true;
		longjmp(jumpBuf, 1);
	}
}

void TCPngImageFormat::errorCallback(png_const_charp /*msg*/)
{
	longjmp(jumpBuf, 1);
}

void TCPngImageFormat::staticErrorCallback(
	png_structp pngPtr, 
	png_const_charp msg)
{
	TCPngImageFormat *format = (TCPngImageFormat *)png_get_error_ptr(pngPtr);

	if (format)
	{
		format->errorCallback(msg);
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
										 png_uint_32 rowNum, int pass)
{
	TCPngImageFormat *format =
		(TCPngImageFormat *)png_get_progressive_ptr(pngPtr);

	if (format)
	{
		format->rowCallback(newRow, rowNum, pass);
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
		if (setjmp(jumpBuf))
#ifdef WIN32
#pragma warning( pop )
#endif // WIN32
		{
			png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
			return false;
		}
		callProgressCallback(_UC("LoadingPNG"), 0.0f);
		png_process_data(pngPtr, infoPtr, data, length);
		png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
		callProgressCallback(NULL, 2.0f);
		return true;
	}
	return false;
}

bool TCPngImageFormat::setup(void)
{
	pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, this,
		staticErrorCallback, NULL);
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
	if (setupProgressive())
	{
		TCByte buf[1024];

		this->image = image;
#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4611 )
#endif // WIN32
		if (setjmp(jumpBuf))
#ifdef WIN32
#pragma warning( pop )
#endif // WIN32
		{
			png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
			return false;
		}
		callProgressCallback(_UC("LoadingPNG"), 0.0f);
		while (true)
		{
			size_t bytesRead = fread(buf, 1, sizeof(buf), file);

			if (bytesRead == 0)
			{
				break;
			}
			png_process_data(pngPtr, infoPtr, buf, bytesRead);
		}
		png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
		callProgressCallback(NULL, 2.0f);
		return true;
	}
	return false;
//	bool retValue = false;
//
//	canceled = false;
//	pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, this,
//		staticErrorCallback, NULL);
//	if (pngPtr)
//	{
//		infoPtr = png_create_info_struct(pngPtr);
//		if (infoPtr)
//		{
//#ifdef WIN32
//#pragma warning( push )
//#pragma warning( disable : 4611 )
//#endif // WIN32
//			if (!setjmp(jumpBuf))
//#ifdef WIN32
//#pragma warning( pop )
//#endif // WIN32
//			{
//				int bitDepth;
//				int colorType;
//				unsigned int i;
//
//				png_init_io(pngPtr, file);
//				png_read_info(pngPtr, infoPtr);
//				png_get_IHDR(pngPtr, infoPtr, &imageWidth,
//					&imageHeight, &bitDepth, &colorType, NULL,
//					NULL, NULL);
//				if (bitDepth == 8 && (colorType == PNG_COLOR_TYPE_RGB ||
//					colorType == PNG_COLOR_TYPE_RGB_ALPHA))
//				{
//					png_bytep row_pointer;
//					TCByte *imageData;
//					int imageRowSize;
//					int pngRowSize;
//
//					callProgressCallback(_UC("LoadingPNG"), 0.0f);
//					if (colorType == PNG_COLOR_TYPE_RGB)
//					{
//						image->setDataFormat(TCRgb8);
//						pngRowSize = imageWidth * 3;
//					}
//					else
//					{
//						image->setDataFormat(TCRgba8);
//						pngRowSize = imageWidth * 4;
//					}
//					image->setSize(imageWidth, imageHeight);
//					image->allocateImageData();
//					imageData = image->getImageData();
//					imageRowSize = image->getRowSize();
//					row_pointer = new TCByte[pngRowSize];
//					for (i = 0; i < imageHeight; i++)
//					{
//						if (!callProgressCallback(NULL,
//							(float)(i) / (float)imageHeight))
//						{
//							canceled = true;
//							break;
//						}
//						png_read_row(pngPtr, row_pointer, NULL);
//						if (image->getFlipped())
//						{
//							memcpy(imageData + (imageHeight - i - 1) *
//								imageRowSize, row_pointer, pngRowSize);
//						}
//						else
//						{
//							memcpy(imageData + i * imageRowSize,
//								row_pointer, pngRowSize);
//						}
//					}
//					delete row_pointer;
//					retValue = true;
//				}
//				png_read_end(pngPtr, NULL);
//				callProgressCallback(NULL, 1.0f);
//			}
//			png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
//		}
//		else
//		{
//			png_destroy_read_struct(&pngPtr, NULL, NULL);
//		}
//	}
//	callProgressCallback(NULL, 2.0f);
//	return retValue && !canceled;
}

bool TCPngImageFormat::saveFile(TCImage *image, FILE *file)
{
	debugPrintf(2, "TCPngImageFormat::saveFile() 1\n");
	bool retValue = false;
	png_structp pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
		this, staticErrorCallback, NULL);

	canceled = false;
	if (pngPtr)
	{
		png_infop infoPtr = png_create_info_struct(pngPtr);

		if (infoPtr)
		{
#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4611 )
#endif // WIN32
			if (!setjmp(jumpBuf))
#ifdef WIN32
#pragma warning( pop )
#endif // WIN32
			{
				int i;
				int rowSize = image->getRowSize();
				int width = image->getWidth();
				int height = image->getHeight();
				int pngColorType = 0;
				int bitDepth;
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
					bitDepth = 8;
					break;
				case TCRgba8:
					pngColorType = PNG_COLOR_TYPE_RGB_ALPHA;
					bitDepth = 8;
					break;
				case TCRgb16:
					pngColorType = PNG_COLOR_TYPE_RGB;
					bitDepth = 16;
					break;
				case TCRgba16:
					pngColorType = PNG_COLOR_TYPE_RGB_ALPHA;
					bitDepth = 16;
					break;
				default:
					failed = true;
					break;
				}
				if (!failed)
				{
					png_init_io(pngPtr, file);
					png_set_sRGB(pngPtr, infoPtr, PNG_sRGB_INTENT_PERCEPTUAL);
					png_set_IHDR(pngPtr, infoPtr, width, height, bitDepth,
						pngColorType, PNG_INTERLACE_NONE,
						PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
					png_write_info(pngPtr, infoPtr);
					callProgressCallback(_UC("SavingPNG"), 0.0f);
					for (i = 0; i < height; i++)
					{
						if (!callProgressCallback(NULL,
							(float)(i) / (float)height))
						{
							canceled = true;
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
					callProgressCallback(NULL, 1.0f);
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
	callProgressCallback(NULL, 2.0f);
	debugPrintf(2, "TCPngImageFormat::saveFile() 2\n");
	return retValue && !canceled;
}
