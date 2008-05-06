#import "ModelWindow.h"
#import "LDrawModelView.h"
#import "LDViewController.h"
#import "Preferences.h"
#import "ToolbarSegmentedControl.h"
//#import "ToolbarPopUpButton.h"
#import "ErrorsAndWarnings.h"
#import "BoundingBox.h"
#import "ErrorItem.h"
#import "OCLocalStrings.h"
#import "OCUserDefaults.h"
#import "SnapshotTaker.h"
#import "SaveSnapshotViewOwner.h"
#import "PartsList.h"
#import "ModelTree.h"
#import "AlertHandler.h"
#import "LDViewCategories.h"
#import "OpenGLDriverInfo.h"
#import "GeneralPage.h"
#import "GoToStep.h"
#import "LatLon.h"

#include <LDLoader/LDLError.h>
#include <LDLoader/LDLMainModel.h>
#include <LDLib/LDPreferences.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <LDLib/LDInputHandler.h>
#include <LDLib/LDPartsList.h>
#include <LDLib/LDHtmlInventory.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCStringArray.h>

#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4

enum
{
	NSSegmentStyleAutomatic = 0,
	NSSegmentStyleRounded = 1,
	NSSegmentStyleTexturedRounded = 2,
	NSSegmentStyleRoundRect = 3,
	NSSegmentStyleTexturedSquare = 4,
	NSSegmentStyleCapsule = 5,
	NSSegmentStyleSmallSquare = 6
};

@interface NSSegmentedControl (SCLeopardOnly)

- (void)setSegmentStyle:(int)style;

@end

#endif

@implementation ModelWindow

- (void)killPolling
{
	[pollingTimer invalidate];
	[pollingTimer release];
	pollingTimer = nil;
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[statusBar release];
	[window release];
	[toolbarItems release];
	[defaultIdentifiers release];
	[otherIdentifiers release];
	TCObject::release(alertHandler);
	alertHandler = NULL;
	[imageFileTypes release];
	[snapshotTaker release];
	[saveSnapshotViewOwner release];
	[modelTree release];
	[initialTitle release];
	[stepToolbarControls release];
	[self killPolling];
	[super dealloc];
}

- (BOOL)showStatusLatLon:(BOOL)show
{
	BOOL changed = NO;
	
	if (show)
	{
		if ([latLonBox isHidden])
		{
			NSRect frame = [progressMessage frame];
			
			[latLonBox setHidden:NO];
			frame.size.width -= latLonDelta;
			[progressMessage setFrame:frame];
			changed = YES;
		}
	}
	else
	{
		if (![latLonBox isHidden])
		{
			NSRect messageFrame = [progressMessage frame];
			NSRect latLonFrame = [latLonBox frame];
			
			latLonDelta = latLonFrame.origin.x + latLonFrame.size.width - messageFrame.origin.x - messageFrame.size.width;
			[latLonBox setHidden:YES];
			messageFrame.size.width += latLonDelta;
			[progressMessage setFrame:messageFrame];
			changed = YES;
		}
	}
	return changed;
}

- (NSToolbarItem *) toolbar:(NSToolbar *)toolbar
      itemForItemIdentifier:(NSString *)itemIdentifier
  willBeInsertedIntoToolbar:(BOOL)flag
{
	return [toolbarItems objectForKey:itemIdentifier];
}

- (NSArray *)toolbarAllowedItemIdentifiers: (NSToolbar *)toolbar
{
	return allIdentifiers;
}

- (NSArray *)toolbarDefaultItemIdentifiers: (NSToolbar *)toolbar
{
	return defaultIdentifiers;
}

- (void)ignoreAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
}

- (void)runAlertSheetWithMessageText:(NSString *)messageTitle defaultButton:(NSString *)defaultButtonTitle alternateButton:(NSString *)alternateButtonTitle otherButton:(NSString *)otherButtonTitle informativeText:(NSString *)informativeText
{
	NSAlert *alert = [NSAlert alertWithMessageText:messageTitle defaultButton:defaultButtonTitle alternateButton:alternateButtonTitle otherButton:otherButtonTitle informativeTextWithFormat:informativeText];
	
	[alert beginSheetModalForWindow:window modalDelegate:self didEndSelector:@selector(ignoreAlertDidEnd:returnCode:contextInfo:) contextInfo:NULL];
}

- (NSToolbarItem *)addToolbarItemWithIdentifier:(NSString *)identifier label:(NSString *)label control:(NSControl **)pControl menuItem:(NSMenuItem *)menuItem highPriority:(BOOL)highPriority isDefault:(BOOL)isDefault
{
	NSControl *&control = *pControl;
	NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:identifier];
	NSSize size;

	if (replaceSegments && [control isKindOfClass:[NSSegmentedControl class]] && ![control isKindOfClass:[ToolbarSegmentedControl class]])
	{
		*pControl = [[ToolbarSegmentedControl alloc] initWithTemplate:*pControl];
	}
	if ([control isKindOfClass:[NSSegmentedControl class]] && [control respondsToSelector:@selector(setSegmentStyle:)])
	{
		[(NSSegmentedControl *)control setSegmentStyle:NSSegmentStyleCapsule];
	}
	size = [control frame].size;
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
	if (isDefault)
	{
		[defaultIdentifiers addObject:identifier];
	}
	else
	{
		[otherIdentifiers addObject:identifier];
	}
	[allIdentifiers addObject:identifier];
	if (!menuItem && [control isKindOfClass:[NSSegmentedControl class]] && [(NSSegmentedControl *)control segmentCount] == 1)
	{
		menuItem = [[[NSMenuItem alloc] initWithTitle:label action:[control action] keyEquivalent:@""] autorelease];
		[menuItem setTag:[[(NSSegmentedControl *)control cell] tagForSegment:0]];
	}
	if (menuItem)
	{
		[menuItem setKeyEquivalent:@""];
		[item setMenuFormRepresentation:menuItem];
	}
	[item release];
	return item;
}

