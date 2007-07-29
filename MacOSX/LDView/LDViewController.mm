#import "LDViewController.h"
#import "ModelWindow.h"
#import "LDrawModelView.h"
#import "OCLocalStrings.h"
#import "OCUserDefaults.h"
#include <LDLib/LDrawModelViewer.h>
#include <TRE/TREMainModel.h>
#include <TCFoundation/TCWebClient.h>
#include "Preferences.h"

@implementation LDViewController

- (id)init
{
	NSString *userAgent = [NSString stringWithFormat:@"LDView/%@ (Mac OSX; ldview@gmail.com; http://ldview.sf.net/)", [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"]];
	[OCLocalStrings loadStringTable:[[NSBundle mainBundle]
		pathForResource:@"LDViewMessages" ofType:@"ini"]];
	ldrawFileTypes = [[NSArray alloc] initWithObjects: @"ldr", @"dat", @"mpd",
		nil];
	TREMainModel::loadStudTexture([[[NSBundle mainBundle] pathForResource:
		@"StudLogo" ofType:@"png"] cStringUsingEncoding:NSASCIIStringEncoding]);
	modelWindows = [[NSMutableArray alloc] init];
	TCWebClient::setUserAgent([userAgent cString]);
	return [super init];
}

- (void)dealloc
{
	[modelWindows release];
	[preferences release];
	[super dealloc];
}

- (NSArray *)modelWindows
{
	return modelWindows;
}

- (ModelWindow *)currentModelWindow
{
	return (ModelWindow *)[[[NSApplication sharedApplication] mainWindow]
		delegate];
}

- (LDrawModelView *)currentModelView
{
	return [[self currentModelWindow] modelView];
}

- (void)updateStatusBarMenuItem
{
	if (showStatusBar)
	{
		[statusBarMenuItem setTitle:@"Hide Status Bar"];
	}
	else
	{
		[statusBarMenuItem setTitle:@"Show Status Bar"];
	}
}

- (BOOL)createWindow:(NSString *)filename
{
	ModelWindow *modelWindow = [[ModelWindow alloc] initWithController:self];

	showStatusBar = [OCUserDefaults longForKey:@"StatusBar" defaultValue:1 sessionSpecific:NO];
	[self updateStatusBarMenuItem];
	[modelWindow setShowStatusBar:showStatusBar];
	[modelWindows addObject:modelWindow];
	[modelWindow release];
	[[self preferences] initModelWindow:modelWindow];
	[modelWindow show];
	//[[[modelWindow window] contentView] setNeedsDisplay:YES];
	if (filename)
	{
		return [modelWindow openModel:filename];
	}
	else
	{
		return NO;
	}
}

- (IBAction)newWindow:(id)sender
{
	[self createWindow:nil];
}

- (BOOL)openFile:(NSString *)filename
{
	if (![[NSApplication sharedApplication] mainWindow])
	{
		return [self createWindow:filename];
	}
	else
	{
		return [[self currentModelWindow] openModel:filename];
	}
}

- (void)openModel
{
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];

	[openPanel setMessage:[OCLocalStrings get:@"SelectModelFile"]];
	if ([openPanel runModalForTypes:ldrawFileTypes] == NSOKButton)
	{
		[self openFile:[openPanel filename]];
	}
}

- (IBAction)openModel:(id)sender
{
	return [self openModel];
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

- (BOOL)application:(NSApplication *)theApplication
		   openFile:(NSString *)filename
{
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

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	if (!launchFileOpened)
	{
		[self openModel];
	}
}

- (IBAction)resetView:(id)sender
{
	[[self currentModelView] resetView:sender];
}

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

@end
