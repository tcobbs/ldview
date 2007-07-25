#import "LDrawModelView.h"
#import "ModelWindow.h"
#import <LDLib/LDrawModelViewer.h>
#include <TCFoundation/TCMacros.h>
#include <TRE/TREGLExtensions.h>
#include <TCFoundation/TCImage.h>

@implementation LDrawModelView

- (void)dealloc
{
	TCObject::release(modelViewer);
	TCObject::release(resizeCornerImage);
	[lastMoveTime release];
	[super dealloc];
}

- (void)rotationUpdate
{
	if (!redisplayRequested)
	{
		redisplayRequested = YES;
		[self performSelectorOnMainThread:@selector(redisplay) withObject:nil waitUntilDone:NO];
	}
}

- (void)loadResizeCornerImage:(NSData *)pngImageData
{
	if (pngImageData)
	{
		TCImage *tcImage = new TCImage;
		
		tcImage->setFlipped(true);
		tcImage->setLineAlignment(4);
		tcImage->setDataFormat(TCRgba8);
		if (tcImage->loadData((TCByte *)[pngImageData bytes], [pngImageData length]))
		{
			TCByte *srcData = tcImage->getImageData();
			int srcRowSize = tcImage->getRowSize();
			TCByte *dstData;
			int dstRowSize;

			resizeCornerImage = new TCImage;
			resizeCornerImage->setDataFormat(TCRgba8);
			resizeCornerImage->setFlipped(true);
			resizeCornerImage->setLineAlignment(4);
			resizeCornerImage->setSize(16, 16);
			resizeCornerImage->allocateImageData();
			dstData = resizeCornerImage->getImageData();
			dstRowSize = resizeCornerImage->getRowSize();
			memset(dstData, 0, dstRowSize * resizeCornerImage->getHeight());
			for (int i = 0; i < 15; i++)
			{
				memcpy(&dstData[dstRowSize * i + 4], &srcData[srcRowSize * i], 4 * 15);
			}
		}
		tcImage->release();
	}
}

- (void)setupWithFrame:(NSRect)frame
{
	NSData *fontData = [NSData dataWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"SansSerif" ofType:@"fnt"]];
	NSImage *resizeCornerNSImage = [NSImage imageNamed:@"NSGrayResizeCorner"];
	NSData *pngImageData = nil;

	if (resizeCornerNSImage)
	{
		pngImageData = [[[resizeCornerNSImage representations] objectAtIndex:0] representationUsingType:NSPNGFileType properties:nil];
	}
	if (!pngImageData)
	{
		pngImageData = [NSData dataWithContentsOfFile:
			[[NSBundle mainBundle] pathForResource:@"MyResizeCorner" ofType:@"png"]];
	}
	if (pngImageData)
	{
		[self loadResizeCornerImage:pngImageData];
	}
	redisplayRequested = NO;
	modelViewer = new LDrawModelViewer((int)frame.size.width,
		(int)frame.size.height);
	modelViewer->setFontData((TCByte *)[fontData bytes], [fontData length]);
}

- (NSOpenGLPixelFormat *)customPixelFormat
{
	NSOpenGLPixelFormatAttribute attrs[] =
	{
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)1,
		NSOpenGLPFABackingStore, (NSOpenGLPixelFormatAttribute)NO,
		NSOpenGLPFAAccelerated, (NSOpenGLPixelFormatAttribute)NO,
		(NSOpenGLPixelFormatAttribute)0
	};
	return [[[NSOpenGLPixelFormat alloc] initWithAttributes:attrs] autorelease];
}

- (void)initWithCoder:(NSCoder *)decoder
{
	[self setupWithFrame:[self frame]];
	[super initWithCoder:decoder];
}

- (void)initWithFrame:(NSRect)frame pixelFormat:(NSOpenGLPixelFormat *)format
{
	[format release];
	[self initWithFrame:frame];
}

- (void)initWithFrame:(NSRect)frame
{
	[self setupWithFrame:frame];
	[super initWithFrame:frame pixelFormat:[self customPixelFormat]];
}

- (ModelWindow *)modelWindow
{
	return (ModelWindow *)[[self window] delegate];
}

- (void)redisplay
{
	redisplayRequested = NO;
	if ([NSApp nextEventMatchingMask:NSAnyEventMask untilDate:nil inMode:NSDefaultRunLoopMode dequeue:NO])
	{
		// If there are any events in the event queue, we don't want to redisplay yet; that causes
		// the event queue to get jammed up with unprocessed events.
		[self rotationUpdate];
	}
	else
	{
		[self setNeedsDisplay:YES];
	}
}