- (NSToolbarItem *)addToolbarItemWithIdentifier:(NSString *)identifier label:(NSString *)label control:(NSControl **)pControl highPriority:(BOOL)highPriority isDefault:(BOOL)isDefault
{
	return [self addToolbarItemWithIdentifier:identifier label:label control:pControl menuItem:nil highPriority:highPriority isDefault:isDefault];
}

- (void)updateSegments:(NSSegmentedControl *)segments states:(NSArray *)states
{
	int i;
	int count = [states count];
	NSSegmentedCell *cell = [segments cell];
	
	for (i = 0; i < count; i++)
	{
		[cell setSelected:[[states objectAtIndex:i] boolValue] forSegment:i];
	}
}

- (void)setupSegments:(NSSegmentedControl *)segments toolTips:(NSArray *)toolTips
{
	int i;
	int count = [toolTips count];
	NSSegmentedCell *cell = [segments cell];

	for (i = 0; i < count; i++)
	{
		[cell setToolTip:[toolTips objectAtIndex:i] forSegment:i];
	}
}

- (void)updateFeatureStates
{
	LDPreferences *ldPreferences = [[controller preferences] ldPreferences];
	NSArray *states = [NSArray arrayWithObjects:
		[NSNumber numberWithBool:ldPreferences->getDrawWireframe()],
		[NSNumber numberWithBool:ldPreferences->getUseSeams()],
		[NSNumber numberWithBool:ldPreferences->getShowHighlightLines()],
		[NSNumber numberWithBool:ldPreferences->getAllowPrimitiveSubstitution()],
		[NSNumber numberWithBool:ldPreferences->getUseLighting()],
		[NSNumber numberWithBool:ldPreferences->getBfc()],
		[NSNumber numberWithBool:ldPreferences->getShowAxes()],
		nil];

	[self updateSegments:featuresSegments states:states];
}

- (void)preferencesDidUpdate:(NSNotification *)notification
{
	[self updateFeatureStates];
}

- (void)setupFeatures
{
	NSArray *toolTips = [NSArray arrayWithObjects:
		@"Enable/Disable Wireframe",
		@"Enable/Disable Seams",
		@"Enable/Disable Edges",
		@"Enable/Disable Primitive Substitution",
		@"Enable/Disable Lighting",
		@"Enable/Disable BFC",
		@"Show/Hide Axes",
		nil];
//	if (replaceSegments)
//	{
//		featuresSegments = [[ToolbarSegmentedControl alloc] initWithTemplate:featuresSegments];
//	}
	[self setupSegments:featuresSegments toolTips:toolTips];
	[self updateFeatureStates];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(preferencesDidUpdate:) name:LDPreferencesDidUpdateNotification object:nil];
}

- (BOOL)haveLatLon
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer)
	{
		if (modelViewer->getViewMode() == LDrawModelViewer::VMExamine && modelViewer->getExamineMode() == LDrawModelViewer::EMLatLong)
		{
			return YES;
		}
	}
	return NO;
}

- (void)updateStatusLatLon
{
	if ([self haveLatLon])
	{
		LDrawModelViewer *modelViewer = [modelView modelViewer];

		if (modelViewer != NULL)
		{
			int lon = (int)modelViewer->getExamineLongitude();

			[latField setIntValue:(int)modelViewer->getExamineLatitude()];
			if (lon < -179)
			{
				lon += 360;
			}
			if (lon > 180)
			{
				lon -= 360;
			}
			[lonField setIntValue:lon];
			[latLonBox display];
		}
	}
}

- (void)setExamineLatLong:(bool)value
{
	LDrawModelViewer::ExamineMode examineMode = LDrawModelViewer::EMFree;
	
	if (value)
	{
		examineMode = LDrawModelViewer::EMLatLong;
	}
	[modelView modelViewer]->setExamineMode(examineMode);
	TCUserDefaults::setLongForKey(examineMode, EXAMINE_MODE_KEY, false);
	[self showStatusLatLon:[self haveLatLon]];
}

- (void)setFlyThroughMode:(bool)value
{
	[modelView setFlyThroughMode:value];
	if (value)
	{
		[viewModeSegments selectSegmentWithTag:LDInputHandler::VMFlyThrough];
	}
	else
	{
		[viewModeSegments selectSegmentWithTag:LDInputHandler::VMExamine];
	}
	[self showStatusLatLon:[self haveLatLon]];
}

- (void)setupViewMode
{
	NSArray *toolTips = [NSArray arrayWithObjects:
		@"Examine Mode",
		@"Fly-through Mode",
		nil];
	
//	if (replaceSegments)
//	{
//		viewModeSegments = [[ToolbarSegmentedControl alloc] initWithTemplate:viewModeSegments];
//	}
	[self setupSegments:viewModeSegments toolTips:toolTips];
	[self setFlyThroughMode:TCUserDefaults::longForKey(VIEW_MODE_KEY, LDInputHandler::VMExamine, false) == LDInputHandler::VMFlyThrough];
	examineLatLong = TCUserDefaults::longForKey(EXAMINE_MODE_KEY, LDrawModelViewer::EMFree, false) == LDrawModelViewer::EMLatLong;
	[self setExamineLatLong:examineLatLong];
}

