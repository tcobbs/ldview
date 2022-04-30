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
#import "OCLocalStrings.h"

#include <LDExporter/LDExporterSetting.h>


@implementation GroupOptionUI

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting spacing:(CGFloat)theSpacing
{
	spacing = theSpacing;
	self = [super initWithOptions:theOptions setting:theSetting];
	if (self != nil)
	{
		bool hasCheck = setting->getType() == LDExporterSetting::TBool;
		NSSize contentMargins;

		if (hasCheck)
		{
			check = [self newCheck];
			[check setTarget:self];
			[check setAction:@selector(checked:)];
		}
		else
		{
			label = [self newLabel];
		}
		box = [[NSBox alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];
		[box setTitlePosition:NSNoTitle];
		contentMargins = [box contentViewMargins];
		leftGroupMargin = rightGroupMargin = contentMargins.width + 6.0f;
		bottomGroupMargin = contentMargins.height + 6.0f;
		resetButton = [self newButton:[OCLocalStrings get:@"LDXResetGroup"]];
		[resetButton setAction:@selector(reset:)];
	}
	return self;
}

- (void)checked:(id)sender
{
	[options updateEnabled];
//	if (check != nil)
//	{
//		[check setCheck:setting->getBoolValue()];
//	}
}

- (void)dealloc
{
	if (!shown)
	{
		[check release];
		[label release];
		[box release];
		[resetButton release];
	}
	[super dealloc];
}

- (CGFloat)updateLayoutX:(CGFloat)x y:(CGFloat)y width:(CGFloat)width update:(bool)update optimalWidth:(CGFloat &)optimalWidth
{
	NSRect labelBounds = { { x, y }, { width, 1024.0f } };
	NSRect boxBounds;
	CGFloat labelHeight;

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
			[docView addSubview:resetButton];
			[resetButton release];
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

- (void)closeGroupAtY:(CGFloat)y
{
	NSRect boxRect = [box frame];
	NSRect resetRect;

	[resetButton sizeToFit];
	resetRect = [resetButton frame];
	resetRect.origin.x = boxRect.origin.x + boxRect.size.width - resetRect.size.width - rightGroupMargin;
	resetRect.origin.y = y;
	[resetButton setFrame:resetRect];
	boxRect.size.height = y + bottomGroupMargin + [[resetButton cell] cellSizeForBounds:resetRect].height + spacing - boxRect.origin.y;
	[box setFrame:boxRect];
}

- (NSView *)firstKeyView
{
	if (check != nil)
	{
		return check;
	}
	else
	{
		return label;
	}
}

- (CGFloat)bottomGroupMargin
{
	return bottomGroupMargin + spacing + [[resetButton cell] cellSizeForBounds:[resetButton frame]].height;
}

- (void)reset
{
	[options updateEnabled];
}

- (void)reset:(id)sender
{
	NSArray *views = [docView subviews];
	NSRect boxRect = [box frame];

	for (int i = 0; i < [views count]; i++)
	{
		id view = [views objectAtIndex:i];

		if ([view respondsToSelector:@selector(cell)])
		{
			NSCell *cell = [view cell];
			NSRect viewRect = [view frame];
			
			if (viewRect.origin.y >= boxRect.origin.y && viewRect.origin.y <= boxRect.origin.y + boxRect.size.height && [cell representedObject])
			{
				[[cell representedObject] reset];
			}
		}
	}
	[options updateEnabled];
}

@end
