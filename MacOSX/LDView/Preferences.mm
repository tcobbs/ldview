#import "Preferences.h"
#include "ModelWindow.h"
#include "LDrawModelView.h"
#include "PreferencePage.h"
#include "LDViewController.h"
#include <LDLib/LDPreferences.h>

@implementation Preferences

- (id)initWithController:(LDViewController *)value
{
	controller = value;	// Don't retain: it's our parent.
	pages = [[NSMutableArray alloc] initWithCapacity:[[tabView tabViewItems] count]];
	ldPreferences = new LDPreferences;
	ldPreferences->loadSettings();
	[NSBundle loadNibNamed:@"Preferences.nib" owner:self];
	return [super init];
}

- (void)dealloc
{
	TCObject::release(ldPreferences);
	[pages release];
	[super dealloc];
}

- (void)show
{
	[window makeKeyAndOrderFront:self];
}

- (void)awakeFromNib
{
	[pages addObject:generalPage];
	[pages addObject:geometryPage];
	[pages addObject:effectsPage];
	[pages addObject:primitivesPage];
	[pages addObject:updatesPage];
	[pages addObject:prefSetsPage];
	[pages makeObjectsPerformSelector:@selector(setPreferences:)
		withObject:self];
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

- (IBAction)apply:(id)sender
{
	NSArray *modelWindows = [controller modelWindows];

	[pages makeObjectsPerformSelector:@selector(apply)];
	for (int i = 0; i < [modelWindows count]; i++)
	{
		LDrawModelView *modelView = [[modelWindows objectAtIndex:i] modelView];
		ldPreferences->setModelViewer([modelView modelViewer]);
		ldPreferences->applySettings();
		ldPreferences->commitSettings();
		[modelView setNeedsDisplay:YES];
	}
}

- (IBAction)cancel:(id)sender
{
	[window orderOut:sender];
	[pages makeObjectsPerformSelector:@selector(setup)];
}

- (IBAction)ok:(id)sender
{
	[self apply:sender];
	[window orderOut:sender];
}

- (void)enableApply:(BOOL)enabled
{
	[applyButton setEnabled:enabled];
	[window setDocumentEdited:enabled];
}

- (NSWindow *)window
{
	return window;
}

@end