- (void)setupToolbarItems
{
	toolbarItems = [[NSMutableDictionary alloc] init];
	defaultIdentifiers = [[NSMutableArray alloc] init];
	otherIdentifiers = [[NSMutableArray alloc] init];
	allIdentifiers = [[NSMutableArray alloc] init];

	// ToDo: Localize
	// Most of these are set to high priority.  That's because they become
	// useless when stuck in the menu.  Note that NONE of them work in the menu
	// in Tiger.  I'm hoping that they work in the menu in Leopard when the
	// segmented control only has one item.
	[self addToolbarItemWithIdentifier:@"Actions" label:@"Actions" control:&actionsSegments highPriority:YES isDefault:YES];
	[self addToolbarItemWithIdentifier:@"Features" label:@"Features" control:&featuresSegments highPriority:YES isDefault:YES];
	[self addToolbarItemWithIdentifier:@"View" label:[OCLocalStrings get:@"SelectView"] control:&viewingAngleSegments highPriority:YES isDefault:NO];
	[self addToolbarItemWithIdentifier:@"ViewMode" label:@"View Mode" control:&viewModeSegments highPriority:YES isDefault:YES];
	[self addToolbarItemWithIdentifier:@"OpenFile" label:[OCLocalStrings get:@"OpenFile"] control:&openButton highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"SaveSnapshot" label:[OCLocalStrings get:@"SaveSnapshot"] control:&snapshotButton highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"Reload" label:[OCLocalStrings get:@"Reload"] control:&reloadButton highPriority:NO isDefault:NO];
	[printSegments setTarget:controller];
	[self addToolbarItemWithIdentifier:@"StepFirst" label:[OCLocalStrings get:@"First"] control:&stepFirstSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"StepPrev" label:[OCLocalStrings get:@"Previous"] control:&stepPrevSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"StepPrev2" label:[OCLocalStrings get:@"Previous"] control:&stepPrevSegments2 highPriority:YES isDefault:NO];
	[self addToolbarItemWithIdentifier:@"StepField" label:[OCLocalStrings get:@"Step"] control:&stepField highPriority:NO isDefault:YES];
	[self addToolbarItemWithIdentifier:@"StepNext2" label:[OCLocalStrings get:@"Next"] control:&stepNextSegments2 highPriority:YES isDefault:NO];
	[self addToolbarItemWithIdentifier:@"StepNext" label:[OCLocalStrings get:@"Next"] control:&stepNextSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"StepLast" label:[OCLocalStrings get:@"Last"] control:&stepLastSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"Step" label:[OCLocalStrings get:@"PrevNext"] control:&stepSegments highPriority:YES isDefault:YES];
	[self addToolbarItemWithIdentifier:@"Step2" label:[OCLocalStrings get:@"PrevNext"] control:&stepSegments2 highPriority:YES isDefault:NO];
	[defaultIdentifiers addObject:NSToolbarFlexibleSpaceItemIdentifier];	
	[self addToolbarItemWithIdentifier:@"Prefs" label:[OCLocalStrings get:@"Preferences"] control:&prefsSegments menuItem:[[[controller prefsMenuItem] copy] autorelease] highPriority:NO isDefault:YES];
	[self addToolbarItemWithIdentifier:@"Print" label:[OCLocalStrings get:@"Print"] control:&printSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"Customize" label:[OCLocalStrings get:@"Customize"] control:&customizeSegments highPriority:NO isDefault:NO];
	[[actionsSegments cell] setToolTip: [OCLocalStrings get:@"OpenFile"] forSegment:0];
	[[actionsSegments cell] setToolTip: [OCLocalStrings get:@"SaveSnapshot"] forSegment:1];
	[[actionsSegments cell] setToolTip: [OCLocalStrings get:@"Reload"] forSegment:2];
	stepToolbarControls = [[NSArray alloc] initWithObjects:stepSegments, stepSegments2, stepPrevSegments, stepPrevSegments2, stepNextSegments, stepNextSegments2, stepFirstSegments, stepLastSegments, nil];
	[allIdentifiers addObjectsFromArray:[NSArray arrayWithObjects:
		NSToolbarFlexibleSpaceItemIdentifier,
		NSToolbarSpaceItemIdentifier,
		NSToolbarSeparatorItemIdentifier,
		nil]];
	[viewingAngleSegments setMenu:[[[controller viewingAngleMenu] copy] autorelease] forSegment:0];
	[self setupFeatures];
	[self setupViewMode];
	//[defaultIdentifiers addObject:NSToolbarCustomizeToolbarItemIdentifier];
}

- (void)setupToolbar
{
	[self setupToolbarItems];
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"LDViewToolbar"];
	[toolbar setDelegate:self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconOnly];
	[toolbar setSizeMode:NSToolbarSizeModeSmall];
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setAutosavesConfiguration:YES];
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

- (void)awakeFromNib
{
	replaceSegments = true;
	[stepsMenu release];
	initialTitle = [[window title] retain];
	showStatusBar = [OCUserDefaults longForKey:@"StatusBar" defaultValue:1 sessionSpecific:NO];
	[self showStatusBar:showStatusBar];
	[self showStatusLatLon:[self haveLatLon]];
	[self setupToolbar];
	[window setToolbar:toolbar];
	[statusBar retain];
	progressAdjust = [progressMessage frame].origin.x - [progress frame].origin.x;
	[window setNextResponder:controller];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(errorFilterChange:) name:LDErrorFilterChange object:nil];
	imageFileTypes = [[NSArray alloc] initWithObjects:@"png", @"bmp", nil];
}

- (void)stepChanged
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	int newStep = 0;
	int i, j;
	
	if (modelViewer && modelViewer->getFilename() && (newStep = modelViewer->getStep()) > 0)
	{
		[stepField setStringValue:[NSString stringWithFormat:@"%d", newStep]];
	}
	else
	{
		[stepField setStringValue:@"--"];
	}
	for (i = 0; i < [stepToolbarControls count]; i++)
	{
		NSSegmentedControl *control = [stepToolbarControls objectAtIndex:i];
		NSSegmentedCell *cell = [control cell];
		
		for (j = 0; j < [cell segmentCount]; j++)
		{
			BOOL enabled = NO;
		
			switch ([cell tagForSegment:j])
			{
				case -1:
				case 0:
					enabled = newStep > 1;
					break;
				case 1:
				case 2:
					enabled = newStep < modelViewer->getNumSteps() && newStep > 0;
					break;
			}
			[control setEnabled:enabled forSegment:j];
		}
	}
}

