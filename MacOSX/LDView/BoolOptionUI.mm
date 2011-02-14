//
//  BoolOptionUI.mm
//  LDView
//
//  Created by Travis Cobbs on 6/15/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "BoolOptionUI.h"
#import "LDViewCategories.h"
#import "Options.h"

#include <LDExporter/LDExporterSetting.h>

@implementation BoolOptionUI

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting
{
	self = [super initWithOptions:theOptions setting:theSetting];
	if (self != nil)
	{
		check = [self createCheck];
		[check setTarget:self];
		[check setAction:@selector(checked:)];
		if (setting->getGroupSize() > 0)
		{
			leftGroupMargin = 6.0f;
		}
		[self addTooltip:check];
	}
	return self;
}

- (void)checked:(id)sender
{
	if (setting->getGroupSize() > 0)
	{
		[options updateEnabled];
	}
}

- (void)dealloc
{
	if (!shown)
	{
		[check release];
	}
	[super dealloc];
}

- (CGFloat)updateLayoutX:(CGFloat)x y:(CGFloat)y width:(CGFloat)width update:(bool)update optimalWidth:(CGFloat &)optimalWidth
{
	NSRect bounds = { { x, y }, { width, 1024.0f } };

	bounds = [self calcCheckLayout:check inRect:bounds optimalWidth:optimalWidth];
	if (update)
	{
		[check setFrame:bounds];
		if (!shown)
		{
			shown = true;
			[docView addSubview:check];
			[check release];
		}
	}
	return bounds.size.height;
}

- (void)commit
{
	setting->setValue([check getCheck], true);
}

- (void)setEnabled:(BOOL)enabled
{
	[check setEnabled:enabled];
}

- (NSRect)frame
{
	NSRect frame = [check frame];
	
	frame.origin.y -= 3.0f;
	frame.size.height += 6.0f;
	return frame;
}

- (NSView *)firstKeyView
{
	return check;
}

- (BOOL)groupEnabled
{
	if (setting->getGroupSize() > 0)
	{
		return [check getCheck];
	}
	else
	{
		return true;
	}
}

- (void)valueChanged
{
	[check setCheck:setting->getBoolValue()];
}

@end
