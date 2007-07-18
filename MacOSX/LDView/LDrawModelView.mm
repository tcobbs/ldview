#import "LDrawModelView.h"
#import <LDLib/LDrawModelViewer.h>
#include <TCFoundation/TCMacros.h>

@implementation LDrawModelView

- (void)dealloc
{
	TCObject::release(modelViewer);
	[lastMoveTime release];
	[super dealloc];
}

- (void)setupWithFrame:(NSRect)frame
{
	NSData *fontData = [NSData dataWithContentsOfFile:
		[[NSBundle mainBundle] pathForResource:@"SansSerif" ofType:@"fnt"]];

	modelViewer = new LDrawModelViewer((int)frame.size.width,
		(int)frame.size.height);
	modelViewer->setFontData((TCByte *)[fontData bytes], [fontData length]);
}

- (void)initWithCoder:(NSCoder *)decoder
{
	[self setupWithFrame:[self frame]];
	[super initWithCoder:decoder];
}

- (void)initWithFrame:(NSRect)frame
{
	NSOpenGLPixelFormatAttribute	attrs[] = {
	  NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)1,
	  NSOpenGLPFAAccelerated, (NSOpenGLPixelFormatAttribute)NO,
	  (NSOpenGLPixelFormatAttribute)0};
	NSOpenGLPixelFormat *screenFormat;

	[self setupWithFrame:frame];
	screenFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	[super initWithFrame:frame pixelFormat:screenFormat];
}

- (void)rotationUpdate
{
   [self setNeedsDisplay:YES];
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
		[self setNeedsDisplay:YES];
	}
	return retValue;
}

- (void)altMouseDown:(NSEvent *)event
{
	[self setNeedsDisplay:YES];
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
	[self setNeedsDisplay:YES];
	mouseDownModifierFlags = [event modifierFlags];
	if (mouseDownModifierFlags & NSControlKeyMask)
	{
		[self altMouseDown:event];
	}
	else
	{
		lButtonDown = YES;
		lastMouseLocation = [event locationInWindow];
		modelViewer->setRotationSpeed(0.0);
	}
}

- (void)rightMouseDown:(NSEvent *)event
{
	[self altMouseDown:event];
}

- (void)altMouseUp:(NSEvent *)event
{
	[self setNeedsDisplay:YES];
	if (rButtonDown)
	{
		rButtonDown = NO;
		modelViewer->setZoomSpeed(0.0f);
	}
}

- (void)mouseUp:(NSEvent *)event
{
	if (mouseDownModifierFlags & NSControlKeyMask)
	{
		[self altMouseUp:event];
	}
	else
	{
		lButtonDown = NO;
		[self setNeedsDisplay:YES];
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
	[self setNeedsDisplay:YES];
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
	if (lButtonDown)
	{
		if ([[NSDate date] timeIntervalSinceReferenceDate] -
			[lastMoveTime timeIntervalSinceReferenceDate] > 0.1)
		{
			[self updateSpinRateXY:lastMouseLocation];
			lButtonDown = YES;
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
		[self setNeedsDisplay:YES];
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
	[self setNeedsDisplay:YES];
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

- (void)drawRect:(NSRect)rect
{
	if (loading || !modelViewer || modelViewer->getUpdating())
	{
		[[NSColor blackColor]  set];
		NSRectFill(rect);
		return;
	}
	[[self openGLContext] makeCurrentContext];
	modelViewer->update();
	[self updateSpinRate];
	glFinish();
	if (rotationSpeed > 0.0f || !fEq(modelViewer->getZoomSpeed(), 0.0f))
	{
		[self performSelector:@selector(rotationUpdate) withObject:nil
			afterDelay:0.0];
	}
}

- (void)resetView:(id)sender
{
	[self setNeedsDisplay:YES];
	modelViewer->resetView();
}

@end
