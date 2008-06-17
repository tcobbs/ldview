//
//  GroupOptionUI.mm
//  LDView
//
//  Created by Travis Cobbs on 6/15/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "GroupOptionUI.h"
#import "LDViewCategories.h"
#import "Options.h"

#include <LDExporter/LDExporterSetting.h>


@implementation GroupOptionUI

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting
{
	self = [super initWithOptions:theOptions setting:theSetting];
	if (self != nil)
	{
		bool hasCheck = setting->getType() == LDExporterSetting::TBool;
		NSSize contentMargins;

		if (hasCheck)
		{
			check = [self createCheck];
			[check setTarget:self];
			[check setAction:@selector(checked:)];
		}
		else
		{
			label = [self createLabel];
		}
		box = [[NSBox alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];
		[box setTitlePosition:NSNoTitle];
		contentMargins = [box contentViewMargins];
		leftGroupMargin = rightGroupMargin = contentMargins.width + 6.0f;
		bottomGroupMargin = contentMargins.height + 6.0f;
	}
	return self;
}

- (void)checked:(id)sender
{
	[options updateEnabled];
}

- (void)dealloc
{
	if (!shown)
	{
		[check release];
		[label release];
		[box release];
	}
	[super dealloc];
}

- (float)updateLayoutX:(float)x y:(float)y width:(float)width update:(bool)update optimalWidth:(float &)optimalWidth
{
	NSRect labelBounds = { { x, y }, { width, 1024.0f } };
	NSRect boxBounds;
	float labelHeight;

	if (check != nil)
	{
		labelBounds = [self calcCheckLayout:check inRect:labelBounds optimalWidth:optimalWidth];
	}
	else
	{
		labelBounds = [self calcLabelLayout:label inRect:labelBounds optimalWidth:optimalWidth];
	}
	labelHeight = labelBounds.size.height;
	boxBounds = NSMakeRect(x, y + labelHeight, width, 6.0f);
	if (update)
	{
		if (check != nil)
		{
			[check setFrame:labelBounds];
		}
		else
		{
			[label setFrame:labelBounds];
		}
		[box setFrame:boxBounds];
		if (!shown)
		{
			shown = true;
			if (check != nil)
			{
				[docView addSubview:check];
				[check release];
			}
			if (label != nil)
			{
				[docView addSubview:label];
				[label release];
			}
			[docView addSubview:box];
			[box release];
		}
	}
	return labelHeight + boxBounds.size.height;
}

- (void)commit
{
	if (check != nil)
	{
		setting->setValue([check getCheck], true);
	}
}

- (void)setEnabled:(BOOL)enabled
{
	[check setEnabled:enabled];
	[label setEnabled:enabled];
}

- (NSRect)frame
{
	if (check != nil)
	{
		return [check frame];
	}
	else
	{
		return [label frame];
	}
}

- (BOOL)groupEnabled
{
	if (check != nil)
	{
		return [check getCheck] != false;
	}
	else
	{
		return YES;
	}
}

- (void)closeGroupAtY:(float)y
{
	NSRect boxRect = [box frame];
	
	boxRect.size.height = y + bottomGroupMargin - boxRect.origin.y;
	[box setFrame:boxRect];
}

@end