- (void)changeStep:(int)action
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer)
	{
		int newStep;
		
		if (action == 0)
		{
			newStep = 1;
		}
		else if (action == 2)
		{
			newStep = modelViewer->getNumSteps();
		}
		else
		{
			newStep = modelViewer->getStep() + action;
		}
		if (newStep <  1)
		{
			newStep = 1;
		}
		else if (newStep > modelViewer->getNumSteps())
		{
			newStep = modelViewer->getNumSteps();
		}
		modelViewer->setStep(newStep);
		[self stepChanged];
		[modelView rotationUpdate];
	}
}

- (IBAction)takeStepFrom:(id)sender
{
	if ([sender isKindOfClass:[NSSegmentedControl class]])
	{
		[self changeStep:[[sender cell] tagForSegment:[sender selectedSegment]]];
	}
	else
	{
		[self changeStep:[sender tag]];
	}
}

- (void)setStep:(int)step
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];

	if (step > 0 && step <= modelViewer->getNumSteps())
	{
		modelViewer->setStep(step);
	}
	else
	{
		NSBeep();
	}
	[self stepChanged];
	[modelView rotationUpdate];
}

- (void)controlTextDidEndEditing:(NSNotification *)notification
{
	if ([notification object] == stepField)
	{
		[self setStep:[stepField intValue]];
	}
}

- (IBAction)goToStep:(id)sender
{
	GoToStep *sheet = [[GoToStep alloc] init];
	
	if ([sheet runSheetInWindow:window] == NSOKButton)
	{
		[self setStep:[sheet step]];
	}
	[sheet release];
}

- (id)initWithController:(LDViewController *)value
{
	if ((self = [super init]) != nil)
	{
		controller = value;
		[NSBundle loadNibNamed:@"ModelWindow.nib" owner:self];
		alertHandler = new AlertHandler(self);
	}
	return self;
}

- (void)enableToolbarItems:(BOOL)enabled
{
	NSEnumerator *enumerator = [allIdentifiers objectEnumerator];
	
	while (NSString *identifier = [enumerator nextObject])
	{
		NSToolbarItem *item = [toolbarItems objectForKey:identifier];
		NSControl *control = (NSControl *)[item view];

		if (control == actionsSegments)
		{
			[actionsSegments setEnabled:enabled forSegment:1];
			[actionsSegments setEnabled:enabled forSegment:2];
		}
		else if ([control isKindOfClass:[NSControl class]])
		{
			[control setEnabled:enabled];
		}
	}
}

- (ErrorItem *)filteredRootErrorItem
{
	return [[ErrorsAndWarnings sharedInstance] filteredRootErrorItem:unfilteredRootErrorItem];
//	if (!filteredRootErrorItem)
//	{
//		filteredRootErrorItem = [[[ErrorsAndWarnings sharedInstance] filteredRootErrorItem:unfilteredRootErrorItem] retain];
//	}
//	return filteredRootErrorItem;
}

- (BOOL)openModel:(NSString *)filename
{
	[unfilteredRootErrorItem release];
	unfilteredRootErrorItem = nil;
	[filteredRootErrorItem release];
	filteredRootErrorItem = nil;
	[window setTitleWithRepresentedFilename:filename];
	[window makeKeyAndOrderFront:self];
	if ([modelView openModel:filename])
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

- (bool)loading
{
	return loading;
}

- (bool)parsing
{
	return parsing;
}

- (bool)loadCanceled
{
	return loadCanceled;
}

- (LDrawModelView *)modelView
{
	return modelView;
}

- (bool)isMyAlert:(TCAlert *)alert
{
	TCAlertSender *sender = alert->getSender();

	return sender != NULL && (sender->getAlertSender() == [modelView modelViewer] || sender->getAlertSender() == [snapshotTaker ldSnapshotTaker]);
}

- (void)addErrorItem:(ErrorItem *)parent string:(NSString *)string error:(LDLError *)error
{
	[parent addChild:[[[ErrorItem alloc] initWithString:string error:error includeIcon:NO] autorelease]];
}

- (void)ldlErrorCallback:(LDLError *)error
{
	if (![self isMyAlert:error] || !loading)
	{
		return;
	}
	TCStringArray *extraInfo = error->getExtraInfo();
	NSString *lineString;

	if (!unfilteredRootErrorItem)
	{
		unfilteredRootErrorItem = [[ErrorItem alloc] init];
	}
	ErrorItem *errorItem = [unfilteredRootErrorItem addChild:[[[ErrorItem alloc] initWithString:[NSString stringWithCString:error->getMessage() encoding:NSASCIIStringEncoding] error:error includeIcon:YES] autorelease]];
	if (error->getFilename())
	{
		lineString = [NSString stringWithFormat:@"%@%s", [OCLocalStrings get:@"ErrorTreeFilePrefix"], error->getFilename()];
	}
	else
	{
		lineString = [OCLocalStrings get:@"ErrorTreeUnknownFile"];
	}
	[self addErrorItem:errorItem string:lineString error:error];
	if (error->getFileLine())
	{
		int lineNumber = error->getLineNumber();
		
		if (lineNumber > 0)
		{
			lineString = [NSString stringWithFormat:[OCLocalStrings get:@"ErrorTreeLine#"], lineNumber];
		}
		else
		{
			lineString = [OCLocalStrings get:@"ErrorTreeUnknownLine#"];
		}
	}
	else
	{
		lineString = [OCLocalStrings get:@"ErrorTreeUnknownLine"];
	}
	[self addErrorItem:errorItem string:lineString error:error];
	if (extraInfo)
	{
		for (int i = 0; i < extraInfo->getCount(); i++)
		{
			[self addErrorItem:errorItem string:[NSString stringWithCString:extraInfo->stringAtIndex(i) encoding:NSASCIIStringEncoding] error:error];
		}
	}
}

- (void)adjustProgressMessageSize:(float)amount
{
	NSRect frame = [progressMessage frame];
	frame.origin.x -= amount;
	frame.size.width += amount;
	[progressMessage setFrame:frame];
}

- (void)cancelLoad:(id)sender
{
	loadCanceled = true;
}

- (void)progressAlertCallback:(TCProgressAlert *)alert
{
	if (![self isMyAlert:alert])
	{
		return;
	}
	static NSDate *lastProgressUpdate = NULL;
	float alertProgress = alert->getProgress();
	NSString *alertMessage = [NSString stringWithCString:alert->getMessage()
		encoding:NSASCIIStringEncoding];
	BOOL forceUpdate = NO;
	BOOL updated = NO;

	if (![alertMessage isEqualToString:[progressMessage stringValue]] && (!forceProgress || [alertMessage length] > 0))
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
			NSEvent *event;

			if ([progress isHidden])
			{
				[progress setHidden:NO];
				[self adjustProgressMessageSize: -progressAdjust]; 
			}
			[window makeFirstResponder:progress];
			[progress setDoubleValue:alertProgress];
			[statusBar display];
			updated = YES;
			[lastProgressUpdate release];
			lastProgressUpdate = [[NSDate alloc] init];
			// Flush the main event loop.
			while ((event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES]) != NULL)
			{
				bool skip = false;
				
				if ([event type] == NSKeyDown || [event type] == NSKeyUp)
				{
					skip = true;
					if ([[event characters] characterAtIndex:0] == 27)
					{
						[[controller currentModelWindow] cancelLoad:self];
					}
				}
				else if ([event window] == window)
				{
					skip = true;
				}
				if (!skip)
				{
					[NSApp sendEvent:event];
				}
			}
		}
	}
	else if (alertProgress == 2.0f)
	{
		[progress setDoubleValue:1.0];
		if (![progress isHidden] && !forceProgress)
		{
			[progress setHidden:YES];
			[self adjustProgressMessageSize: progressAdjust]; 
		}
		//[self showStatusBar:showStatusBar];
	}
	if (forceUpdate && !updated)
	{
		[statusBar display];
	}
	if (loadCanceled)
	{
		alert->abort();
	}
}

