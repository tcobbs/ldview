//
//  BoolOptionUI.mm
//  LDView
//
//  Created by Travis Cobbs on 6/15/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "BoolOptionUI.h"
#import "LDViewCategories.h"

#include <LDExporter/LDExporterSetting.h>

@implementation BoolOptionUI

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting
{
	self = [super initWithOptions:theOptions setting:theSetting];
	if (self != nil)
	{
		check = [self createCheck];
	}
	return self;
}

- (void)dealloc
{
	if (!shown)
	{
		[check release];
	}
	[super dealloc];
}

- (float)updateLayoutX:(float)x y:(float)y width:(float)width update:(bool)update optimalWidth:(float &)optimalWidth
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
	return [check frame];
}

- (NSView *)firstKeyView
{
	return check;
}

@end
