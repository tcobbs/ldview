#import "ModelWindow.h"
#import "LDrawModelView.h"
#import "LDViewController.h"
#import "Preferences.h"
#import "ToolbarSegmentedControl.h"
#import "ToolbarPopUpButton.h"
#import "OCLocalStrings.h"
#include <LDLoader/LDLError.h>
#include <LDLib/LDPreferences.h>
#include <TCFoundation/TCProgressAlert.h>
#import "AlertHandler.h"

@implementation ModelWindow

- (void)dealloc
{
	[statusBar release];
	[window release];
	[toolbarItems release];
	[toolbarItemIdentifiers release];
	TCObject::release(alertHandler);
	alertHandler = NULL;
	[super dealloc];
}

- (NSToolbarItem *) toolbar:(NSToolbar *)toolbar
      itemForItemIdentifier:(NSString *)itemIdentifier
  willBeInsertedIntoToolbar:(BOOL)flag
{
	return [toolbarItems objectForKey:itemIdentifier];
}

- (NSArray *)toolbarItemIdentifiers
{
	return [[toolbarItems allKeys] sortedArrayUsingSelector:@selector(compare:)];
}

- (NSArray *)toolbarAllowedItemIdentifiers: (NSToolbar *)toolbar
{
	return [toolbarItemIdentifiers arrayByAddingObjectsFromArray:[NSArray arrayWithObjects:
		NSToolbarFlexibleSpaceItemIdentifier,
		NSToolbarSpaceItemIdentifier,
		NSToolbarSeparatorItemIdentifier,
		nil]];
}

- (NSArray *)toolbarDefaultItemIdentifiers: (NSToolbar *)toolbar
{
	return toolbarItemIdentifiers;
}

- (NSToolbarItem *)addToolbarItemWithIdentifier:(NSString *)identifier label:(NSString *)label control:(NSControl *)control highPriority:(BOOL)highPriority
{
	NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:identifier];
	NSSize size = [control frame].size;

	size.height += 1.0f;
	[item setLabel:label];
	[item setPaletteLabel:label];
	[item setToolTip:label];
	[item setTarget:self];
	[item setMinSize:size];
	[item setMaxSize:size];
	[control retain];
	[control removeFromSuperview];
	[item setView:control];
	[control release];
	if (highPriority)
	{
		[item setVisibilityPriority:NSToolbarItemVisibilityPriorityHigh];
	}
	else
	{
		[item setVisibilityPriority:NSToolbarItemVisibilityPriorityStandard];
	}
	[toolbarItems setObject:item forKey:identifier];
	[toolbarItemIdentifiers addObject:identifier];
	[item release];
	return item;
}

- (void)setupToolbarItems
{
	toolbarItems = [[NSMutableDictionary alloc] init];
	toolbarItemIdentifiers = [[NSMutableArray alloc] init];

	[[viewPopUp itemAtIndex:0] setImage:[NSImage imageNamed:@"toolbar_view"]];
	// ToDo: Localize
	NSLog(@"[actionsSegments cell]: %@\n", [actionsSegments cell]);
	actionsSegments = [[ToolbarSegmentedControl alloc] initWithTemplate:actionsSegments];
	featuresSegments = [[ToolbarSegmentedControl alloc] initWithTemplate:featuresSegments];
	viewPopUp = [[ToolbarPopUpButton alloc] initWithTemplate:viewPopUp];
	prefsSegments = [[ToolbarSegmentedControl alloc] initWithTemplate:prefsSegments];
	[[actionsSegments cell] setToolTip: @"Save Snapshot" forSegment:0];
	[[actionsSegments cell] setToolTip: @"Reload" forSegment:1];
	[[featuresSegments cell] setToolTip: @"Enable/Disable Wireframe" forSegment:0];
	[[featuresSegments cell] setToolTip: @"Enable/Disable Seams" forSegment:1];
	[[featuresSegments cell] setToolTip: @"Enable/Disable Edges" forSegment:2];
	[[featuresSegments cell] setToolTip: @"Enable/Disable Primitive Substitution" forSegment:3];
	[[featuresSegments cell] setToolTip: @"Enable/Disable Lighting" forSegment:4];
	[[featuresSegments cell] setToolTip: @"Enable/Disable BFC" forSegment:5];
	[self addToolbarItemWithIdentifier:@"Actions" label:@"Actions" control:actionsSegments highPriority:YES];
	[self addToolbarItemWithIdentifier:@"Features" label:@"Features" control:featuresSegments highPriority:NO];
	[self addToolbarItemWithIdentifier:@"View" label:@"Viewing Angle" control:viewPopUp highPriority:NO];
	[self addToolbarItemWithIdentifier:@"Prefs" label:@"Preferences" control:prefsSegments highPriority:YES];
}

- (void)setupToolbar
{
	[self setupToolbarItems];
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"LDViewToolbar"];
	[toolbar setDelegate:self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconOnly];
	[toolbar setSizeMode:NSToolbarSizeModeRegular];
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setAutosavesConfiguration:YES];
}