- (void)reshape
{
	NSRect frame = [self frame];
	modelViewer->setWidth((int)frame.size.width);
	modelViewer->setHeight((int)frame.size.height);
}

- (LDrawModelViewer *)modelViewer
{
	return modelViewer;
}

- (BOOL)openModel:(NSString *)filename
{
	BOOL retValue = NO;

	if (modelViewer)
	{
		loading = YES;
		modelViewer->setFilename([filename cStringUsingEncoding:
			NSASCIIStringEncoding]);
		if (modelViewer->loadModel(true))
		{
			retValue = YES;
		}
		else
		{
			NSRunAlertPanel(@"Error", @"Error loading file", @"OK", nil, nil);
		}
		loading = NO;
		[self rotationUpdate];
	}
	return retValue;
}

- (void)altMouseDown:(NSEvent *)event
{
	[self rotationUpdate];
	rightMouseDownModifierFlags = [event modifierFlags];
	if (!lButtonDown)
	{
		if (viewMode == LDVViewExamine)
		{
			originalZoomY = [event locationInWindow].y;
			rButtonDown = YES;
		}
	}
}

- (void)mouseDown:(NSEvent *)event
{
	[self rotationUpdate];
	mouseDownModifierFlags = [event modifierFlags];
	if (mouseDownModifierFlags & NSControlKeyMask)
	{
		[self altMouseDown:event];
	}
	else
	{
		lButtonDown = YES;
		lastMouseLocation = [event locationInWindow];
		lastFrameMouseLocation = lastMouseLocation;
		modelViewer->setRotationSpeed(0.0);
		rotationSpeed = 0.0f;
		//NSLog(@"Stopped?\n");
	}
}

- (void)rightMouseDown:(NSEvent *)event
{
	[self altMouseDown:event];
}

- (void)altMouseUp:(NSEvent *)event
{
	[self rotationUpdate];
	if (rButtonDown)
	{
		rButtonDown = NO;
		modelViewer->setZoomSpeed(0.0f);
	}
}

- (void)mouseUp:(NSEvent *)event
{
	//NSLog(@"mouseUp: (%g, %g)\n", lastMouseLocation.x, lastMouseLocation.y);
	if (mouseDownModifierFlags & NSControlKeyMask)
	{
		[self altMouseUp:event];
	}
	else
	{
		lButtonDown = NO;
		[self rotationUpdate];
		modelViewer->setCameraXRotate(0.0f);
		modelViewer->setCameraYRotate(0.0f);
	}
}

- (void)rightMouseUp:(NSEvent *)event
{
	[self altMouseUp:event];
}

- (void)updateZoom:(float)yPos
{
	float magnitude = yPos - originalZoomY;

	modelViewer->setZoomSpeed(-magnitude / 2.0f);
}

- (void)altMouseDragged:(NSEvent *)event
{
	[self rotationUpdate];
	if (rightMouseDownModifierFlags & NSAlternateKeyMask)
	{
		modelViewer->setClipZoom(true);
	}
	else
	{
		modelViewer->setClipZoom(false);
	}
	[self updateZoom:[event locationInWindow].y];
}

- (void)rightMouseDragged:(NSEvent *)event
{
	[self altMouseDragged:event];
}

- (void)updatePanXY:(NSPoint)mouseLocation
{
	float deltaX = mouseLocation.x - lastMouseLocation.x;
	float deltaY = mouseLocation.y - lastMouseLocation.y;
	
	lastMouseLocation = mouseLocation;
	modelViewer->panXY((int)deltaX, (int)-deltaY);
}

- (void)updateSpinRateXY:(NSPoint)mouseLocation
{
	float deltaX = mouseLocation.x - lastMouseLocation.x;
	float deltaY = mouseLocation.y - lastMouseLocation.y;
	float magnitude = (float)sqrt(deltaX * deltaX + deltaY * deltaY);

	//NSLog(@"lastMouseLocation: (%g, %g); mouseLocation: (%g, %g)", lastMouseLocation.x, lastMouseLocation.y, mouseLocation.x, mouseLocation.y);
	lastMouseLocation = mouseLocation;
	rotationSpeed = magnitude / 10.0f;
	if (fEq(rotationSpeed, 0.0f))
	{
		rotationSpeed = 0.0f;
		modelViewer->setXRotate(1.0f);
		modelViewer->setYRotate(1.0f);
	}
	else
	{
		modelViewer->setXRotate(-deltaY);
		modelViewer->setYRotate(deltaX);
	}
	modelViewer->setRotationSpeed(rotationSpeed);
}

