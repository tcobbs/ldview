//
//  PathOptionUI.mm
//  LDView
//
//  Created by Travis Cobbs on 6/17/08.
//  Copyright 2008 Travis Cobbs. All rights reserved.
//

#import "PathOptionUI.h"
#import "LDViewCategories.h"
#import "OCLocalStrings.h"

#include <LDExporter/LDExporterSetting.h>


@implementation PathOptionUI

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting
{
	self = [super initWithOptions:theOptions setting:theSetting];
	if (self != nil)
	{
		browseButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];
		[browseButton setTitle:[OCLocalStrings get:@"LDXBrowse..."]];
		[browseButton setBezelStyle:NSRoundedBezelStyle];
		[textField setNextKeyView:browseButton];
		[[browseButton cell] setRepresentedObject:self];
	}
	return self;
}

- (void)dealloc
{
	if (!shown)
	{
		[browseButton release];
	}
	[super dealloc];
}

- (float)textFieldWidthForWidth:(float)width
{
	return width - [browseButton frame].size.width - 6.0f;
}

- (float)row2Height
{
	return [browseButton frame].size.height;
}

- (float)updateLayoutX:(float)x y:(float)y width:(float)width update:(bool)update optimalWidth:(float &)optimalWidth
{
	bool origShown = shown;
	NSRect browseButtonFrame;
	NSRect textFieldFrame;

	// Note: must do sizeToFit before calling super, since it will call
	// textFieldWidthForWidth:, which requires browseButton to be the right
	// size.
	[browseButton sizeToFit];
	[super updateLayoutX:x y:y width:width update:update optimalWidth:optimalWidth];
	browseButtonFrame = [browseButton frame];
	textFieldFrame = [textField frame];
	browseButtonFrame.origin.x = x + width - browseButtonFrame.size.width;
	//browseButtonFrame.origin.y = textFieldFrame.origin.y;
	browseButtonFrame.origin.y = y + [label frame].size.height;
	if (update)
	{
		[browseButton setFrame:browseButtonFrame];
		if (!origShown)
		{
			[docView addSubview:browseButton];
			[browseButton release];
		}
	}
	return browseButtonFrame.origin.y + browseButtonFrame.size.height - y;
}

- (void)setEnabled:(BOOL)enabled
{
	[super setEnabled:enabled];
	[browseButton setEnabled:enabled];
}

- (NSRect)frame
{
	return NSUnionRect([super frame], [browseButton frame]);
}

- (NSView *)lastKeyView
{
	return browseButton;
}

@end
