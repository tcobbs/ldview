#include <TRE/TREGLExtensions.h>
#import "LDrawModelView.h"
#import "ModelWindow.h"
#import "OCLocalStrings.h"
#import "SnapshotTaker.h"
#import "PrintAccessoryViewOwner.h"
#import "LDViewController.h"
#import "Preferences.h"

#include <LDLib/LDrawModelViewer.h>
#include <LDLib/LDInputHandler.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCMacros.h>
#include <TCFoundation/TCImage.h>
#include <TCFoundation/TCDefines.h>
#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCUserDefaults.h>

#if MAC_OS_X_VERSION_MAX_ALLOWED <= 1040

typedef float CGFloat;

#endif // Tiger or earlier

@implementation LDrawModelView

static NSOpenGLContext *sharedContext = nil;
static BOOL loadResizeCornerImageTried = NO;
static TCImage *resizeCornerImage = NULL;

- (void)dealloc
{
	if (modelViewer)
	{
		modelViewer->openGlWillEnd();
	}
	TCObject::release(modelViewer);
	[super dealloc];
}

- (TCULong)convertKeyModifiers:(unsigned long)osModifiers
{
	TCULong retValue = 0;

	if (osModifiers & NSShiftKeyMask)
	{
		retValue |= LDInputHandler::MKShift;
	}
	if (osModifiers & NSCommandKeyMask)
	{
		retValue |= LDInputHandler::MKControl;
	}
	if (osModifiers & NSControlKeyMask)
	{
		retValue |= LDInputHandler::MKAppleControl;
	}
	return retValue;
}
	
- (void)rotationUpdate
{
	if (!redisplayRequested)
	{
		redisplayRequested = YES;
		[self performSelectorOnMainThread:@selector(redisplay) withObject:nil waitUntilDone:NO];
	}
}

- (TCImage *)tcImageFromNSImage:(NSImage *)image
{
	if (image == nil)
	{
		return NULL;
	}
	TCImage *tcImage = NULL;
	// Dimensions - source image determines context size
	
	NSSize imageSize = image.size;
	NSRect imageRect = [self convertRectToBacking:NSMakeRect(0, 0, imageSize.width, imageSize.height)];
	imageSize.width = imageRect.size.width;
	imageSize.height = imageRect.size.height;

	// Create a context to hold the image data
	
	CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	
	tcImage = new TCImage;
	tcImage->setFlipped(false);
	tcImage->setLineAlignment(4);
	tcImage->setDataFormat(TCRgba8);
	tcImage->setSize((int)imageSize.width, (int)imageSize.height);
	tcImage->allocateImageData();
	TCByte* pixels = tcImage->getImageData();
	memset(pixels, 1, (int)imageSize.height * tcImage->getRowSize());
	CGContextRef ctx = CGBitmapContextCreate(pixels,
											 imageSize.width,
											 imageSize.height,
											 8,
											 tcImage->getRowSize(),
											 colorSpace,
											 kCGImageAlphaPremultipliedLast);
	CGColorSpaceRelease(colorSpace);
	
	// Wrap graphics context
	
	NSGraphicsContext* gctx = [NSGraphicsContext graphicsContextWithCGContext:ctx flipped:NO];
	// Make our bitmap context current and render the NSImage into it
	
	NSGraphicsContext *origCtx = [NSGraphicsContext currentContext];
	[NSGraphicsContext setCurrentContext:gctx];
	[image drawInRect:imageRect];
	[NSGraphicsContext setCurrentContext:origCtx];
	CGContextRelease(ctx);
	return tcImage;
}

- (void)loadResizeCornerImage:(TCImage *)tcImage
{
	if (tcImage)
	{
		TCByte *srcData = tcImage->getImageData();
		int srcRowSize = tcImage->getRowSize();
		TCByte *dstData;
		int dstRowSize;
		int srcWidth = tcImage->getWidth();
		int srcHeight = tcImage->getHeight();
		int dstWidth = srcWidth < 16 ? 16 : 32;
		int dstHeight = srcHeight < 16 ? 16 : 32;
		int shiftBytes = (dstWidth - srcWidth) * 4;

		resizeCornerImage = new TCImage;
		resizeCornerImage->setDataFormat(TCRgba8);
		resizeCornerImage->setFlipped(true);
		resizeCornerImage->setLineAlignment(4);
		resizeCornerImage->setSize(dstWidth, dstHeight);
		resizeCornerImage->allocateImageData();
		dstData = resizeCornerImage->getImageData();
		dstRowSize = resizeCornerImage->getRowSize();
		memset(dstData, 0, dstRowSize * resizeCornerImage->getHeight());
		for (int i = 0; i < srcHeight; i++)
		{
			int rowOfs = dstRowSize * i;
			int srcRow = tcImage->getFlipped() ? i : (srcHeight - i - 1);
			memcpy(&dstData[rowOfs + shiftBytes], &srcData[srcRowSize * srcRow], 4 * srcWidth);
			for (int x = 3; x < dstRowSize; x += 4)
			{
				dstData[rowOfs + x] = (TCByte)((double)dstData[rowOfs + x] * 0.667);
			}
		}
	}
}

