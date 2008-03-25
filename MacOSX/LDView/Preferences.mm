#import "Preferences.h"
#include "ModelWindow.h"
#include "LDrawModelView.h"
#include "PreferencePage.h"
#include "LDViewController.h"
#include <LDLib/LDPreferences.h>

@implementation Preferences

NSString *LDPreferencesDidUpdateNotification = @"LDPreferencesDidUpdate";

- (id)initWithController:(LDViewController *)value
{
	if ((self = [super init]) != nil)
	{
		controller = value;	// Don't retain: it's our parent.
		pages = [[NSMutableArray alloc] initWithCapacity:[[tabView tabViewItems] count]];
		ldPreferences = new LDPreferences;
		[NSBundle loadNibNamed:@"Preferences.nib" owner:self];
	}
	return self;
}

- (void)dealloc
{
	TCObject::release(ldPreferences);
	[pages release];
	[super dealloc];
}

- (void)loadSettings
{
	ldPreferences->loadSettings();
	[pages makeObjectsPerformSelector:@selector(setup)];
}

- (void)show
{
	[window makeKeyAndOrderFront:self];
}

- (void)awakeFromNib
{
	generalIndex = 0;
	ldrawIndex = 1;
	geometryIndex = 2;
	effectsIndex = 3;
	primitivesIndex = 4;
	updatesIndex = 5;
	prefSetsIndex = 6;
	[pages addObject:generalPage];
	[pages addObject:ldrawPage];
	[pages addObject:geometryPage];
	[pages addObject:effectsPage];
	[pages addObject:primitivesPage];
	[pages addObject:updatesPage];
	[pages addObject:prefSetsPage];
	[pages makeObjectsPerformSelector:@selector(setPreferences:) withObject:self];
	[self loadSettings];
}

- (void)initModelWindow:(ModelWindow *)value
{
	ldPreferences->setModelViewer([[value modelView] modelViewer]);
	ldPreferences->applySettings();
}

- (LDPreferences *)ldPreferences
{
	return ldPreferences;
}

- (bool)applyFailed
{
	return applyFailedPage != nil;
}

- (void)setApplyFailed:(PreferencePage *)page
{
	applyFailedPage = page;
	if (page)
	{
		[tabView selectTabViewItemAtIndex:[pages indexOfObject:applyFailedPage]];
	}
}

- (IBAction)apply:(id)sender
{
	NSArray *modelWindows = [controller modelWindows];

	applyFailedPage = nil;
	[pages makeObjectsPerformSelector:@selector(apply)];
	if (applyFailedPage == nil)
	{
		for (int i = 0; i < [modelWindows count]; i++)
		{
			LDrawModelView *modelView = [[modelWindows objectAtIndex:i] modelView];
			ldPreferences->setModelViewer([modelView modelViewer]);
			ldPreferences->applySettings();
			ldPreferences->commitSettings();
			[modelView setNeedsDisplay:YES];
		}
		[[NSNotificationCenter defaultCenter] postNotificationName:LDPreferencesDidUpdateNotification object:self];
	}
}

- (void)hotKeyPressed:(int)index
{
	[prefSetsPage hotKeyPressed:index];
}

- (IBAction)cancel:(id)sender
{
	[window orderOut:sender];
	[pages makeObjectsPerformSelector:@selector(setup)];
}

- (IBAction)ok:(id)sender
{
	[self apply:sender];
	if (applyFailedPage == nil)
	{
		[window orderOut:sender];
	}
}

- (void)enableApply:(BOOL)enabled
{
	[applyButton setEnabled:enabled];
	[window setDocumentEdited:enabled];
}

- (BOOL)isApplyEnabled
{
	return [applyButton isEnabled];
}

- (NSWindow *)window
{
	return window;
}

- (BOOL)tabView:(NSTabView *)aTabView shouldSelectTabViewItem:(NSTabViewItem *)tabViewItem
{
	return [[pages objectAtIndex:[aTabView indexOfTabViewItem:[tabView selectedTabViewItem]]] canSwitchPages];
}

- (bool)getBoolFrom:(id)sender
{
	if ([sender isKindOfClass:[NSSegmentedControl class]])
	{
		return [[sender cell] isSelectedForSegment:[sender selectedSegment]] ? true : false;
	}
	return false;
}

- (void)takeWireframeFrom:(id)sender
{
	ldPreferences->setDrawWireframe([self getBoolFrom:sender]);
	[[pages objectAtIndex:geometryIndex] setup];
	[self apply:sender];
}

- (void)takeSeamsFrom:(id)sender
{
	ldPreferences->setUseSeams([self getBoolFrom:sender]);
	[[pages objectAtIndex:geometryIndex] setup];
	[self apply:sender];
}

- (void)takeEdgesFrom:(id)sender
{
	ldPreferences->setShowHighlightLines([self getBoolFrom:sender]);
	[[pages objectAtIndex:geometryIndex] setup];
	[self apply:sender];
}

- (void)takePrimSubFrom:(id)sender
{
	ldPreferences->setAllowPrimitiveSubstitution([self getBoolFrom:sender]);
	[[pages objectAtIndex:primitivesIndex] setup];
	[self apply:sender];
}

- (void)takeLightingFrom:(id)sender
{
	ldPreferences->setUseLighting([self getBoolFrom:sender]);
	[[pages objectAtIndex:effectsIndex] setup];
	[self apply:sender];
}

- (void)takeBfcFrom:(id)sender
{
	ldPreferences->setBfc([self getBoolFrom:sender]);
	[[pages objectAtIndex:geometryIndex] setup];
	[self apply:sender];
}

- (void)saveViewingAngle:(ModelWindow*)modelWindow
{
	ldPreferences->setModelViewer([[modelWindow modelView] modelViewer]);
	ldPreferences->saveDefaultView();
}

- (void)lightVectorChanged:(TCAlert *)alert
{
	[effectsPage lightVectorChanged:alert];
}

@end