- (void)awakeFromNib
{
	[self setupToolbar];
	[window setToolbar:toolbar];
	[statusBar retain];
	progressAdjust = [progressMessage frame].origin.x - [progress frame].origin.x;
	[window setNextResponder:controller];
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

- (BOOL)showStatusBar:(BOOL)show
{
	BOOL changed = NO;
	
	if (show)
	{
		if (![statusBar superview])
		{
			NSRect modelViewFrame1 = [modelView frame];
			float height = [statusBar frame].size.height;
			
			modelViewFrame1.size.height -= height;
			modelViewFrame1.origin.y += height;
			[modelView setFrame:modelViewFrame1];
			[[window contentView] addSubview:statusBar];
			changed = YES;
		}
	}
	else
	{
		if ([statusBar superview])
		{
			NSRect modelViewFrame2 = [modelView frame];
			float height = [statusBar frame].size.height;
			
			modelViewFrame2.size.height += height;
			modelViewFrame2.origin.y -= height;
			[statusBar removeFromSuperview];
			[modelView setFrame:modelViewFrame2];
			changed = YES;
		}
	}
	return changed;
}

- (void)adjustProgressMessageSize:(float)amount
{
	NSRect frame = [progressMessage frame];
	frame.origin.x -= amount;
	frame.size.width += amount;
	[progressMessage setFrame:frame];
}

- (void)progressAlertCallback:(TCProgressAlert *)alert
{
	if ([NSOpenGLContext currentContext] != [modelView openGLContext])
	{
		// This alert is coming from a different model viewer.
		[self showStatusBar:showStatusBar];
		return;
	}
	static NSDate *lastProgressUpdate = NULL;
	float alertProgress = alert->getProgress();
	NSString *alertMessage = [NSString stringWithCString:alert->getMessage()
		encoding:NSASCIIStringEncoding];
	BOOL forceUpdate = NO;
	BOOL updated = NO;

	if ([self showStatusBar:YES])
	{
		[window display];
	}
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
			if ([progress isHidden])
			{
				[progress setHidden:NO];
				[self adjustProgressMessageSize: -progressAdjust]; 
			}
			[progress setDoubleValue:alertProgress];
			[statusBar display];
			updated = YES;
			[lastProgressUpdate release];
			lastProgressUpdate = [[NSDate alloc] init];
		}
	}
	else if (alertProgress == 2.0f)
	{
		[progress setDoubleValue:1.0];
		if (![progress isHidden])
		{
			[progress setHidden:YES];
			[self adjustProgressMessageSize: progressAdjust]; 
		}
		[self showStatusBar:showStatusBar];
	}
	if (forceUpdate && !updated)
	{
		[statusBar display];
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
	[modelView setFps:0.0f];
	if ([[controller preferences] ldPreferences]->getShowFps() && [modelView modelViewer]->getMainTREModel())
	{
		if (showStatusBar)
		{
			if (fps > 0.0f)
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
			[modelView setFps:fps];
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

- (void)setShowStatusBar:(BOOL)value
{
	showStatusBar = value;
	[self showStatusBar:showStatusBar];
}

- (BOOL)showStatusBar
{
	return showStatusBar;
}

- (void)show
{
	[window makeKeyAndOrderFront:self];
}

- (NSWindow *)window
{
	return window;
}

- (NSToolbar *)toolbar
{
	return toolbar;
}

- (IBAction)saveSnapshot:(id)sender
{
	NSLog(@"saveSnapshot.\n");
}

- (IBAction)reload:(id)sender
{
	NSLog(@"reload.\n");
}

- (IBAction)actions:(id)sender
{
	switch ([[sender cell] tagForSegment:[sender selectedSegment]])
	{
		case 1:
			[self saveSnapshot:sender];
			break;
		case 2:
			[self reload:sender];
			break;
		default:
			NSLog(@"Unknown action.\n");
			break;
	}
}

- (IBAction)toggleWireframe:(id)sender
{
	NSLog(@"toggleWireframe.\n");
}

- (IBAction)toggleSeams:(id)sender
{
	NSLog(@"toggleSeams.\n");
}

- (IBAction)toggleEdges:(id)sender
{
	NSLog(@"toggleEdges.\n");
}

- (IBAction)togglePrimSub:(id)sender
{
	NSLog(@"togglePrimSub.\n");
}

- (IBAction)toggleLighting:(id)sender
{
	NSLog(@"toggleLighting.\n");
}

- (IBAction)toggleBfc:(id)sender
{
	NSLog(@"toggleBfc.\n");
}


- (IBAction)features:(id)sender
{
	switch([[sender cell] tagForSegment:[sender selectedSegment]])
	{
		case 0:
			[self toggleWireframe:sender];
			break;
		case 1:
			[self toggleSeams:sender];
			break;
		case 2:
			[self toggleEdges:sender];
			break;
		case 3:
			[self togglePrimSub:sender];
			break;
		case 4:
			[self toggleLighting:sender];
			break;
		case 5:
			[self toggleBfc:sender];
			break;
		default:
			NSLog(@"Unknown feature.\n");
			break;
	}
}

- (IBAction)viewingAngle:(id)sender
{
	NSLog(@"viewingAngle toolbar button : %d.\n", [sender tag]);
}

- (IBAction)saveViewingAngle:(id)sender
{
	NSLog(@"saveViewingAngle toolbar button.\n");
}

- (IBAction)preferences:(id)sender
{
	NSLog(@"preferences toolbar button.\n");
}

@end