- (void)loadResizeCornerImage
{
	if (!resizeCornerImage && !loadResizeCornerImageTried)
	{
		TCImage *tcImage = NULL;

		loadResizeCornerImageTried = YES;
		NSImage *resizeImage = [NSImage imageNamed:@"MyResizeCorner"];
		tcImage = [self tcImageFromNSImage:resizeImage];
		if (tcImage)
		{
			[self loadResizeCornerImage:tcImage];
			tcImage->release();
		}
	}
}

- (void)setupWithFrame:(NSRect)frame
{
	NSData *fontData = [NSData dataWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"SansSerif" ofType:@"fnt"]];
	// macOS doesn't want resize corners anymore, so don't load it. Note that
	// having it work right when a window moves from a Retina display to a non-
	// Retina display also takes extra work.
//	[self loadResizeCornerImage];
	redisplayRequested = NO;
	modelViewer = new LDrawModelViewer((int)frame.size.width,
		(int)frame.size.height);
	modelViewer->setFontData((TCByte *)[fontData bytes], [fontData length]);
	NSString *font2xPath = [[NSBundle mainBundle] pathForResource:@"SanSerif@2x" ofType:@"png"];
	if (font2xPath)
	{
		modelViewer->setupFont2x([font2xPath UTF8String]);
	}
	inputHandler = modelViewer->getInputHandler();
#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1060
	if ([self respondsToSelector:@selector(setAcceptsTouchEvents:)])
	{
		[self setAcceptsTouchEvents:YES];
	}
#endif // Post-Tiger
}

- (NSOpenGLPixelFormat *)customPixelFormat
{
	int attrs[] =
	{
		NSOpenGLPFAWindow,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAColorSize, 16,
		NSOpenGLPFAAlphaSize, 4,
		NSOpenGLPFADepthSize, 16,
		NSOpenGLPFAMaximumPolicy,
		// The documentation claims that the following isn't useful, but is MUST
		// be set in order for the full screen context to share with this one.
		// Go figure.
		NSOpenGLPFANoRecovery,
		0
	};
	return [[[NSOpenGLPixelFormat alloc] initWithAttributes:(NSOpenGLPixelFormatAttribute *)attrs] autorelease];
}

- (id)initWithCoder:(NSCoder *)decoder
{
	if ((self = [super initWithCoder:decoder]) != nil)
	{
		[self setupWithFrame:[self frame]];
	}
	return self;
}

- (id)initWithFrame:(NSRect)frame pixelFormat:(NSOpenGLPixelFormat *)format
{
	[format release];
	return [self initWithFrame:frame];
}

- (id)initWithFrame:(NSRect)frame
{
	if ((self = [super initWithFrame:frame pixelFormat:[self customPixelFormat]]) != nil)
	{
		[self setupWithFrame:frame];
	}
	return self;
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
	NSRect bounds = [self bounds];
	NSRect backingBounds = [self convertRectToBacking:bounds];
	CGFloat scaleFactor = backingBounds.size.width / bounds.size.width;
	modelViewer->setScaleFactor(scaleFactor);
	modelViewer->setWidth((int)bounds.size.width);
	modelViewer->setHeight((int)bounds.size.height);
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
		int commandLineStep = [[[self modelWindow] controller] commandLineStep];
	
		loading = YES;
		modelViewer->setFilename([filename cStringUsingEncoding:
			NSASCIIStringEncoding]);
		if (commandLineStep > 0)
		{
			modelViewer->setCommandLineStep(commandLineStep);
		}
		if (modelViewer->loadModel(true))
		{
			retValue = YES;
		}
		loading = NO;
		[self rotationUpdate];
	}
	return retValue;
}