- (void)modelViewerAlertCallback:(TCAlert *)alert
{
	[modelView modelViewerAlertCallback:alert];
}

- (void)setLastWriteTime:(NSDate *)value
{
	if (value != lastWriteTime)
	{
		[lastWriteTime release];
		lastWriteTime = [value retain];
	}
}

- (void)pollingUpdate
{
	[self reload:self];
}

- (void)pollingAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSAlertDefaultReturn)
	{
		[[alert window] orderOut:self];
		[self pollingUpdate];
	}
}

- (void)askForPollingUpdate
{
	NSAlert *alert = [NSAlert alertWithMessageText:[OCLocalStrings get:@"PollFileUpdate"] defaultButton:[OCLocalStrings get:@"Yes"] alternateButton:[OCLocalStrings get:@"No"] otherButton:nil informativeTextWithFormat:[OCLocalStrings get:@"PollReloadCheck"]];
	[alert beginSheetModalForWindow:window modalDelegate:self didEndSelector:@selector(pollingAlertDidEnd:returnCode:contextInfo:) contextInfo:NULL];
}

- (void)pollingTimerFired:(NSTimer*)theTimer
{
	NSString *filename = [self filename];
	
	if (filename)
	{
		NSDate *thisWriteTime = [[[NSFileManager defaultManager] fileAttributesAtPath:[self filename] traverseLink:YES] objectForKey:NSFileModificationDate];
		
		if (![lastWriteTime isEqualToDate:thisWriteTime])
		{
			[self setLastWriteTime:thisWriteTime];
			switch ([controller pollingMode])
			{
				case 1:
					pollingUpdateNeeded = true;
					break;
				case 2:
					if ([controller currentModelWindow] == self)
					{
						[self pollingUpdate];
					}
					else
					{
						pollingUpdateNeeded = true;
					}
					break;
				case 3:
					[self pollingUpdate];
					break;
				default:
					break;
			}
		}
	}
}

- (NSString *)filename
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer)
	{
		return [NSString stringWithASCIICString:modelViewer->getFilename()];
	}
	else
	{
		return nil;
	}
}

- (void)updatePolling
{
	NSString *filename = [self filename];
	long pollingMode = [controller pollingMode];

	if (pollingMode && filename != nil && [modelView modelViewer]->getMainModel() != NULL)
	{
		if (!pollingTimer)
		{
			pollingTimer = [[NSTimer scheduledTimerWithTimeInterval:0.5f target:self selector:@selector(pollingTimerFired:) userInfo:nil repeats:YES] retain];
		}
	}
	else if (pollingTimer)
	{
		[self killPolling];
	}
}

- (void)updateUtilityWindows:(id)sender andShowErrorsIfNeeded:(BOOL)andShowErrorsIfNeeded
{
	if ([ErrorsAndWarnings sharedInstanceIsVisible] || andShowErrorsIfNeeded)
	{
		[[ErrorsAndWarnings sharedInstance] update:self];
	}
	if (andShowErrorsIfNeeded)
	{
		[[ErrorsAndWarnings sharedInstance] showIfNeeded];
	}
	if ([BoundingBox sharedInstanceIsVisible])
	{
		[[BoundingBox sharedInstance] update:sender];
	}
}

- (void)updateUtilityWindows:(id)sender
{
	[self updateUtilityWindows:self andShowErrorsIfNeeded:NO];
}

- (void)postLoad
{
	if ([self showStatusBar:showStatusBar] || [self showStatusLatLon:[self haveLatLon]])
	{
		[window display];
	}
	[modelView rotationUpdate];
	[self stepChanged];
	[self updateFps];
	[self updatePolling];
}

