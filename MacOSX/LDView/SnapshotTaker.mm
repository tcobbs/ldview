//
//  SnapshotTaker.mm
//  LDView
//
//  Created by Travis Cobbs on 10/14/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "SnapshotTaker.h"
#include <LDLib/LDrawModelViewer.h>

#define PB_WIDTH 1024
#define PB_HEIGHT 1024

@implementation SnapshotTaker

- (id)init
{
	return [self initWithModelViewer:nil sharedContext:nil];
}

- (BOOL)choosePixelFormat:(CGLPixelFormatObj *)pPixelFormat sharedContext:(NSOpenGLContext *)sharedContext;
{
	CGLPixelFormatAttribute attrs[] =
	{
		kCGLPFADepthSize, (CGLPixelFormatAttribute)24,
		kCGLPFAColorSize, (CGLPixelFormatAttribute)24,
		kCGLPFAAlphaSize, (CGLPixelFormatAttribute)8,
		kCGLPFAStencilSize, (CGLPixelFormatAttribute)8,
		kCGLPFAAccelerated, (CGLPixelFormatAttribute)NO,
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

- (id)initWithModelViewer:(LDrawModelViewer *)modelViewer sharedContext:(NSOpenGLContext *)sharedContext
{
	self = [super init];
	if (self)
	{
		if (modelViewer)
		{
			ldSnapshotTaker = new LDSnapshotTaker(modelViewer);
		}
		else
		{
			ldSnapshotTaker = new LDSnapshotTaker;
		}
		if (CGLCreatePBuffer(PB_WIDTH, PB_HEIGHT, GL_TEXTURE_2D, GL_RGBA, 0, &pbuffer) == kCGLNoError)
		{			
			CGLPixelFormatObj pixelFormat;

			if ([self choosePixelFormat:&pixelFormat sharedContext:sharedContext])
			{
				if (CGLCreateContext(pixelFormat, (CGLContextObj)[sharedContext CGLContextObj], &context) == kCGLNoError)
				{
					long virtualScreen;

					CGLDestroyPixelFormat(pixelFormat);
					CGLSetCurrentContext(context);
					CGLGetVirtualScreen(context, &virtualScreen);
					CGLSetPBuffer(context, pbuffer, 0, 0, virtualScreen);
					return self;
				}
				CGLDestroyPixelFormat(pixelFormat);
			}
		}
		[self dealloc];
		return nil;
	}
	return self;
}

- (void)dealloc
{
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
	LDrawModelViewer *modelViewer = ldSnapshotTaker->getModelViewer();
	
	CGLSetCurrentContext(context);
	glViewport(0, 0, PB_WIDTH, PB_HEIGHT);
	modelViewer->perspectiveView();
	glViewport(0, 0, PB_WIDTH, PB_HEIGHT);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glDrawBuffer(GL_FRONT);
	glReadBuffer(GL_FRONT);
	if (modelViewer->getMainTREModel() == NULL && !modelViewer->getNeedsReload())
	{
		modelViewer->loadModel(true);
	}
}

- (bool)saveFile
{
	[self saveFileSetup];
	return ldSnapshotTaker->saveImage();
}

- (bool)saveFile:(NSString *)filename width:(int)width height:(int)height zoomToFit:(bool)zoomToFit
{
	[self saveFileSetup];
	return ldSnapshotTaker->saveImage([filename cStringUsingEncoding:NSASCIIStringEncoding], width, height, zoomToFit);
}

@end