- (void)mouseDown:(NSEvent *)event
{
	NSPoint loc = [event locationInWindow];
	
	inputHandler->mouseDown([self convertKeyModifiers:[event modifierFlags]], LDInputHandler::MBLeft, (int)loc.x, (int)-loc.y);
}

- (void)rightMouseDown:(NSEvent *)event
{
	NSPoint loc = [event locationInWindow];
	
	inputHandler->mouseDown([self convertKeyModifiers:[event modifierFlags]], LDInputHandler::MBRight, (int)loc.x, (int)-loc.y);
}

- (void)otherMouseDown:(NSEvent *)event
{
	NSPoint loc = [event locationInWindow];
	
	inputHandler->mouseDown([self convertKeyModifiers:[event modifierFlags]], LDInputHandler::MBMiddle, (int)loc.x, (int)-loc.y);
}

- (void)mouseUp:(NSEvent *)event
{
	NSPoint loc = [event locationInWindow];
	
	inputHandler->mouseUp([self convertKeyModifiers:[event modifierFlags]], LDInputHandler::MBLeft, (int)loc.x, (int)-loc.y);
}

- (void)rightMouseUp:(NSEvent *)event
{
	NSPoint loc = [event locationInWindow];
	
	inputHandler->mouseUp([self convertKeyModifiers:[event modifierFlags]], LDInputHandler::MBRight, (int)loc.x, (int)-loc.y);
}

- (void)otherMouseUp:(NSEvent *)event
{
	NSPoint loc = [event locationInWindow];
	
	inputHandler->mouseUp([self convertKeyModifiers:[event modifierFlags]], LDInputHandler::MBMiddle, (int)loc.x, (int)-loc.y);
}

- (void)mouseDragged:(NSEvent *)event
{
	NSPoint loc = [event locationInWindow];
	
	inputHandler->mouseMove([self convertKeyModifiers:[event modifierFlags]], (int)loc.x, (int)-loc.y);
}

- (void)rightMouseDragged:(NSEvent *)event
{
	NSPoint loc = [event locationInWindow];
	
	inputHandler->mouseMove([self convertKeyModifiers:[event modifierFlags]], (int)loc.x, (int)-loc.y);
}

- (void)otherMouseDragged:(NSEvent *)event
{
	NSPoint loc = [event locationInWindow];
	
	inputHandler->mouseMove([self convertKeyModifiers:[event modifierFlags]], (int)loc.x, (int)-loc.y);
}

- (LDInputHandler::KeyCode)convertKeyCode:(NSEvent *)theEvent
{
	NSString *characters = [theEvent charactersIgnoringModifiers];
	
	if ([characters length] == 1)
	{
		unichar character = [characters characterAtIndex:0];
		
		if (isalpha(character))
		{
			return (LDInputHandler::KeyCode)(toupper(character) - 'A' + LDInputHandler::KCA);
		}
		else
		{
			switch (character)
			{
			case NSUpArrowFunctionKey:
				return LDInputHandler::KCUp;
			case NSDownArrowFunctionKey:
				return LDInputHandler::KCDown;
			case NSLeftArrowFunctionKey:
				return LDInputHandler::KCLeft;
			case NSRightArrowFunctionKey:
				return LDInputHandler::KCRight;
			case ' ':
				return LDInputHandler::KCSpace;
			case NSPageUpFunctionKey:
				return LDInputHandler::KCPageUp;
			case NSPageDownFunctionKey:
				return LDInputHandler::KCPageDown;
			case NSHomeFunctionKey:
				return LDInputHandler::KCHome;
			case NSEndFunctionKey:
				return LDInputHandler::KCEnd;
			case NSInsertFunctionKey:
				return LDInputHandler::KCInsert;
			case NSDeleteFunctionKey:
				return LDInputHandler::KCDelete;
			case 27:
				return LDInputHandler::KCEscape;
			case '\r':
				return LDInputHandler::KCReturn;
			}
		}
	}
	return LDInputHandler::KCUnknown;
}

- (TCULong)currentKeyModifiers
{
	return 0;
}

- (BOOL)acceptsFirstResponder
{
	return YES;
}

- (BOOL)becomeFirstResponder
{
	return YES;
}

- (BOOL)resignFirstResponder
{
	return YES;
}