- (void)loadAlertCallback:(TCAlert *)alert
{
	LDrawModelViewer *modelViewer = (LDrawModelViewer *)alert->getSender();
	
	if (modelViewer == [modelView modelViewer])
	{
		NSString *message = [NSString stringWithASCIICString:alert->getMessage()];

		pollingUpdateNeeded = false;
		if ([message isEqualToString:@"ModelLoading"])
		{
			[self killPolling];
			loading = true;
			loadCanceled = false;
			if ([self showStatusBar:YES] || [self showStatusLatLon:NO])
			{
				[window display];
			}
			[unfilteredRootErrorItem release];
			unfilteredRootErrorItem = nil;
		}
		else if ([message isEqualToString:@"ModelParsing"])
		{
			[self killPolling];
			parsing = true;
			if (!loading)
			{
				loadCanceled = false;
				if ([self showStatusBar:YES] || [self showStatusLatLon:NO])
				{
					[window display];
				}
			}
		}
		else if ([message isEqualToString:@"ModelParsed"] || [message isEqualToString:@"ModelParseCanceled"])
		{
			if (!loading)
			{
				[self postLoad];
			}
			parsing = false;
		}
		else if ([message isEqualToString:@"ModelLoaded"])
		{
			bool showErrorsIfNeeded = [[[controller preferences] generalPage] showErrorsIfNeeded];
			NSString *filename = nil;

			[self enableToolbarItems:YES];
			loading = false;
			filename = [self filename];
			[OCUserDefaults setString:[filename stringByDeletingLastPathComponent] forKey:[NSString stringWithASCIICString:LAST_OPEN_PATH_KEY] sessionSpecific:NO];
			[self setLastWriteTime:[[[NSFileManager defaultManager] fileAttributesAtPath:filename traverseLink:YES] objectForKey:NSFileModificationDate]];
			[[self controller] recordRecentFile:filename];
			[self postLoad];
			[self updateUtilityWindows:self andShowErrorsIfNeeded:showErrorsIfNeeded];
		}
		else if ([message isEqualToString:@"ModelLoadCanceled"])
		{
			if ([self showStatusBar:showStatusBar] || [self showStatusLatLon:[self haveLatLon]])
			{
				[window display];
			}
			if (!loadCanceled)
			{
				[self runAlertSheetWithMessageText:[OCLocalStrings get:@"Error"] defaultButton:[OCLocalStrings get:@"OK"] alternateButton:nil otherButton:nil informativeText:[NSString stringWithFormat: [OCLocalStrings get:@"ErrorLoadingModel"], modelViewer->getFilename()]];
			}
			loading = false;
			[[self window] setTitleWithRepresentedFilename:@""];
			[[self window] setTitle:initialTitle];
			[self postLoad];
			[self updateUtilityWindows:self];
		}
		[[NSNotificationCenter defaultCenter] postNotificationName:message object:self];
	}
}

- (void)redrawAlertCallback:(TCAlert *)alert
{
	[modelView redrawAlertCallback:alert];
}

- (void)captureAlertCallback:(TCAlert *)alert
{
	[modelView captureAlertCallback:alert];
}

- (void)releaseAlertCallback:(TCAlert *)alert
{
	[modelView releaseAlertCallback:alert];
}

//- (void)peekMouseUpAlertCallback:(TCAlert *)alert
//{
//	[modelView peekMouseUpAlertCallback:alert];
//}

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
		[self updateUtilityWindows:nil];
		[window setDelegate:nil];
		[controller modelWindowWillClose:self];
	}
}

- (float)fps
{
	return fps;
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
	if (!showStatusBar)
	{
		[modelView rotationUpdate];
	}
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

- (IBAction)open:(id)sender
{
	[controller openModel:sender];
}

- (void)htmlSavePanelDidEnd:(NSSavePanel *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		if (htmlInventory->generateHtml([[sheet filename] cStringUsingEncoding:NSASCIIStringEncoding], partsList, [modelView modelViewer]->getFilename()))
		{
			if (htmlInventory->isSnapshotNeeded())
			{
				LDrawModelViewer *modelViewer = [modelView modelViewer];
				
				if (!snapshotTaker)
				{
					snapshotTaker = [[SnapshotTaker alloc] initWithModelViewer:[modelView modelViewer] sharedContext:[modelView openGLContext]];
				}
				htmlInventory->prepForSnapshot(modelViewer);
				[snapshotTaker saveFile:[NSString stringWithCString:htmlInventory->getSnapshotPath() encoding:NSASCIIStringEncoding] width:400 height:300 zoomToFit:YES];
				htmlInventory->restoreAfterSnapshot(modelViewer);
				[modelView rotationUpdate];
			}
			if (htmlInventory->getShowFileFlag())
			{
				[[NSWorkspace sharedWorkspace] openFile:[sheet filename]];
			}
		}
	}
	htmlInventory->release();
	partsList->release();
	sheetBusy = false;
}

- (NSString *)defaultSaveDirForOp:(LDPreferences::SaveOp)op
{
	return [[[controller preferences] generalPage] defaultSaveDirForOp:op modelFilename:[self filename]];
}

