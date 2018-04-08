//
//  GenericSheet.mm
//  LDView
//
//  Created by Travis Cobbs on 7/29/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "GenericSheet.h"
#import "LDViewCategories.h"

@implementation GenericSheet

- (void)dealloc
{
	[self releaseTopLevelObjects:topLevelObjects orTopLevelObject:panel];
	[super dealloc];
}

- (id)initWithNibName:(NSString *)nibName
{
	if ((self = [super init]) != nil)
	{
		[self ldvLoadNibNamed:nibName topLevelObjects:&topLevelObjects];
		[topLevelObjects retain];
	}
	return self;
}

- (NSInteger)runSheetInWindow:(NSWindow *)window;
{
	NSInteger modalResult;
	
	[[NSApplication sharedApplication] beginSheet:panel modalForWindow:window modalDelegate:self didEndSelector:nil contextInfo:NULL];
	modalResult = [[NSApplication sharedApplication] runModalForWindow:panel];
	[panel orderOut:self];
	return modalResult;
}

- (IBAction)cancel:(id)sender
{
	[[NSApplication sharedApplication] endSheet:panel];
	[[NSApplication sharedApplication] stopModalWithCode:NSModalResponseCancel];
}

- (IBAction)ok:(id)sender
{
	[[NSApplication sharedApplication] endSheet:panel];
	[[NSApplication sharedApplication] stopModalWithCode:NSModalResponseOK];
}

@end
