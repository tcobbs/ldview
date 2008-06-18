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
		label = [self createLabel];
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
	NSRect bounds = { { x, y }, { width, 1024.0f } };
	
	bounds = [self calcLabelLayout:label inRect:bounds optimalWidth:optimalWidth];
	if (update)
	{
		[label setFrame:bounds];
		if (!shown)
		{
			shown = true;
			[docView addSubview:label];
			[docView setNeedsDisplay:YES];
			[label release];
		}
	}
	return bounds.size.height;
}

- (void)commit
{
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