- (void)keyDown:(NSEvent *)theEvent
{
	TCULong modifiers = [self convertKeyModifiers:[theEvent modifierFlags]];
	LDInputHandler::KeyCode keyCode = [self convertKeyCode:theEvent];
	
	if (modifiers == 0)
	{
		ModelWindow *modelWindow = [self modelWindow];

		switch (keyCode)
		{
			case LDInputHandler::KCPageUp:
				[modelWindow changeStep:-1];
				return;
			case LDInputHandler::KCPageDown:
				[modelWindow changeStep:1];
				return;
			case LDInputHandler::KCHome:
				[modelWindow changeStep:0];
				return;
			case LDInputHandler::KCEnd:
				[modelWindow changeStep:2];
				return;
			case LDInputHandler::KCEscape:
				[modelWindow escapePressed];
				return;
            default:
                // Do nothing
                break;
		}
	}
	inputHandler->keyDown(modifiers, keyCode);
}

- (void)keyUp:(NSEvent *)theEvent
{
	inputHandler->keyUp([self convertKeyModifiers:[theEvent modifierFlags]], [self convertKeyCode:theEvent]);
}

#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1060

- (void)magnifyWithEvent:(NSEvent *)event
{
	if ([event respondsToSelector:@selector(magnification)])
	{
		// No modifiers.  Pinch zoom is zoom only.
		inputHandler->mouseWheel(0, [event magnification] * 200.0f);
	}
}

- (void)touchesBeganWithEvent:(NSEvent *)event
{
    NSSet *touches = [event touchesMatchingPhase:NSTouchPhaseTouching inView:self];
	if ([touches count] == 3)
	{
		NSTouch *touch = [touches anyObject];
		NSPoint loc = [touch normalizedPosition];

		inputHandler->mouseDown(0, LDInputHandler::MBMiddle, (int)(loc.x * modelViewer->getWidth()), (int)(-loc.y * modelViewer->getHeight()));
		threeFingerPan = true;
		panIdentity = touch.identity;
	}
	else
	{
		[super touchesBeganWithEvent:event];
	}
}

- (void)touchesCancelledWithEvent:(NSEvent *)event
{
	if (threeFingerPan)
	{
		threeFingerPan = false;
		inputHandler->cancelMouseDrag();
	}
	[super touchesCancelledWithEvent:event];
}

- (void)touchesMovedWithEvent:(NSEvent *)event
{
    NSSet *touches = [event touchesMatchingPhase:NSTouchPhaseTouching inView:self];

	if ([touches count] == 3 && threeFingerPan)
	{
		NSTouch *touch = nil;

		for (NSTouch *otherTouch in touches)
		{
			if (otherTouch.identity == panIdentity)
			{
				touch = otherTouch;
				break;
			}
		}
		if (touch != nil)
		{
			NSPoint loc = [touch normalizedPosition];
			inputHandler->mouseMove(0, (int)(loc.x * modelViewer->getWidth()), (int)(-loc.y * modelViewer->getHeight()));
		}
	}
	else if (threeFingerPan)
	{
		inputHandler->cancelMouseDrag();
		threeFingerPan = false;
		[super touchesMovedWithEvent:event];
	}
}

- (void)touchesEndedWithEvent:(NSEvent *)event
{
	if (threeFingerPan)
	{
		inputHandler->cancelMouseDrag();
		threeFingerPan = false;
	}
	else
	{
		[super touchesEndedWithEvent:event];
	}
}

#endif // Post-Tiger

- (void)scrollWheel:(NSEvent *)event
{
	inputHandler->mouseWheel([self convertKeyModifiers:[event modifierFlags]], [event deltaY] * 20.0f);
}

- (void)reload
{
	if (modelViewer)
	{
		[[self openGLContext] makeCurrentContext];
		modelViewer->reload();
		[self rotationUpdate];
	}
	loading = NO;
}