- (void)updateSpinRate
{
	//NSLog(@"Waiting for mouse up...\n");
	NSEvent *mouseUpEvent = [[self window] nextEventMatchingMask:NSLeftMouseUpMask untilDate:nil inMode:NSDefaultRunLoopMode dequeue:NO];
	if (mouseUpEvent)
	{
		//[self mouseUp:mouseUpEvent];
	}
	//NSLog(@"Found? %@\n", mouseUpEvent);
	
	// if mouseUpEvent has a value, then there's a mouse up in the queue, and we don't want
	// to stop our spinning.
	if (lButtonDown && !mouseUpEvent)
	{
		if ([[NSDate date] timeIntervalSinceReferenceDate] -
			[lastMoveTime timeIntervalSinceReferenceDate] > 0.1 ||
			(lastFrameMouseLocation.x == lastMouseLocation.x &&
			 lastFrameMouseLocation.y == lastMouseLocation.y))
		{
			[self updateSpinRateXY:lastMouseLocation];
		}
	}
}

- (void)mouseDragged:(NSEvent *)event
{
	if (mouseDownModifierFlags & NSControlKeyMask)
	{
		[self altMouseDragged:event];
	}
	else
	{
		[self rotationUpdate];
		[lastMoveTime release];
		lastMoveTime = [[NSDate alloc] init];
		NSPoint mouseLocation = [event locationInWindow];
		if (viewMode == LDVViewExamine)
		{
			if (mouseDownModifierFlags & NSCommandKeyMask)
			{
				[self updatePanXY:mouseLocation];
			}
			else
			{
				[self updateSpinRateXY:mouseLocation];
			}
		}
	}
}

- (void)scrollWheel:(NSEvent *)event
{
	[self rotationUpdate];
	if ([event modifierFlags] & NSAlternateKeyMask)
	{
		modelViewer->setClipZoom(YES);
	}
	else
	{
		modelViewer->setClipZoom(NO);
	}
	modelViewer->zoom([event deltaY] * -10.0f);
}

- (void)reloadNeeded
{
	if (modelViewer)
	{
		[[self openGLContext] makeCurrentContext];
		if (modelViewer->getNeedsReload())
		{
			modelViewer->reload();
		}
		[[self openGLContext] makeCurrentContext];
		if (modelViewer->getNeedsRecompile())
		{
			modelViewer->recompile();
		}
		[self rotationUpdate];
	}
	loading = NO;
}

- (void)awakeFromNib
{
	[super setPixelFormat:[self customPixelFormat]];
	[[self openGLContext] makeCurrentContext];
	TREGLExtensions::setup();
}

- (void)prepResizeCornerTexture
{
	if (resizeCornerTextureId == 0)
	{
		glGenTextures(1, (GLuint *)&resizeCornerTextureId);
		glBindTexture(GL_TEXTURE_2D, resizeCornerTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, resizeCornerImage->getWidth(), resizeCornerImage->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, resizeCornerImage->getImageData());
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, resizeCornerTextureId);
	}
	if (resizeCornerTextureId != 0)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
}

- (void)drawRect:(NSRect)rect
{
	if (modelViewer && (modelViewer->getNeedsReload() || modelViewer->getNeedsRecompile()))
	{
		[[self modelWindow] modelWillReload];
		loading = YES;
	}
	if (loading || !modelViewer || modelViewer->getUpdating())
	{
		[[NSColor blackColor]  set];
		NSRectFill(rect);
		return;
	}
	[[self openGLContext] makeCurrentContext];
	modelViewer->update();
	if (fps != 0.0f)
	{
		modelViewer->drawFPS(fps);
	}
	if (resizeCornerImage && ![[self modelWindow] showStatusBar])
	{
		glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
		[self prepResizeCornerTexture];
		glEnable(GL_TEXTURE_2D);
		modelViewer->orthoView();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glTranslatef(rect.size.width - 16.0, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(16.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(16.0f, 16.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(0.0f, 16.0f);
		glEnd();
		glPopAttrib();
	}
	//glFinish();
	[self updateSpinRate];
	lastFrameMouseLocation = lastMouseLocation;
	if (rotationSpeed > 0.0f || !fEq(modelViewer->getZoomSpeed(), 0.0f))
	{
		[self rotationUpdate];
		[[self modelWindow] performSelectorOnMainThread:@selector(updateFps) withObject:nil waitUntilDone:NO];
	}
	else
	{
		[[self modelWindow] performSelectorOnMainThread:@selector(clearFps) withObject:nil waitUntilDone:NO];
		//[[self modelWindow] clearFps];
	}
	long swapInterval;
	[[self openGLContext] getValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
	[[self openGLContext] flushBuffer];
}

- (void)resetView:(id)sender
{
	modelViewer->resetView();
	[self rotationUpdate];
}

- (void)setFps:(float)value
{
	fps = value;
}

@end
