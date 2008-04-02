//
//  GenericSheet.mm
//  LDView
//
//  Created by Travis Cobbs on 7/29/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "GenericSheet.h"


@implementation GenericSheet

- (void)dealloc
{
	[panel release];
	[super dealloc];
}

- (id)initWithNibName:(NSString *)nibName
{
	if ((self = [super init]) != nil)
	{
		[NSBundle loadNibNamed:nibName owner:self];
	}
	return self;
}

- (int)runSheetInWindow:(NSWindow *)window;
{
	int modalResult;
	
	[[NSApplication sharedApplication] beginSheet:panel modalForWindow:window modalDelegate:self didEndSelector:nil contextInfo:NULL];
	modalResult = [[NSApplication sharedApplication] runModalForWindow:panel];
	[panel orderOut:self];
	return modalResult;
}

- (IBAction)cancel:(id)sender
{
	[[NSApplication sharedApplication] endSheet:panel];
	[[NSApplication sharedApplication] stopModalWithCode:NSCancelButton];
}

- (IBAction)ok:(id)sender
{
	[[NSApplication sharedApplication] endSheet:panel];
	[[NSApplication sharedApplication] stopModalWithCode:NSOKButton];
}

@end
