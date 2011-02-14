//
//  EnumOptionUI.mm
//  LDView
//
//  Created by Travis Cobbs on 6/16/08.
//  Copyright 2008 Travis Cobbs. All rights reserved.
//

#import "EnumOptionUI.h"
#import "LDViewCategories.h"
#import "Options.h"

#include <LDExporter/LDExporterSetting.h>


@implementation EnumOptionUI

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting
{
	self = [super initWithOptions:theOptions setting:theSetting];
	if (self != nil)
	{
		const UCStringVector &values = setting->getOptions();
		size_t i;

		label = [self createLabel];
		popUpButton = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];
		for (i = 0; i < values.size(); i++)
		{
			[popUpButton addItemWithTitle:[NSString stringWithUCString:values[i]]];
		}
		[popUpButton selectItemAtIndex:setting->getSelectedOption()];
		[[popUpButton itemAtIndex:0] setRepresentedObject:self];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willPopUp:) name:NSPopUpButtonWillPopUpNotification object:popUpButton];
		[self addTooltip:popUpButton];
	}
	return self;
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	if (!shown)
	{
		[label release];
		[popUpButton release];
	}
	[super dealloc];
}

- (void)willPopUp:(NSNotification *)notification
{
	[options makeOptionUIVisible:self];
}

- (CGFloat)updateLayoutX:(CGFloat)x y:(CGFloat)y width:(CGFloat)width update:(bool)update optimalWidth:(CGFloat &)optimalWidth
{
	NSRect labelFrame = { { x, y }, { width, 1024.0f } };
	NSRect popUpFrame;

	labelFrame = [self calcLabelLayout:label inRect:labelFrame optimalWidth:optimalWidth];
	[popUpButton sizeToFit];
	popUpFrame = [popUpButton frame];
	popUpFrame.origin.x = x;
	popUpFrame.origin.y = y + labelFrame.size.height;
	popUpFrame.size.width = width;
	if (update)
	{
		[label setFrame:labelFrame];
		[popUpButton setFrame:popUpFrame];
		if (!shown)
		{
			shown = true;
			[docView addSubview:label];
			[label release];
			[docView addSubview:popUpButton];
			[popUpButton release];
		}
	}
	return popUpFrame.origin.y + popUpFrame.size.height - labelFrame.origin.y;
}

- (void)commit
{
	setting->selectOption([popUpButton indexOfSelectedItem], true);
}

- (void)setEnabled:(BOOL)enabled
{
	[label setEnabled:enabled];
	[popUpButton setEnabled:enabled];
}

- (NSRect)frame
{
	return NSUnionRect([label frame], [popUpButton frame]);
}

- (NSView *)firstKeyView
{
	return popUpButton;
}

- (void)valueChanged
{
	[popUpButton selectItemAtIndex:setting->getSelectedOption()];
}

@end