- (void)snapshotSavePanelDidEnd:(NSSavePanel *)sheet returnCode:(int)returnCode  contextInfo:(void  *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		NSSize viewSize = [modelView frame].size;
		int width = (int)viewSize.width;
		int height = (int)viewSize.height;		

		[saveSnapshotViewOwner saveSettings];
		[OCUserDefaults setString:[sheet filename] forKey:[NSString stringWithASCIICString:LAST_SNAPSHOT_DIR_KEY] sessionSpecific:NO];
		if (!snapshotTaker)
		{
			snapshotTaker = [[SnapshotTaker alloc] initWithModelViewer:[modelView modelViewer] sharedContext:[modelView openGLContext]];
		}
		[snapshotTaker setImageType:[saveSnapshotViewOwner imageType]];
		[snapshotTaker setTrySaveAlpha:[saveSnapshotViewOwner transparentBackground]];
		[snapshotTaker setAutoCrop:[saveSnapshotViewOwner autocrop]];
		[(NSSavePanel *)contextInfo orderOut:self];
		if ([self showStatusBar:YES])
		{
			[window display];
		}
		forceProgress = true;
		[snapshotTaker saveFile:[sheet filename] width:[saveSnapshotViewOwner width:width] height:[saveSnapshotViewOwner height:height] zoomToFit:[saveSnapshotViewOwner zoomToFit]];
		[saveSnapshotViewOwner saveSettings];
		forceProgress = false;
		if (![progress isHidden])
		{
			[progress setHidden:YES];
			[self adjustProgressMessageSize: progressAdjust];
		}
		[self updateFps];
		if ([self showStatusBar:showStatusBar])
		{
			[window display];
		}
	}
	[saveSnapshotViewOwner setSavePanel:nil];
	sheetBusy = false;
}

- (bool)sheetBusy
{
	return sheetBusy;
}

- (IBAction)saveSnapshot:(id)sender
{
	NSSavePanel *savePanel = [NSSavePanel savePanel];
	NSString *modelFilename = [self filename];
	NSString *defaultFilename = [[modelFilename lastPathComponent] stringByDeletingPathExtension];

	if (!saveSnapshotViewOwner)
	{
		saveSnapshotViewOwner = [[SaveSnapshotViewOwner alloc] init];
	}
	[saveSnapshotViewOwner setNumSteps:[modelView modelViewer]->getNumSteps()];
	[saveSnapshotViewOwner setSavePanel:savePanel];
	[savePanel setCanSelectHiddenExtension:YES];
	sheetBusy = true;
	[savePanel beginSheetForDirectory:[self defaultSaveDirForOp:LDPreferences::SOSnapshot] file:defaultFilename modalForWindow:window modalDelegate:self didEndSelector:@selector(snapshotSavePanelDidEnd:returnCode:contextInfo:) contextInfo:savePanel];
}

- (IBAction)reload:(id)sender
{
	[modelView reload];
}

- (IBAction)actions:(id)sender
{
	switch ([[sender cell] tagForSegment:[sender selectedSegment]])
	{
		case 0:
			[self open:sender];
			break;
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

- (void)toggleFeature:(SEL)selector sender:(id)sender
{
	// I know it looks wrong that were asking if the selectedSegment
	// is selected.  However, selectedSegment just returns the segment
	// the user just clicked on.  It's not necessarily actually selected.
	[[controller preferences] performSelector:selector withObject:sender];
}

- (IBAction)features:(id)sender
{
	switch ([[sender cell] tagForSegment:[sender selectedSegment]])
	{
		case 0:
			[self toggleFeature:@selector(takeWireframeFrom:) sender:sender];
			break;
		case 1:
			[self toggleFeature:@selector(takeSeamsFrom:) sender:sender];
			break;
		case 2:
			[self toggleFeature:@selector(takeEdgesFrom:) sender:sender];
			break;
		case 3:
			[self toggleFeature:@selector(takePrimSubFrom:) sender:sender];
			break;
		case 4:
			[self toggleFeature:@selector(takeLightingFrom:) sender:sender];
			break;
		case 5:
			[self toggleFeature:@selector(takeBfcFrom:) sender:sender];
			break;
		case 6:
			[self toggleFeature:@selector(takeShowAxesFrom:) sender:sender];
			break;
		default:
			NSLog(@"Unknown feature.\n");
			break;
	}
}

- (IBAction)resetView:(id)sender
{
	[modelView resetView:sender];
}

- (IBAction)viewingAngle:(id)sender
{
	[modelView setViewingAngle:[sender tag]];
}

- (IBAction)specifyLatLon:(id)sender
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer != NULL)
	{
		LatLon *sheet = [[LatLon alloc] init];
		
		if ([sheet runSheetInWindow:window] == NSOKButton)
		{
			modelViewer->setLatLon([sheet lat], [sheet lon]);
		}
		[sheet release];
	}
}

- (IBAction)saveViewingAngle:(id)sender
{
	[[controller preferences] saveViewingAngle:self];
}

- (IBAction)preferences:(id)sender
{
	[controller preferences:sender];
}

- (IBAction)alwaysOnTop:(id)sender
{
	if ([window level] == NSNormalWindowLevel)
	{
		[window setLevel:NSPopUpMenuWindowLevel];
	}
	else
	{
		[window setLevel:NSNormalWindowLevel];
	}
}

- (IBAction)toggleStatusBar:(id)sender
{
	[self setShowStatusBar:!showStatusBar];
	//[controller updateStatusBarMenuItem];
	[OCUserDefaults setLong:showStatusBar forKey:@"StatusBar" sessionSpecific:NO];
}

- (IBAction)customizeToolbar:(id)sender
{
	[toolbar runCustomizationPalette:sender];
}

- (IBAction)viewMode:(id)sender
{
	[modelView viewMode:sender];
}

- (IBAction)zoomToFit:(id)sender
{
	[modelView zoomToFit:sender];
}

- (IBAction)errorsAndWarnings:(id)sender
{
	[[ErrorsAndWarnings sharedInstance] update:self];
	[[ErrorsAndWarnings sharedInstance] show:self];
}

- (IBAction)boundingBox:(id)sender
{
	[[BoundingBox sharedInstance] update:self];
	[[BoundingBox sharedInstance] show:self];
}

- (void)windowDidBecomeMain:(NSNotification *)aNotification
{
	if (pollingUpdateNeeded)
	{
		pollingUpdateNeeded = false;
		switch ([controller pollingMode])
		{
			case 1:
				[self askForPollingUpdate];
				break;
			case 2:
				[self pollingUpdate];
				break;
		}
	}
	[self updateUtilityWindows:self];
}