- (void)reloadNeeded
{
	if (modelViewer)
	{
		[[self openGLContext] makeCurrentContext];
		if (modelViewer->getNeedsReload() && !loading)
		{
			loading = YES;
			modelViewer->reload();
			loading = NO;
		}
		else if (modelViewer->getNeedsReparse() && !parsing)
		{
			parsing = YES;
			modelViewer->reparse();
			parsing = NO;
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
	if (sharedContext == nil)
	{
		[super setPixelFormat:[self customPixelFormat]];
		sharedContext = [[super openGLContext] retain];
	}
	else
	{
		[super setOpenGLContext:[[[NSOpenGLContext alloc] initWithFormat:[self customPixelFormat] shareContext:sharedContext] autorelease]];
		// We don't have to set the GL Context's view to ourself because we
		// haven't been shown yet.  That will get done automatically when our
		// window is initially shown (as far as I can tell).
		//[[self openGLContext] setView:self];
	}
	[[self openGLContext] makeCurrentContext];
	TREGLExtensions::setup();
	[[[[self modelWindow] controller] preferences] openGLInitialized];
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

- (BOOL)knowsPageRange:(NSRangePointer)aRange
{
	aRange->location = 1;
	aRange->length = 1;
	return YES;
}

- (NSRect)rectForPage:(NSInteger)pageNumber
{
	return (NSRect) { {0, 0}, [[[NSPrintOperation currentOperation] printInfo] paperSize] };
}

- (NSPoint)locationOfPrintRect:(NSRect)aRect
{
	return NSMakePoint(0.0f, 0.0f);
}

- (void)printRect:(NSRect)rect
{
	NSPrintInfo	*printInfo = [[NSPrintOperation currentOperation] printInfo];
	SnapshotTaker *snapshotTaker = [[SnapshotTaker alloc] initWithModelViewer:modelViewer sharedContext:sharedContext];
	NSImage *image;
	int backgroundR = modelViewer->getBackgroundR();
	int backgroundG = modelViewer->getBackgroundG();
	int backgroundB = modelViewer->getBackgroundB();
	int backgroundA = modelViewer->getBackgroundA();
	float dpi = (float)TCUserDefaults::longForKey(PRINT_DPI_KEY, 300, false);
	NSRect page = { {0.0f, 0.0f}, [printInfo paperSize] };
	float leftMargin = [printInfo leftMargin];
	float topMargin = [printInfo topMargin];
	float rightMargin = [printInfo rightMargin];
	float bottomMargin = [printInfo bottomMargin];
	NSRect printRect = { { leftMargin, bottomMargin }, { page.size.width - leftMargin - rightMargin, page.size.height - topMargin - bottomMargin } };
	TCFloat32 origEdgeWidth = modelViewer->getHighlightLineWidth();
	bool printBackground = TCUserDefaults::boolForKey(PRINT_BACKGROUND_KEY, false, false);
	bool adjustEdges = TCUserDefaults::boolForKey(PRINT_ADJUST_EDGES_KEY, true, false);

	if (!printBackground)
	{
		modelViewer->setBackgroundRGBA(255, 255, 255, 255);
	}
	if (adjustEdges)
	{
		// Assume 100 DPI for screen
		TCFloat32 newEdgeWidth = origEdgeWidth * dpi / 100.0f;

		if (newEdgeWidth < 1.0f)
		{
			newEdgeWidth = 1.0f;
		}
		modelViewer->setHighlightLineWidth(newEdgeWidth);
	}
	image = [snapshotTaker imageWithWidth:(int)(printRect.size.width * dpi / 72.0f) height:(int)(printRect.size.height * dpi / 72.0f) zoomToFit:false];
	[snapshotTaker release];
	if (adjustEdges)
	{
		modelViewer->setHighlightLineWidth(origEdgeWidth);
	}
	if (!printBackground)
	{
		modelViewer->setBackgroundRGBA(backgroundR, backgroundG, backgroundB, backgroundA);
	}
	printRect = NSIntegralRect(printRect);
	[(NSImageRep *)[[image representations] lastObject] drawInRect:printRect];
}

- (void)drawRect:(NSRect)rect
{
	BOOL skip = NO;
	ModelWindow *modelWindow = [self modelWindow];

	if (![NSGraphicsContext currentContextDrawingToScreen])
	{
		[self printRect:rect];
		return;
	}
	if (loading)
	{
		skip = YES;
	}
	else if (modelViewer && modelViewer->getFilename() && (modelViewer->getNeedsReload() || modelViewer->getNeedsRecompile() || modelViewer->getNeedsReparse()))
	{
		[modelWindow modelWillReload];
		skip = YES;
	}
	if (skip || [modelWindow loading] || [modelWindow parsing] || !modelViewer || modelViewer->getUpdating())
	{
		if (modelViewer)
		{
			float r = (float)modelViewer->getBackgroundR() / 255.0f;
			float g = (float)modelViewer->getBackgroundG() / 255.0f;
			float b = (float)modelViewer->getBackgroundB() / 255.0f;

			glClearColor(r, g, b, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			[[self openGLContext] flushBuffer];
		}
		else
		{
			[[NSColor blackColor]  set];
			NSRectFill(rect);
		}
		return;
	}
	[[self openGLContext] makeCurrentContext];
	redrawRequested = false;
	modelViewer->update();
	modelViewer->drawFPS(fps);
	if (resizeCornerImage && ![modelWindow showStatusBar])
	{
		NSRect backingRect = [self convertRectToBacking:rect];
		glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
		[self prepResizeCornerTexture];
		glEnable(GL_TEXTURE_2D);
		modelViewer->orthoView();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glTranslatef(backingRect.size.width - resizeCornerImage->getWidth(), 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f((float)resizeCornerImage->getWidth(), 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f((float)resizeCornerImage->getWidth(), (float)resizeCornerImage->getHeight());
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(0.0f, (float)resizeCornerImage->getHeight());
		glEnd();
		glPopAttrib();
	}
	if (redrawRequested)
	{
		[modelWindow performSelectorOnMainThread:@selector(updateFps) withObject:nil waitUntilDone:NO];
	}
	else
	{
		if ([modelWindow fps] != 0.0)
		{
			[modelWindow performSelectorOnMainThread:@selector(clearFps) withObject:nil waitUntilDone:NO];
		}
		//[modelWindow clearFps];
	}
	[[self openGLContext] flushBuffer];
	[modelWindow updateStatusLatLon];
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

- (void)setViewingAngle:(int)value
{
	if (value >= LDVAngleDefault && value <= LDVAngleIso)
	{
		modelViewer->resetView((LDVAngle)value);
		[self rotationUpdate];
	}
	else
	{
		NSLog(@"Invalid viewing angle.");
	}
}

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
	return modelViewer != NULL && modelViewer->getFilename() != NULL;
}

- (IBAction)zoomToFit:(id)sender
{
	modelViewer->zoomToFit();
	[self rotationUpdate];
}

- (IBAction)rightSideUp:(id)sender
{
	modelViewer->rightSideUp();
	[self rotationUpdate];
}

- (void)modelViewerAlertCallback:(TCAlert *)alert
{
	if (alert)
	{
		//MessageBox(hWindow, alert->getMessage(), "LDView", MB_OK | MB_ICONWARNING);
	}
}

- (void)redrawAlertCallback:(TCAlert *)alert
{
	if (alert->getSender() == modelViewer)
	{
		redrawRequested = true;
		[self rotationUpdate];
	}
}

- (void)captureAlertCallback:(TCAlert *)alert
{
	if (alert->getSender() == inputHandler)
	{
		// Unnecessary?
		//captureMouse();
	}
}

- (void)releaseAlertCallback:(TCAlert *)alert
{
	if (alert->getSender() == inputHandler)
	{
		// Unnecessary?
		//releaseMouse();
	}
}

- (IBAction)viewMode:(id)sender
{
	LDInputHandler::ViewMode viewMode;
	switch ([[sender cell] tagForSegment:[sender selectedSegment]])
	{
		case 1:
			viewMode = LDInputHandler::VMFlyThrough;
			break;
		case 2:
			viewMode = LDInputHandler::VMWalk;
			break;
		default:
			viewMode = LDInputHandler::VMExamine;
			break;
	}
	[self setViewMode:viewMode];
}

- (void)setViewMode:(long)newViewMode
{
	inputHandler->setViewMode((LDInputHandler::ViewMode)newViewMode);
	TCUserDefaults::setLongForKey(newViewMode, VIEW_MODE_KEY, false);
}

- (bool)examineMode
{
	return inputHandler->getViewMode() == LDInputHandler::VMExamine;
}

- (long)viewMode
{
	return inputHandler->getViewMode();
}

- (void)setKeepRightSideUp:(bool)keepRightSideUp
{
	modelViewer->setKeepRightSideUp(keepRightSideUp);
	TCUserDefaults::setBoolForKey(keepRightSideUp, KEEP_RIGHT_SIDE_UP_KEY);
}

- (bool)keepRightSideUp
{
	return modelViewer->getKeepRightSideUp();
}

- (IBAction)print:(id)sender
{
	NSPrintOperation *printOperation = [NSPrintOperation printOperationWithView:self];
	PrintAccessoryViewOwner *printAccessoryViewOwner = [[PrintAccessoryViewOwner alloc] initWithPrintOperation:printOperation];
	
	[printOperation runOperation];
	[printAccessoryViewOwner release];
}

@end
