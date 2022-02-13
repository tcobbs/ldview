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
		[self finishInitWithNibName:nibName];
	}
	return self;
}

- (void)finishInitWithNibName:(NSString *)nibName
{
	[self ldvLoadNibNamed:nibName topLevelObjects:&topLevelObjects];
	[topLevelObjects retain];
}

- (void)beginSheetInWindow:(NSWindow *)window completionHandler:(void (^)(NSModalResponse returnCode))handler
{
	sheetWindow = window;
	[window beginSheet:panel completionHandler:^(NSModalResponse response){
		[panel orderOut:self];
		handler(response);
	}];
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
	[self stopModalWithCode:NSModalResponseCancel];
}

- (IBAction)ok:(id)sender
{
	[self stopModalWithCode:NSModalResponseOK];
}

- (void)stopModalWithCode:(NSModalResponse)returnCode
{
	if (sheetWindow)
	{
		[sheetWindow endSheet:panel returnCode:returnCode];
		sheetWindow = nil;
	}
	else
	{
		[[NSApplication sharedApplication] endSheet:panel];
		[[NSApplication sharedApplication] stopModalWithCode:returnCode];
	}
}

- (void)setField:(NSTextField *)textField toFloat:(double)value
{
	ucstring valueString = ftoucstr(value, 2);
	
	[textField setStringValue:[NSString stringWithUCString:valueString]];
}

@end
