//
//  StringOptionUI.mm
//  LDView
//
//  Created by Travis Cobbs on 6/16/08.
//  Copyright 2008 Travis Cobbs. All rights reserved.
//

#import "StringOptionUI.h"
#import "LDViewCategories.h"

#include <LDExporter/LDExporterSetting.h>


@implementation StringOptionUI

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting
{
	self = [super initWithOptions:theOptions setting:theSetting];
	if (self != nil)
	{
		label = [self createLabel];
		textField = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];
		[textField setStringValue:[NSString stringWithUCString:setting->getStringValue()]];
		[[textField cell] setRepresentedObject:self];
		[self addTooltip:textField];
	}
	return self;
}

- (void)dealloc
{
	if (!shown)
	{
		[label release];
		[textField release];
	}
	[super dealloc];
}

- (CGFloat)textFieldWidthForWidth:(CGFloat)width
{
	return width;
}

- (CGFloat)row2Height
{
	return [textField frame].size.height;
}

- (CGFloat)updateLayoutX:(CGFloat)x y:(CGFloat)y width:(CGFloat)width update:(bool)update optimalWidth:(CGFloat &)optimalWidth
{
	NSRect labelFrame = { { x, y }, { width, 1024.0f } };
	NSRect textFieldFrame;
	int textFieldMargin;

	labelFrame = [self calcLabelLayout:label inRect:labelFrame optimalWidth:optimalWidth];
	[textField sizeToFit];
	textFieldFrame = [textField frame];
	textFieldFrame.origin.x = x;
	textFieldMargin = (int)(([self row2Height] - textFieldFrame.size.height) / 2.0f);
	textFieldFrame.origin.y = y + labelFrame.size.height + (CGFloat)textFieldMargin;
	textFieldFrame.size.width = [self textFieldWidthForWidth:width];
	if (update)
	{
		[label setFrame:labelFrame];
		[textField setFrame:textFieldFrame];
		if (!shown)
		{
			shown = true;
			[docView addSubview:label];
			[label release];
			[docView addSubview:textField];
			[textField release];
		}
	}
	return textFieldFrame.origin.y + textFieldFrame.size.height - y;
}

- (void)commit
{
	setting->setValue([[textField stringValue] ucString].c_str(), true);
}

- (void)setEnabled:(BOOL)enabled
{
	[label setEnabled:enabled];
	[textField setEnabled:enabled];
}

- (NSRect)frame
{
	return NSUnionRect([label frame], [textField frame]);
}

- (NSView *)firstKeyView
{
	return textField;
}

@end
