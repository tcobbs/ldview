//
//  SnapshotTaker.mm
//  LDView
//
//  Created by Travis Cobbs on 10/14/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "SnapshotTaker.h"
#include <LDLib/LDrawModelViewer.h>
#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCImage.h>
#include <TCFoundation/TCBmpImageFormat.h>
#import "SnapshotAlertHandler.h"
#include <algorithm>

#define PB_WIDTH 1024
#define PB_HEIGHT 1024

@implementation SnapshotTaker

- (id)init
{
	return [self initWithModelViewer:nil sharedContext:nil];
}

- (BOOL)choosePixelFormat:(CGLPixelFormatObj *)pPixelFormat
{
	CGLPixelFormatAttribute attrs[] =
	{
		kCGLPFADepthSize, (CGLPixelFormatAttribute)24,
		kCGLPFAColorSize, (CGLPixelFormatAttribute)24,
		kCGLPFAAlphaSize, (CGLPixelFormatAttribute)8,
		kCGLPFAStencilSize, (CGLPixelFormatAttribute)8,
		kCGLPFAAccelerated,
		kCGLPFAPBuffer,
		(CGLPixelFormatAttribute)0,
		(CGLPixelFormatAttribute)0
	};
	long num;

	if (sharedContext == nil)
	{
		attrs[sizeof(attrs) / sizeof(attrs[0]) - 2] = kCGLPFARemotePBuffer;
	}
	if (CGLChoosePixelFormat(attrs, pPixelFormat, &num) == kCGLNoError)
	{
		return YES;
	}
	return NO;
}

- (id)initWithModelViewer:(LDrawModelViewer *)theModelViewer sharedContext:(NSOpenGLContext *)theSharedContext
{
	self = [super init];
	if (self)
	{
		sharedContext = theSharedContext;
		modelViewer = theModelViewer;
		snapshotAlertHandler = new SnapshotAlertHandler(self);
		if (modelViewer)
		{
			ldSnapshotTaker = new LDSnapshotTaker(modelViewer);
		}
	}
	return self;
}

- (void)setupContext
{
	if (CGLCreatePBuffer(PB_WIDTH, PB_HEIGHT, GL_TEXTURE_2D, GL_RGBA, 0, &pbuffer) == kCGLNoError)
	{			
		CGLPixelFormatObj pixelFormat;

		if ([self choosePixelFormat:&pixelFormat])
		{
			if (CGLCreateContext(pixelFormat, (CGLContextObj)[sharedContext CGLContextObj], &context) == kCGLNoError)
			{
				long virtualScreen;

				CGLDestroyPixelFormat(pixelFormat);
				CGLSetCurrentContext(context);
				CGLGetVirtualScreen(context, &virtualScreen);
				CGLSetPBuffer(context, pbuffer, 0, 0, virtualScreen);
				return;
			}
			CGLDestroyPixelFormat(pixelFormat);
		}
	}
}

- (void)dealloc
{
	TCObject::release(snapshotAlertHandler);
	TCObject::release(ldSnapshotTaker);
	if (context)
	{
		CGLDestroyContext(context);
	}
	if (pbuffer)
	{
		CGLDestroyPBuffer(pbuffer);
	}
	[super dealloc];
}

- (void)setImageType:(LDSnapshotTaker::ImageType)value
{
	ldSnapshotTaker->setImageType(value);
}

- (void)setTrySaveAlpha:(bool)value
{
	ldSnapshotTaker->setTrySaveAlpha(value);
}

- (void)setAutoCrop:(bool)value
{
	ldSnapshotTaker->setAutoCrop(value);
}

- (void)saveFileSetup
{
	CGLSetCurrentContext(context);
	glViewport(0, 0, PB_WIDTH, PB_HEIGHT);
	if (modelViewer)
	{
		modelViewer->perspectiveView();
	}
	glViewport(0, 0, PB_WIDTH, PB_HEIGHT);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glDrawBuffer(GL_FRONT);
	glReadBuffer(GL_FRONT);
	if (modelViewer)
	{
		if (modelViewer->getMainTREModel() == NULL && !modelViewer->getNeedsReload())
		{
			modelViewer->loadModel(true);
		}
	}
}

- (void)snapshotCallback:(TCAlert *)alert;
{
	if (strcmp(alert->getMessage(), "PreSave") == 0)
	{
		if (!context)
		{
			[self setupContext];
		}
		[self saveFileSetup];
	}
}

- (bool)saveFile
{
	if (ldSnapshotTaker)
	{
		return ldSnapshotTaker->saveImage();
	}
	else
	{
		return LDSnapshotTaker::doCommandLine();
	}
}

- (NSImage *)imageWithWidth:(int)width height:(int)height zoomToFit:(bool)zoomToFit
{
	int actualWidth = width;
	int actualHeight = height;
	int rowSize = TCImage::roundUp(width * 3, 4);
	int headerSize = BMP_HEADER_SIZE;
	TCByte *imageData = (TCByte *)malloc(rowSize * height + headerSize);

	ldSnapshotTaker->setTrySaveAlpha(false);
	if (ldSnapshotTaker->grabImage(actualWidth, actualHeight, zoomToFit, &imageData[headerSize], NULL))
	{
		int newRowSize = TCImage::roundUp(actualWidth * 3, 4);
		int newSize = newRowSize * actualHeight + headerSize;
		TCBmpImageFormat::writeHeader(actualWidth, actualHeight, imageData);
		int x, y;

		// Swap the red and blue channels.
		for (y = 0; y < actualHeight; y++)
		{
			TCByte *spot = &imageData[headerSize + y * newRowSize];

			for (x = 0; x < actualWidth; x++)
			{
				std::swap(spot[0], spot[2]);
				spot += 3;
			}
		}
		// Note: imageData is owned by the NSData object after the below, which
		// will free the memory.
		return [[NSImage alloc] initWithData:[NSData dataWithBytesNoCopy:imageData length:newSize]];
	}
	return nil;
}

- (bool)saveFile:(NSString *)filename width:(int)width height:(int)height zoomToFit:(bool)zoomToFit
{
	[self saveFileSetup];
	return ldSnapshotTaker->saveImage([filename cStringUsingEncoding:NSASCIIStringEncoding], width, height, zoomToFit);
}

@end
