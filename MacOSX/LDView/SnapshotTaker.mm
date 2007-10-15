//
//  SnapshotTaker.mm
//  LDView
//
//  Created by Travis Cobbs on 10/14/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "SnapshotTaker.h"
#include <LDLib/LDrawModelViewer.h>
#include <TRE/TREGL.h>

#define PB_WIDTH 1024
#define PB_HEIGHT 1024

@implementation SnapshotTaker

- (id)initWithModelViewer:(LDrawModelViewer *)aModelViewer
{
	return [self initWithModelViewer:aModelViewer sharedContext:nil];
}

- (NSOpenGLPixelFormat *)pixelFormat
{
	NSOpenGLPixelFormatAttribute attrs[] =
	{
		NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)24,
		NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute)24,
		NSOpenGLPFAAlphaSize, (NSOpenGLPixelFormatAttribute)8,
		NSOpenGLPFAStencilSize, (NSOpenGLPixelFormatAttribute)8,
		NSOpenGLPFAAccelerated, (NSOpenGLPixelFormatAttribute)NO,
		NSOpenGLPFAPixelBuffer,
		(NSOpenGLPixelFormatAttribute)0
	};
	return [[[NSOpenGLPixelFormat alloc] initWithAttributes:attrs] autorelease];
}

- (id)initWithModelViewer:(LDrawModelViewer *)aModelViewer sharedContext:(NSOpenGLContext *)sharedContext
{
	self = [super init];
	if (self)
	{
		NSOpenGLPixelBuffer *pixelBuffer = [[NSOpenGLPixelBuffer alloc] initWithTextureTarget:GL_TEXTURE_2D textureInternalFormat:GL_RGBA textureMaxMipMapLevel:0 pixelsWide:PB_WIDTH pixelsHigh:PB_HEIGHT];

		modelViewer = aModelViewer;
		ldSnapshotTaker = new LDSnapshotTaker(modelViewer);
		glContext = [[NSOpenGLContext alloc] initWithFormat:[self pixelFormat] shareContext:sharedContext];
		[glContext setPixelBuffer:pixelBuffer cubeMapFace:0 mipMapLevel:0 currentVirtualScreen:[glContext currentVirtualScreen]];
		[pixelBuffer release];
		if (!glContext)
		{
			[self dealloc];
			return nil;
		}
	}
	return self;
}

- (void)dealloc
{
	TCObject::release(ldSnapshotTaker);
	[glContext release];
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

- (bool)saveFile:(NSString *)filename width:(int)width height:(int)height zoomToFit:(bool)zoomToFit
{
	bool retValue;

	[glContext makeCurrentContext];
	glViewport(0, 0, PB_WIDTH, PB_HEIGHT);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glDrawBuffer(GL_FRONT);
	glReadBuffer(GL_FRONT);
	modelViewer->setup();
	retValue = ldSnapshotTaker->saveImage([filename cStringUsingEncoding:NSASCIIStringEncoding], width, height, zoomToFit);
	return retValue;
}

@end
