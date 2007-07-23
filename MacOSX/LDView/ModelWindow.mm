#import "ModelWindow.h"
#import "LDrawModelView.h"
#import "LDViewController.h"
#import "Preferences.h"
#import "OCLocalStrings.h"
#include <LDLoader/LDLError.h>
#include <LDLib/LDPreferences.h>
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

- (id)initWithController:(LDViewController *)value
{
	controller = value;
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
	if ([NSOpenGLContext currentContext] != [modelView openGLContext])
	{
		// This alert is coming from a different model viewer.
		return;
	}
	static NSDate *lastProgressUpdate = NULL;
	float alertProgress = alert->getProgress();
	NSString *alertMessage = [NSString stringWithCString:alert->getMessage()
		encoding:NSASCIIStringEncoding];
	BOOL forceUpdate = NO;
	BOOL updated = NO;

	if (![alertMessage isEqualToString:[progressMessage stringValue]])
	{
		[progressMessage setStringValue:alertMessage];
		forceUpdate = YES;
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
			[statusBox display];
			updated = YES;
			[lastProgressUpdate release];
			lastProgressUpdate = [[NSDate alloc] init];
		}
	}
	else if (alertProgress == 2.0f)
	{
		[progress setDoubleValue:1.0];
	}
	if (forceUpdate && !updated)
	{
		[statusBox display];
	}
}

- (void)reloadNeeded
{
	[window makeKeyWindow];
	[modelView reloadNeeded];
}

- (void)modelWillReload
{
	[self performSelectorOnMainThread:@selector(reloadNeeded) withObject:nil waitUntilDone:NO];
}

- (void)windowWillClose:(NSNotification *)aNotification
{
	if ([aNotification object] == window)
	{
		[self autorelease];
	}
}

- (void)showFps
{
	if ([[controller preferences] ldPreferences]->getShowFps())
	{
		if (fps > 0.0)
		{
			[progressMessage setStringValue:[NSString stringWithFormat:[OCLocalStrings get:@"FPSFormat"], fps]];
		}
		else
		{
			[progressMessage setStringValue:[OCLocalStrings get:@"FPSSpinPrompt"]];
		}
	}
	else
	{
		[progressMessage setStringValue:@""];
	}
}

- (void)updateFps
{
	if (fpsReferenceDate)
	{
		NSTimeInterval interval = -[fpsReferenceDate timeIntervalSinceNow];
		
		if (interval >= 0.25)
		{
			[fpsReferenceDate release];
			fpsReferenceDate = [[NSDate alloc] init];
			fps = 1.0 / interval * fpsFrameCount;
			fpsFrameCount = 1;
		}
		else
		{
			fpsFrameCount++;
		}
	}
	else
	{
		fpsReferenceDate = [[NSDate alloc] init];
	}
	[self showFps];
}

- (void)clearFps
{
	[fpsReferenceDate release];
	fpsReferenceDate = nil;
	fps = 0.0f;
	fpsFrameCount = 1;
	[self showFps];
}

@end