- (void)errorFilterChange:(NSNotification *)aNotification
{
	[filteredRootErrorItem release];
	filteredRootErrorItem = nil;
	if ([controller currentModelWindow] == self)
	{
		[[ErrorsAndWarnings sharedInstance] update:self];
	}
}

- (LDViewController *)controller
{
	return controller;
}

- (void)lightVectorChanged:(TCAlert *)alert
{
	[modelView modelViewer]->setLightVector([[controller preferences] ldPreferences]->getLightVector());
	[modelView rotationUpdate];
	[[controller preferences] lightVectorChanged:alert];
}

- (IBAction)latLongRotation:(id)sender
{
	examineLatLong = !examineLatLong;
	[self setExamineLatLong:examineLatLong];
}

- (IBAction)examineMode:(id)sender
{
	[self setFlyThroughMode:false];
}

- (IBAction)flyThroughMode:(id)sender
{
	[self setFlyThroughMode:true];
}

- (bool)examineLatLong
{
	return examineLatLong;
}

- (bool)flyThroughMode
{
	return [modelView flyThroughMode];
}

- (BOOL)validateMenuItem:(id <NSMenuItem>)menuItem
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];

	if (modelViewer != NULL && modelViewer->getFilename() != NULL)
	{
		return [controller validateMenuItem:menuItem];
	}
	else
	{
		return menuItem == [controller statusBarMenuItem];
	}
}

- (IBAction)modelTree:(id)sender
{
	if (!modelTree)
	{
		modelTree = [[ModelTree alloc] initWithParent:self];
	}
	[modelTree toggle];
}

- (IBAction)partsList:(id)sender
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer)
	{
		partsList = modelViewer->getPartsList();
		
		if (partsList)
		{
			htmlInventory = new LDHtmlInventory;
			PartsList *partsListSheet = [[PartsList alloc] initWithModelWindow:self htmlInventory:htmlInventory];

			if ([partsListSheet runSheetInWindow:window] == NSOKButton)
			{
				NSString *htmlFilename = [NSString stringWithCString:htmlInventory->defaultFilename([modelView modelViewer]->getFilename()).c_str() encoding:NSASCIIStringEncoding];
				NSSavePanel *savePanel = [NSSavePanel savePanel];
				NSString *defaultFilename = [[htmlFilename lastPathComponent] stringByDeletingPathExtension];

				[savePanel setRequiredFileType:@"html"];
				[savePanel setCanSelectHiddenExtension:YES];
				sheetBusy = true;
				[savePanel beginSheetForDirectory:[self defaultSaveDirForOp:LDPreferences::SOPartsList] file:defaultFilename modalForWindow:window modalDelegate:self didEndSelector:@selector(htmlSavePanelDidEnd:returnCode:contextInfo:) contextInfo:NULL];
			}
			[partsListSheet release];
		}
	}

}

- (bool)isModelTreeOpen
{
	return [modelTree isOpen];
}

- (void)copyStringToPasteboard:(NSString *)string
{
	NSPasteboard *pb = [NSPasteboard generalPasteboard];

	[pb declareTypes:[NSArray arrayWithObject:NSStringPboardType] owner:self];
	[pb setString:string forType:NSStringPboardType];
}

- (IBAction)showViewInfo:(id)sender
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer != NULL)
	{
		ucstring message;
		ucstring commandLine;
		
		if (modelViewer->getViewInfo(message, commandLine))
		{
			if (NSRunAlertPanel([OCLocalStrings get:@"ViewInfoTitle"], [NSString stringWithUCString:message], [OCLocalStrings get:@"OK"], [OCLocalStrings get:@"Cancel"], nil) == NSOKButton)
			{
				[self copyStringToPasteboard:[NSString stringWithUCString:commandLine]];
			}
		}
	}
}

- (IBAction)showPovCameraInfo:(id)sender
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer != NULL)
	{
		UCSTR message;
		char *povCamera;

		modelViewer->getPovCameraInfo(message, povCamera);
		if (message && povCamera)
		{
			if (NSRunAlertPanel([OCLocalStrings get:@"PovCameraTitle"], [NSString stringWithUCString:message], [OCLocalStrings get:@"OK"], [OCLocalStrings get:@"Cancel"], nil) == NSOKButton)
			{
				[self copyStringToPasteboard:[NSString stringWithASCIICString:povCamera]];
			}
		}
		delete message;
		delete povCamera;
	}
}

- (IBAction)openGLDriverInfo:(id)sender
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer)
	{
		int numExtensions;
		UCSTR openGlMessage = modelViewer->getOpenGLDriverInfo(numExtensions);
		
		if (openGlMessage)
		{
			OpenGLDriverInfo *driverInfo = [[OpenGLDriverInfo alloc] init];
			
			[driverInfo showWithInfo:[NSString stringWithUCString:openGlMessage] numExtensions:numExtensions];
			delete openGlMessage;
			[driverInfo release];
		}
	}
}

//- (IBAction)print:(id)sender
//{
//	NSRunAlertPanel(@"Error", @"Print not yet implemented.", @"OK", nil, nil);
//}

- (IBAction)pollingMode:(id)sender
{
	[self updatePolling];
}

- (BOOL)canCopy
{
	if ([window firstResponder] == [modelTree outlineView])
	{
		return [modelTree canCopy];
	}
	else
	{
		return NO;
	}
}

- (IBAction)copy:(id)sender
{
	if ([window firstResponder] == [modelTree outlineView])
	{
		return [modelTree copy:sender];
	}
}

- (IBAction)toggleFullScreen:(id)sender
{
	[modelView toggleFullScreen:sender];
}

- (bool)fullScreen
{
	return [modelView fullScreen];;
}

@end
