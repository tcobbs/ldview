//
//  NumberOptionUI.mm
//  LDView
//
//  Created by Travis Cobbs on 6/17/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "NumberOptionUI.h"
#import "LDViewCategories.h"

#include <LDExporter/LDExporterSetting.h>


@implementation NumberOptionUI

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting
{
	self = [super initWithOptions:theOptions setting:theSetting];
	if (self != nil)
	{
		NSRect frame;

		textFieldWidth = 72.0f;
		label = [self createLabel];
		[label setAlignment:NSRightTextAlignment];
		textField = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];
		[textField setStringValue:[NSString stringWithUCString:setting->getStringValue()]];
		[textField sizeToFit];
		frame = [textField frame];
		frame.size.width = textFieldWidth;
		[textField setFrame:frame];
	}
	return self;
}

- (void)dealloc
{
	if (!shown)
	{
		[label release];
	}
	[super dealloc];
}

- (float)updateLayoutX:(float)x y:(float)y width:(float)width update:(bool)update optimalWidth:(float &)optimalWidth
{
	float delta = textFieldWidth + 6.0f;
	NSRect labelBounds = { { x, y }, { width, 1024.0f } };
	NSRect textFieldBounds = [textField frame];

	labelBounds = [self calcLabelLayout:label inRect:labelBounds optimalWidth:optimalWidth delta:delta];
	if (update)
	{
		
		textFieldBounds.origin.x = x + optimalWidth - textFieldWidth;
		textFieldBounds.origin.y = y;
		labelBounds.origin.y += (float)(int)((textFieldBounds.size.height - labelBounds.size.height) / 2.0f);
		labelBounds.size.width = optimalWidth - textFieldWidth - 6.0f;
		[label setFrame:labelBounds];
		[textField setFrame:textFieldBounds];
		if (!shown)
		{
			shown = true;
			[docView addSubview:label];
			[docView addSubview:textField];
			[label release];
		}
	}
	return textFieldBounds.size.height;
}

- (void)setEnabled:(BOOL)enabled
{
	[label setEnabled:enabled];
}

- (NSRect)frame
{
	return [label frame];
}

- (NSView *)firstKeyView
{
	return label;
}

@end
