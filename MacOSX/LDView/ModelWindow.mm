#import "ModelWindow.h"
#import "LDrawModelView.h"
#include <LDLoader/LDLError.h>
#include <TCFoundation/TCProgressAlert.h>
#import "AlertHandler.h"

@implementation ModelWindow

- (void)dealloc
{
	[window release];
	TCObject::release(alertHandler);
	alertHandler = NULL;
	[super dealloc];
}

- (NSToolbarItem *) toolbar:(NSToolbar *)toolbar
      itemForItemIdentifier:(NSString *)itemIdentifier
  willBeInsertedIntoToolbar:(BOOL)flag
{
	return nil;
//	NSToolbarItem *toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:
//		itemIdentifier] autorelease];
//	return toolbarItem;
}

- (NSArray *)toolbarAllowedItemIdentifiers: (NSToolbar *)toolbar
{
	return [NSArray arrayWithObjects:
		NSToolbarPrintItemIdentifier,
		NSToolbarCustomizeToolbarItemIdentifier,
		NSToolbarFlexibleSpaceItemIdentifier,
		NSToolbarSpaceItemIdentifier,
		NSToolbarSeparatorItemIdentifier,
		nil];
}

- (NSArray *)toolbarDefaultItemIdentifiers: (NSToolbar *)toolbar
{
	return [NSArray arrayWithObjects:
//		NSToolbarPrintItemIdentifier,
		NSToolbarCustomizeToolbarItemIdentifier,
		nil];
}

- (void)setupToolbar
{
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"LDViewToolbar"];
	[toolbar setDelegate:self];
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setAutosavesConfiguration:YES];
}

- (void)awakeFromNib
{
	[self setupToolbar];
	[window setToolbar:toolbar];
}

- (id)init
{
	[NSBundle loadNibNamed:@"ModelWindow.nib" owner:self];
	alertHandler = new AlertHandler(self);
	return [super init];
}

- (BOOL)openModel:(NSString *)filename
{
	[window setTitleWithRepresentedFilename:filename];
	[window makeKeyAndOrderFront:self];
	return [modelView openModel:filename];
}

- (LDrawModelView *)modelView
{
	return modelView;
}

- (void)ldlErrorCallback:(LDLError *)error
{
}

- (void)progressAlertCallback:(TCProgressAlert *)alert
{
	static NSDate *lastProgressUpdate = NULL;
	float alertProgress = alert->getProgress();
	NSString *alertMessage = [NSString stringWithCString:alert->getMessage()
		encoding:NSASCIIStringEncoding];

	if (![alertMessage isEqualToString:[progressMessage stringValue]])
	{
		[progressMessage setStringValue:alertMessage];
	}
	if (alertProgress >= 0.0f && alertProgress <= 1.0f)
	{
		// Don't update the progress more than 5 times per second, or loading
		// the model can take a REALLY long time.
		if (alertProgress == 1.0f || lastProgressUpdate == NULL ||
			[lastProgressUpdate timeIntervalSinceNow] < -0.2)
		{
			[window makeFirstResponder:progress];
			[progress setDoubleValue:alertProgress];
			[window display];
			[lastProgressUpdate release];
			lastProgressUpdate = [[NSDate alloc] init];
		}
		else
		{
			//printf("Undisplayed progress: %f\n", alertProgress);
		}
	}
}

- (void)windowWillClose:(NSNotification *)aNotification
{
	if ([aNotification object] == window)
	{
		[self autorelease];
	}
}

@end
