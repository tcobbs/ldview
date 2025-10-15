// The following is necessary to get rid of some truly screwed up warnings
#pragma GCC visibility push(default)

#import "LDViewController.h"
#import "ModelWindow.h"
#import "LDrawModelView.h"
#import "OCLocalStrings.h"
#import "OCUserDefaults.h"
#import "LDrawPage.h"
#import "Updater.h"
#import "Preferences.h"
#import "GeneralPage.h"
#import "LDViewCategories.h"

#include <LDLib/LDrawModelViewer.h>
#include <LDLib/LDInputHandler.h>
#include <LDLib/LDPreferences.h>
#include <TRE/TREMainModel.h>
#include <TCFoundation/TCWebClient.h>
#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <LDLoader/LDLModel.h>
#include <TCFoundation/TCAutoreleasePool.h>

@implementation LDViewController

- (void)processTCReleases:(NSTimer*)theTimer
{
	TCAutoreleasePool::processReleases();
}


- (id)init
{
	if ((self = [super init]) != nil)
	{
		NSString *userAgent = [NSString stringWithFormat:@"LDView/%@ (Mac OSX; ldview@gmail.com; https://github.com/tcobbs/ldview)", [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"]];
		ldrawFileTypes = [[NSArray alloc] initWithObjects: @"ldr", @"dat", @"mpd",
			nil];
		TREMainModel::loadStudTexture([[[NSBundle mainBundle] pathForResource:
			@"StudLogo" ofType:@"png"] UTF8String]);
		modelWindows = [[NSMutableArray alloc] init];
		TCWebClient::setUserAgent([userAgent UTF8String]);
		pollingMode = TCUserDefaults::longForKey(POLL_KEY, 0, false);
		tcAutoreleaseTimer = [[NSTimer scheduledTimerWithTimeInterval:1.0f target:self selector:@selector(processTCReleases:) userInfo:nil repeats:YES] retain];
		standardSizes = [[NSMutableArray alloc] init];
	}
	return self;
}

- (void)dealloc
{
	[tcAutoreleaseTimer invalidate];
	[tcAutoreleaseTimer release];
	[ldrawFileTypes release];
	[modelWindows release];
	[preferences release];
	[statusBarMenuFormat release];
	[toolbarStyleMenuFormat release];
	[standardSizes release];
	[noWindowText release];
	[super dealloc];
}

- (NSArray *)modelWindows
{
	return modelWindows;
}

- (ModelWindow *)currentModelWindow
{
	id delegate = [[[NSApplication sharedApplication] mainWindow] delegate];
	
	if ([delegate isKindOfClass:[ModelWindow class]])
	{
		return delegate;
	}
	else
	{
		return nil;
	}
}

- (LDrawModelView *)currentModelView
{
	return [[self currentModelWindow] modelView];
}

- (void)updateShowHideMenuItem:(NSMenuItem *)menuItem show:(BOOL)show format:(NSString *)format
{
	NSString *showHide;

	// Note: logic is backwards on purpose.  If show is set, then the menu needs
	// to say Hide, since the current state has the item shown.
	if (show)
	{
		showHide = [OCLocalStrings get:@"Hide"];
	}
	else
	{
		showHide = [OCLocalStrings get:@"Show"];
	}
	[menuItem setTitle:[NSString stringWithFormat:format, showHide]];
}

- (void)updateStatusBarMenuItem:(BOOL)showStatusBar
{
	[self updateShowHideMenuItem:statusBarMenuItem show:showStatusBar format:statusBarMenuFormat];
}

- (void)updateToolbarStyleMenuItem:(bool)compactToolbar
{
	NSString *style;

	// Note: logic is backwards on purpose.  If compactToolbar is set, then the
	// menu needs to say Expanded, selecting the menu item switches to the state
	// that is shown on the item (similar to the show/hide items).
	if (compactToolbar)
	{
		style = [OCLocalStrings get:@"Expanded"];
	}
	else
	{
		style = [OCLocalStrings get:@"Compact"];
	}
	[toolbarStyleMenuItem setTitle:[NSString stringWithFormat:toolbarStyleMenuFormat, style]];
}

- (NSInteger)numberOfItemsInMenu:(NSMenu *)menu
{
	return [menu numberOfItems];
}

- (void)updateLatLongRotationMenuItem:(ModelWindow *)modelWindow
{
	bool examineLatLong = [modelWindow examineLatLong];
	bool examineMode = [modelWindow examineMode];

	if (examineLatLong)
	{
		[latLongRotationMenuItem setState:NSOnState];
	}
	else
	{
		[latLongRotationMenuItem setState:NSOffState];
	}
	if (examineMode)
	{
		[latLongRotationMenuItem setEnabled:YES];
	}
	else
	{
		[latLongRotationMenuItem setEnabled:NO];
	}
}

- (void)setStatusBarMenuItemDisabled:(BOOL)disabled
{
	statusBarMenuItemDisabled = disabled;
}

- (void)updateViewModeMenuItems:(long)viewMode keepRightSideUp:(bool)keepRightSideUp
{
	[examineMenuItem setState:NSOffState];
	[flyThroughMenuItem setState:NSOffState];
	[walkMenuItem setState:NSOffState];
	[latLongRotationMenuItem setEnabled:NO];
	[keepRightSideUpMenuItem setEnabled:NO];
	switch (viewMode)
	{
		case LDInputHandler::VMExamine:
			[examineMenuItem setState:NSOnState];
			[latLongRotationMenuItem setEnabled:YES];
			break;
		case LDInputHandler::VMFlyThrough:
			[flyThroughMenuItem setState:NSOnState];
			[keepRightSideUpMenuItem setEnabled:YES];
			break;
		case LDInputHandler::VMWalk:
			[walkMenuItem setState:NSOnState];
			break;
	}
	[keepRightSideUpMenuItem setState:keepRightSideUp ? NSOnState : NSOffState];
}

- (void)updateAlwaysOnTopMenuItem:(NSInteger)level
{
	if (level == NSNormalWindowLevel)
	{
		[alwaysOnTopMenuItem setState:NSOffState];
	}
	else
	{
		[alwaysOnTopMenuItem setState:NSOnState];
	}
}

- (NSSize)calcMaxSize:(ModelWindow *)modelWindow
{
	NSSize screenSize = [[[modelWindow window] screen] visibleFrame].size;
	NSSize marginSize = [modelWindow mainMarginSize];
	
	return NSMakeSize(screenSize.width - marginSize.width, screenSize.height - marginSize.height);
}

- (IBAction)standardSize:(id)sender
{
	int index = (int)[sender tag];
	
	if (index >= 0 && (NSUInteger)index < [standardSizes count])
	{
		NSDictionary *size = [standardSizes objectAtIndex:index];
		int width = [[size objectForKey:@"Width"] intValue];
		int height = [[size objectForKey:@"Height"] intValue];
		
		[[self currentModelWindow] setMainViewWidth:width height:height];
	}
}

- (void)setupStandardSizes
{
	ModelWindow *modelWindow = [self currentModelWindow];
	NSSize newMaxSize;

	if (noWindowText == nil)
	{
		noWindowText = [[[standardSizesMenu itemAtIndex:0] title] retain];
	}
	if (modelWindow != nil)
	{
		newMaxSize = [self calcMaxSize:modelWindow];
	}
	else
	{
		newMaxSize.width = newMaxSize.height = 0.0f;
	}
	if (newMaxSize.width != maxSize.width || newMaxSize.height != maxSize.height)
	{
		LDrawModelViewer::StandardSizeVector sizes;
		size_t i;

		maxSize = newMaxSize;
		while ([standardSizesMenu numberOfItems] > 0)
		{
			[standardSizesMenu removeItemAtIndex:[standardSizesMenu numberOfItems] - 1];
		}
		LDrawModelViewer::getStandardSizes((int)maxSize.width, (int)maxSize.height, sizes);
		[standardSizes removeAllObjects];
		if (sizes.size() > 0)
		{
			for (i = 0; i < sizes.size(); i++)
			{
				const LDrawModelViewer::StandardSize &size = sizes[i];
				NSString *name = [NSString stringWithUCString:size.name];
				
				[standardSizes addObject:[NSDictionary dictionaryWithObjectsAndKeys:name, @"Name", [NSNumber numberWithInt:size.width], @"Width", [NSNumber numberWithInt:size.height], @"Height", nil]];
				[standardSizesMenu addItemWithTitle:name action:@selector(standardSize:) keyEquivalent:@""];
				[[standardSizesMenu itemAtIndex:i] setTag:i];
			}
		}
		else
		{
			[standardSizesMenu addItemWithTitle:noWindowText action:NULL keyEquivalent:@""];
		}
	}
}

- (BOOL)menu:(NSMenu *)menu updateItem:(NSMenuItem *)item atIndex:(NSInteger)index shouldCancel:(BOOL)shouldCancel
{
	ModelWindow *modelWindow = [self currentModelWindow];

	if (modelWindow != nil)
	{
		if (item == statusBarMenuItem)
		{
			[self updateStatusBarMenuItem:[modelWindow showStatusBar]];
		}
		else if (item == toolbarStyleMenuItem)
		{
			[self updateToolbarStyleMenuItem:[modelWindow compactToolbar]];
		}
		else if (item == alwaysOnTopMenuItem)
		{
			[self updateAlwaysOnTopMenuItem:[[modelWindow window] level]];
		}
		else if (item == latLongRotationMenuItem)
		{
			[self updateLatLongRotationMenuItem:modelWindow];
		}
		else if (item == examineMenuItem)
		{
			[self updateViewModeMenuItems:[modelWindow viewMode] keepRightSideUp:[modelWindow keepRightSideUp]];
		}
		else if (item == modelTreeMenuItem)
		{
			[item setState:[modelWindow isModelTreeOpen] ? NSOnState : NSOffState];
		}
		else if (item == mpdMenuItem)
		{
			[item setState:[modelWindow isMPDOpen] ? NSOnState : NSOffState];
		}
		else if (item == povCameraAspectMenuItem)
		{
			[item setState:[[modelWindow modelView] modelViewer]->getPovCameraAspect() ? NSOnState : NSOffState];
		}
	}
	if (item == pollingDisabledMenuItem)
	{
		[item setState:pollingMode == 0 ? NSOnState : NSOffState];
	}
	else if (item == pollingPromptMenuItem)
	{
		[item setState:pollingMode == 1 ? NSOnState : NSOffState];
	}
	else if (item == pollingAutoLaterMenuItem)
	{
		[item setState:pollingMode == 2 ? NSOnState : NSOffState];
	}
	else if (item == pollingAutoNowMenuItem)
	{
		[item setState:pollingMode == 3 ? NSOnState : NSOffState];
	}
	else if ([item submenu] == standardSizesMenu)
	{
		[self setupStandardSizes];
	}
	return YES;
}

//- (BOOL)validateMenuItem:(id <NSMenuItem>)menuItem
- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
	ModelWindow *modelWindow = [self currentModelWindow];

	if ([modelWindow loading])
	{
		if (menuItem == cancelMenuItem)
		{
			return YES;
		}
		else
		{
			return NO;
		}
	}
	else
	{
		if (menuItem == copyMenuItem)
		{
			return [modelWindow canCopy];
		}
		else if (menuItem == statusBarMenuItem)
		{
			return !statusBarMenuItemDisabled;
		}
		else if (menuItem == cancelMenuItem || [modelWindow sheetBusy])
		{
			return NO;
		}
		else if (menuItem == latLongRotationMenuItem)
		{
			if ([modelWindow examineMode])
			{
				return YES;
			}
			else
			{
				return NO;
			}
		}
		else if ([menuItem menu] == stepMenu)
		{
			LDrawModelViewer *modelViewer = [[modelWindow modelView] modelViewer];
			size_t numSteps = modelViewer->getNumSteps();
			size_t step = modelViewer->getStep();
			
			if (menuItem == stepFirstMenuItem || menuItem == stepPrevMenuItem)
			{
				return numSteps > 1 && step > 1;
			}
			else if (menuItem == stepNextMenuItem || menuItem == stepLastMenuItem)
			{
				return numSteps > 1 && step < numSteps;
			}
			else if (menuItem == stepGoToMenuItem)
			{
				return numSteps > 1;
			}
		}
	}
	return YES;
}

- (void)awakeFromNib
{
	//showStatusBar = [OCUserDefaults longForKey:@"StatusBar" defaultValue:1 sessionSpecific:NO];
	statusBarMenuFormat = [[statusBarMenuItem title] retain];
	if (@available(macOS 11.0, *))
	{
		toolbarStyleMenuFormat = [[toolbarStyleMenuItem title] retain];
	}
	else
	{
		[[toolbarStyleMenuItem menu] removeItem:toolbarStyleMenuItem];
	}
	[self updateStatusBarMenuItem:YES];
}

- (BOOL)createWindow:(NSString *)filename
{
	ModelWindow *modelWindow = [[ModelWindow alloc] initWithController:self];

	if ([modelWindows count] > 0)
	{
		NSWindow *lastWindow = [[modelWindows lastObject] window];
		NSRect frame = [lastWindow frame];

		frame.origin.y += frame.size.height;
		frame.origin = [lastWindow cascadeTopLeftFromPoint:frame.origin];
		[[modelWindow window] setFrameTopLeftPoint:frame.origin];
	}
	[modelWindows addObject:modelWindow];
	[modelWindow release];
	[[self preferences] initModelWindow:modelWindow];
	[modelWindow show];
	if (filename)
	{
		return [modelWindow openModel:filename];
	}
	else
	{
		return NO;
	}
}

- (void)modelWindowWillClose:(ModelWindow *)modelWindow
{
	[modelWindows removeObject:modelWindow];
	[[self preferences] modelWindowWillClose:modelWindow];
}

- (IBAction)newWindow:(id)sender
{
	[self createWindow:nil];
}

- (void)recordRecentFile:(NSString *)filename
{
	[[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL:[NSURL fileURLWithPath:filename]];
}

- (BOOL)browseForLDrawDir
{
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];

	openPanel.allowsMultipleSelection = NO;
	openPanel.canChooseFiles = NO;
	openPanel.canChooseDirectories = YES;
	openPanel.message = [OCLocalStrings get:@"SelectLDrawFolder"];
	openPanel.directoryURL = [NSURL fileURLWithPath:NSHomeDirectory()];
	if ([openPanel runModal] == NSModalResponseOK)
	{
		[openPanel orderOut:self];
		if ([self verifyLDrawDir:[openPanel ldvFilename] prompt:NO])
		{
			[[[self preferences] ldrawPage] updateLDrawDir:[openPanel ldvFilename]];
			return YES;
		}
		else
		{
			if ([NSAlert runModalWithTitle:[OCLocalStrings get:@"Error"] message:[OCLocalStrings get:@"LDrawNotInFolder"] defaultButton:[OCLocalStrings get:@"Yes"] alternateButton:[OCLocalStrings get:@"No"] otherButton:nil isCritical:YES] == NSAlertFirstButtonReturn)
			{
				return [self browseForLDrawDir];
			}
		}
	}
	else
	{
		[openPanel orderOut:self];
	}
	//NSRunCriticalAlertPanel([OCLocalStrings get:@"Error"], [OCLocalStrings get:@"LDrawFolderRequired"], [OCLocalStrings get:@"OK"], nil, nil);
	return NO;
}

- (BOOL)verifyLDrawDir
{
	LDPreferences *ldPreferences = [preferences ldPreferences];
	if (ldPreferences != NULL && !ldPreferences->shouldVerifyLDrawDir())
	{
		return YES;
	}
	return [self verifyLDrawDir:[[[self preferences] ldrawPage] ldrawDir] prompt:YES];
}

- (BOOL)checkForUpdates:(NSString *)parentDir full:(bool)full
{
	return [self checkForUpdates:parentDir full:full isParent:true];
}

- (BOOL)checkForUpdates:(NSString *)dir full:(bool)full isParent:(bool)isParent
{
	Updater *updater = [[Updater alloc] init];

	if ((full && [updater downloadLDraw:dir]) || (!full && [updater checkForUpdates:dir isParent:isParent]))
	{
		[updater release];
		return YES;
	}
	else
	{
		[updater release];
		return NO;
	}
}

- (BOOL)downloadLDraw
{
	NSString *parentDir = [NSHomeDirectory() stringByAppendingPathComponent:@"Library"];
	
	if ([self checkForUpdates:parentDir full:true])
	{
		NSString *ldrawDir = [parentDir stringByAppendingPathComponent:@"ldraw"];
		[[[self preferences] ldrawPage] updateLDrawZipPath:[NSString stringWithUTF8String:LDLModel::ldrawZipPath().c_str()]];
		[[[self preferences] ldrawPage] updateLDrawDir:ldrawDir];
		[NSAlert runModalWithTitle:[OCLocalStrings get:@"LDrawInstalled"] message:[NSString stringWithFormat:[OCLocalStrings get:@"LDrawInstalledFormat"], ldrawDir] defaultButton:nil alternateButton:nil otherButton:nil];
//		NSRunAlertPanel([OCLocalStrings get:@"LDrawInstalled"], [NSString stringWithFormat:[OCLocalStrings get:@"LDrawInstalledFormat"], ldrawDir], [OCLocalStrings get:@"OK"], nil, nil);
		return YES;
	}
	else
	{
		return NO;
	}
}

- (IBAction)checkForLibraryUpdates:(id)sender
{
	NSString *ldrawZipPath = [[[self preferences] ldrawPage] ldrawZipPath];
	NSString *ldrawDir = [[[self preferences] ldrawPage] ldrawDir];
	
	if ([self verifyLDrawDir:ldrawDir prompt:NO])
	{
		NSString *lastPathComponent = [ldrawDir lastPathComponent];
		if ([ldrawZipPath length] != 0)
		{
			bool suppress = TCUserDefaults::boolForKey(SUPPRESS_REPLACE_LDRAW_ZIP_KEY, false, false);
			NSAlert *alert = nil;
			if (!suppress) {
				NSString *message = [[OCLocalStrings get:@"ReplaceLDrawZipMessage"] stringByReplacingOccurrencesOfString:@"%s" withString:ldrawZipPath];
				alert = [NSAlert alertWithTitle:[OCLocalStrings get:@"ReplaceLDrawZipTitle"] message:message defaultButton:[OCLocalStrings get:@"Yes"] alternateButton:[OCLocalStrings get:@"No"] otherButton:nil isCritical:NO];
				alert.showsSuppressionButton = YES;
			}
			if (suppress || [alert runModal] == NSAlertFirstButtonReturn)
			{
				if (alert.suppressionButton.state == NSControlStateValueOn)
				{
					// Note: only pay attention to suppressionButton.state if they choose
					// "Yes". If we paid attention to the state when they choose "No" and
					// stored their choice, future update checks would do nothing at all.
					TCUserDefaults::setBoolForKey(true, SUPPRESS_REPLACE_LDRAW_ZIP_KEY, false);
				}
				[self checkForUpdates:ldrawDir full:false isParent:false];
			}
		}
		else if ([lastPathComponent caseInsensitiveCompare:@"ldraw"] == NSOrderedSame)
		{
			[self checkForUpdates:[ldrawDir stringByDeletingLastPathComponent] full:false];
		}
		else
		{
			[NSAlert runModalWithTitle:[OCLocalStrings get:@"CannotUpdate"] message:[NSString stringWithFormat:[OCLocalStrings get:@"AutoUpdatesBadFolder"], lastPathComponent] defaultButton:nil alternateButton:nil otherButton:nil];
//			NSRunAlertPanel([OCLocalStrings get:@"CannotUpdate"], [NSString stringWithFormat:[OCLocalStrings get:@"AutoUpdatesBadFolder"], lastPathComponent], [OCLocalStrings get:@"OK"], nil, nil);
		}
	}
	else
	{
		[self downloadLDraw];
	}
}

- (BOOL)verifyLDrawDir:(NSString *)ldrawDir prompt:(BOOL)prompt
{
	if (![LDrawPage verifyLDrawDir:ldrawDir ldPreferences:[preferences ldPreferences]])
	{
		if (prompt)
		{
			BOOL retValue = NO;

			switch ([NSAlert runModalWithTitle:[OCLocalStrings get:@"LDrawFolderNotFoundHeader"] message:[OCLocalStrings get:@"LDrawFolderNotFound"] defaultButton:[OCLocalStrings get:@"BrowseToLDrawFolder"] alternateButton:[OCLocalStrings get:@"DownloadFromLDrawOrg"] otherButton:[OCLocalStrings get:@"Cancel"]])
			{
				case NSAlertFirstButtonReturn:
					retValue = [self browseForLDrawDir];
					break;
				case NSAlertSecondButtonReturn:
					retValue = [self downloadLDraw];
					break;
				case NSAlertThirdButtonReturn:
					break;
			}
			if (!retValue)
			{
				[NSAlert runModalWithTitle:[OCLocalStrings get:@"Error"] message:[OCLocalStrings get:@"LDrawFolderRequired"] defaultButton:nil alternateButton:nil otherButton:nil isCritical:YES];
			}
			return retValue;
		}
		else
		{
			return NO;
		}
	}
	return YES;
}

- (BOOL)newWindowNeeded
{
	return forceNewWindow || [[[self preferences] generalPage] newModelWindows] || [[self currentModelWindow] sheetBusy];
}

- (BOOL)openFile:(NSString *)filename
{
	if (![self verifyLDrawDir])
	{
		return NO;
	}
	if (![[NSApplication sharedApplication] mainWindow] || [self newWindowNeeded])
	{
		if ([self createWindow:filename])
		{
			return YES;
		}
	}
	else
	{
		if ([[self currentModelWindow] openModel:filename])
		{
			return YES;
		}
	}
	return NO;
}

- (void)openModelInNewWindow:(BOOL)newWindow
{
	if ([self verifyLDrawDir])
	{
		NSOpenPanel *openPanel = [NSOpenPanel openPanel];

		openPanel.message = [OCLocalStrings get:@"SelectModelFile"];
		NSString *lastOpenPath = [OCUserDefaults stringForKey:[NSString stringWithUTF8String:LAST_OPEN_PATH_KEY] defaultValue:nil sessionSpecific:NO];
		if (lastOpenPath != nil && lastOpenPath.length > 0)
		{
			openPanel.directoryURL = [NSURL fileURLWithPath:lastOpenPath];
		}
		openPanel.allowedFileTypes = ldrawFileTypes;
		if ([openPanel runModal] == NSModalResponseOK)
		{
			[openPanel orderOut:self];
			if (newWindow)
			{
				[self createWindow:[openPanel ldvFilename]];
			}
			else
			{
				[self openFile:[openPanel ldvFilename]];
			}
		}
		else
		{
			[openPanel orderOut:self];
		}
	}
}

- (void)openModel
{
	[self openModelInNewWindow:NO];
}

- (IBAction)openModel:(id)sender
{
	[self openModelInNewWindow:[self newWindowNeeded]];
}

- (Preferences *)preferences
{
	if (!preferences)
	{
		preferences = [[Preferences alloc] initWithController:self];
	}
	return preferences;
}

- (IBAction)preferences:(id)sender
{
	[[self preferences] show];
}

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename
{
	didInitialLoad = YES;
	if ([self openFile:filename])
	{
		launchFileOpened = YES;
		return YES;
	}
	else
	{
		return NO;
	}
}

- (int)commandLineStep
{
	return commandLineStep;
}

+ (NSString *)appVersion
{
	NSBundle *mainBundle = [NSBundle mainBundle];
	NSDictionary *infoDict = [mainBundle infoDictionary];
	NSString *version = [infoDict objectForKey:@"CFBundleShortVersionString"];
	if (version == nil)
	{
		NSLog(@"No CFBundleShortVersionString in mainBundle!");
		return @"";
	}
	return version;
}

+ (NSString *)appCopyright
{
	NSBundle *mainBundle = [NSBundle mainBundle];
	NSDictionary *localizedInfoDict = [mainBundle localizedInfoDictionary];
	NSMutableString *copyrightString = [[localizedInfoDict objectForKey:@"NSHumanReadableCopyright"] mutableCopy];
	NSCharacterSet *charSet = [NSCharacterSet characterSetWithRange:NSMakeRange(169, 1)];
	NSRange range = [copyrightString rangeOfCharacterFromSet:charSet];
	if (range.length > 0)
	{
		[copyrightString replaceCharactersInRange:range withString:@"(C)"];
	}
	if (copyrightString == nil)
	{
		NSLog(@"No NSHumanReadableCopyright in mainBundle!");
		return @"";
	}
	return [copyrightString autorelease];
}

- (void)tryInitialLoad
{
	// We need to make sure not to open the first file until after we are
	// active.
	if (didInitialLoad || ![NSApplication sharedApplication].isActive)
	{
		return;
	}
	didInitialLoad = YES;
	TCStringArray *unhandledArgs = TCUserDefaults::getUnhandledCommandLineArgs();
	BOOL opened = NO;
	std::string stepString = TCUserDefaults::commandLineStringForKey(STEP_KEY);
	int step;

	if (sscanf(stepString.c_str(), "%i", &step) == 1)
	{
		commandLineStep = step;
	}
	if (unhandledArgs != NULL)
	{
		size_t count = unhandledArgs->getCount();
		size_t i;

		forceNewWindow = YES;
		for (i = 0; i < count; i++)
		{
			char *arg = unhandledArgs->stringAtIndex(i);
			
			if (arg[0] != '-' && arg[0] != 0)
			{
				if ([self openFile:[NSString stringWithUTF8String:arg]])
				{
					opened = YES;
				}
			}
		}
		forceNewWindow = NO;
	}
	commandLineStep = 0;
	if (!opened)
	{
		[self openModel];
	}
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	LDrawModelViewer::setAppVersion([[LDViewController appVersion] UTF8String]);
	LDrawModelViewer::setAppCopyright([[LDViewController appCopyright] UTF8String]);
	if (!launchFileOpened && [[[self preferences] generalPage] promptAtStartup])
	{
		[self tryInitialLoad];
	}
}

- (void)applicationDidBecomeActive:(NSNotification *)aNotification
{
	[self tryInitialLoad];
}

- (IBAction)resetView:(id)sender
{
	[[self currentModelView] resetView:sender];
}

/*
- (IBAction)toggleStatusBar:(id)sender
{
	showStatusBar = !showStatusBar;
	[self updateStatusBarMenuItem];
	[OCUserDefaults setLong:showStatusBar forKey:@"StatusBar" sessionSpecific:NO];
	for (int i = 0; i < [modelWindows count]; i++)
	{
		[[modelWindows objectAtIndex:i] setShowStatusBar:showStatusBar];
	}
}
*/

- (BOOL)acceptsFirstResponder
{
	return NO;
}

- (BOOL)becomeFirstResponder
{
	return NO;
}

- (void)keyUp:(NSEvent *)theEvent
{
	unichar character = [[theEvent charactersIgnoringModifiers] characterAtIndex:0];
	
	if (character >= '0' && character <= '9')
	{
		if (([theEvent modifierFlags] & (NSEventModifierFlagShift | NSEventModifierFlagControl | NSEventModifierFlagOption | NSEventModifierFlagCommand)) == NSEventModifierFlagControl)
		{
			[preferences hotKeyPressed:character - '0'];
			[modelWindows makeObjectsPerformSelector:@selector(reloadNeeded)];
			return;
			//NSLog(@"Ctrl+%d pressed.\n", character - '0');
		}
	}
	[[self currentModelView] keyUp:theEvent];
	//[super keyUp:theEvent];
}

- (BOOL)performKeyEquivalent:(NSEvent *)theEvent
{
	return [super performKeyEquivalent:theEvent];
}

- (NSMenu *)viewingAngleMenu
{
	return viewingAngleMenu;
}

- (NSMenuItem *)statusBarMenuItem
{
	return statusBarMenuItem;
}

- (IBAction)showHelp:(id)sender
{
	NSString *helpPath = [[NSBundle mainBundle] pathForResource:@"Help" ofType:@"html"];
	NSURL *helpURL = [NSURL URLWithString:[[[NSURL fileURLWithPath:helpPath] absoluteString] stringByAppendingString:@"#MacNotes"]];

	//[[NSWorkspace sharedWorkspace] openURL:helpURL];
	// NSWorkspace strips off the #MacNotes when openURL: is used.  What a pain.
	// Fortunately, all the system APIs that expect CFURLRefs will automatically
	// work with an NSURL simply by type-casting, so the solution is trivial.
	LSOpenCFURLRef((CFURLRef)helpURL, NULL);
}

- (IBAction)print:(id)sender
{
	[[self currentModelView] print:sender];
}

- (IBAction)pollingMode:(id)sender
{
	pollingMode = [sender tag];
	TCUserDefaults::setLongForKey(pollingMode, POLL_KEY, false);
	[modelWindows makeObjectsPerformSelector:@selector(pollingMode:) withObject:sender];
}

- (long)pollingMode
{
	return pollingMode;
}

- (void)keyDown:(NSEvent *)event
{
	[[self currentModelView] keyDown:event];
}

- (NSMenuItem *)prefsMenuItem
{
	return prefsMenuItem;
}

@end
