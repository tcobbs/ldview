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
	TCWebClient::setUserAgent([userAgent cStringUsingEncoding:NSASCIIStringEncoding]);
	return [super init];
}

- (void)dealloc
{
	[modelWindows release];
	[preferences release];
	[statusBarMenuFormat release];
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

- (void)updateShowHideMenuItem:(NSMenuItem *)menuItem show:(BOOL)show format:(NSString *)format
{
	NSString *showHide;

	// Note: logic is backwards on purpose.  If show is set, then the menu needs
	// to say Hide, since the current state has the item shown.
	if (show)
	{
		showHide = @"Hide";
	}
	else
	{
		showHide = @"Show";
	}
	[menuItem setTitle:[NSString stringWithFormat:format, showHide]];
}

- (void)updateStatusBarMenuItem:(BOOL)showStatusBar
{
	[self updateShowHideMenuItem:statusBarMenuItem show:showStatusBar format:statusBarMenuFormat];
}

- (int)numberOfItemsInMenu:(NSMenu *)menu
{
	return [menu numberOfItems];
}

- (BOOL)menu:(NSMenu *)menu updateItem:(NSMenuItem *)item atIndex:(int)index shouldCancel:(BOOL)shouldCancel
{
	ModelWindow *modelWindow = (ModelWindow *)[[NSApp mainWindow] delegate];

	if (item == statusBarMenuItem && [modelWindow isKindOfClass:[ModelWindow class]])
	{
		[self updateStatusBarMenuItem:[modelWindow showStatusBar]];
	}
	return YES;
}

/*
- (BOOL)validateMenuItem:(id <NSMenuItem>)menuItem
{
	if (menuItem == statusBarMenuItem)
	{
		if ([[[NSApp mainWindow] delegate] isKindOfClass:[ModelWindow class]])
		{
			return YES;
		}
		else
		{
			return NO;
		}
	}
	return YES;
}
*/

- (void)awakeFromNib
{
	//showStatusBar = [OCUserDefaults longForKey:@"StatusBar" defaultValue:1 sessionSpecific:NO];
	statusBarMenuFormat = [[statusBarMenuItem title] retain];
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
		if (([theEvent modifierFlags] & (NSShiftKeyMask | NSControlKeyMask | NSAlternateKeyMask | NSCommandKeyMask)) == NSControlKeyMask)
		{
			[preferences hotKeyPressed:character - '0'];
			[modelWindows makeObjectsPerformSelector:@selector(reloadNeeded)];
			//NSLog(@"Ctrl+%d pressed.\n", character - '0');
		}
	}
	[super keyUp:theEvent];
}

- (BOOL)performKeyEquivalent:(NSEvent *)theEvent
{
	return [super performKeyEquivalent:theEvent];
}

@end
