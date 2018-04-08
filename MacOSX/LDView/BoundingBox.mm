#import "BoundingBox.h"
#import "ModelWindow.h"
#import "LDrawModelView.h"
#import "LDViewCategories.h"

#include <LDLib/LDrawModelViewer.h>

@implementation BoundingBox

static BoundingBox *sharedInstance = nil;

- (id)init
{
	if ((self = [super init]) != nil)
	{
		[self ldvLoadNibNamed:@"BoundingBox" topLevelObjects:&topLevelObjects];
		[topLevelObjects retain];
	}
	return self;
}

+ (id)sharedInstance
{
	if (!sharedInstance)
	{
		sharedInstance = [[BoundingBox alloc] init];
	}
	return sharedInstance;
}

+ (BOOL)sharedInstanceIsVisible
{
	if (sharedInstance != nil)
	{
		return [sharedInstance isVisible];
	}
	else
	{
		return NO;
	}
}

- (void)awakeFromNib
{
	NSSize size = [panel contentMaxSize];
	size.height = [panel contentMinSize].height;
	// IB doesn't allow the min/max height to be the same, even when the min/max
	// width AREN'T the same.
	[panel setContentMaxSize:size];
}

- (void)dealloc
{
	[self releaseTopLevelObjects:topLevelObjects orTopLevelObject:panel];
	[super dealloc];
}

- (void)setModelWindow:(ModelWindow *)theModelWindow
{
	if (theModelWindow != modelWindow)
	{
		if (modelWindow)
		{
			LDrawModelViewer *modelViewer = [[modelWindow modelView] modelViewer];
			
			if (modelViewer != NULL)
			{
				modelViewer->setShowBoundingBox(false);
			}
		}
		[modelWindow release];
		modelWindow = [theModelWindow retain];
	}
}

- (void)update:(ModelWindow *)theModelWindow
{
	LDrawModelViewer *modelViewer = [[theModelWindow modelView] modelViewer];

	[self setModelWindow:theModelWindow];
	if (modelViewer != NULL && modelViewer->getMainModel() != NULL)
	{
		TCVector boundingMin = modelViewer->getBoundingMin();
		TCVector boundingMax = modelViewer->getBoundingMax();
		char buf[1024];
		
		boundingMin.print(buf);
		[minField setStringValue:[NSString stringWithFormat:@"<%@>", [NSString stringWithASCIICString:buf]]];
		boundingMax.print(buf);
		[maxField setStringValue:[NSString stringWithFormat:@"<%@>", [NSString stringWithASCIICString:buf]]];
		modelViewer->setShowBoundingBox(true);
	}
	else
	{
		[minField setStringValue:@""];
		[maxField setStringValue:@""];
	}
}

- (BOOL)isVisible
{
	return [panel isVisible];
}

- (IBAction)show:(id)sender
{
	[panel makeKeyAndOrderFront:sender];
}

- (void)windowWillClose:(NSNotification *)aNotification
{
	[self setModelWindow:nil];
}

@end
