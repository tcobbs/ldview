//
//  ViewOwnerBase.mm
//  LDView
//
//  Created by Travis Cobbs on 10/27/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "ViewOwnerBase.h"
#import "LDViewCategories.h"

@implementation ViewOwnerBase

+ (NSValue *)valueKey:(id)object
{
	return [NSValue valueWithPointer:object];
	//return [NSNumber numberWithUnsignedInt:(unsigned int)object];
}

- (void)dealloc
{
	[origColors release];
	[origBoxTitles release];
	[disabledBoxTitles release];
	[self releaseTopLevelObjects:topLevelObjects];
	[super dealloc];
}

- (void)awakeFromNib
{
	origColors = [[NSMutableDictionary alloc] init];
	origBoxTitles = [[NSMutableDictionary alloc] init];
	disabledBoxTitles = [[NSMutableDictionary alloc] init];
}

- (void)setCheck:(NSButton *)check value:(bool)value
{
	[check setState:value ? NSOnState : NSOffState];
}

- (bool)getCheck:(NSButton *)check
{
	return [check state] == NSOnState;
}

- (void)setColorWell:(NSColorWell *)colorWell r:(int)r g:(int)g b:(int)b
{
	[colorWell setColor:[NSColor colorWithCalibratedRed:r / 255.0 green:g / 255.0 blue:b / 255.0 alpha:1.0f]];
}

- (void)getColorWell:(NSColorWell *)colorWell r:(int *)r g:(int *)g b:(int *)b
{
	[self getColorWell:colorWell r:r g:g b:b a:nil];
}

- (void)getColorWell:(NSColorWell *)colorWell r:(int *)r g:(int *)g b:(int *)b a:(int *)a
{
	CGFloat rf, gf, bf, af;
	
	[[[colorWell color] colorUsingColorSpaceName:NSCalibratedRGBColorSpace] getRed:&rf green:&gf blue:&bf alpha:&af];
	*r = (int)(rf * 255.0 + 0.5);
	*g = (int)(gf * 255.0 + 0.5);
	*b = (int)(bf * 255.0 + 0.5);
	if (a)
	{
		*a = (int)(af * 255.0 + 0.5);
	}
}

- (void)enableLabel:(NSTextField *)label value:(BOOL)enabled
{
	if (enabled)
	{
		NSColor *color = [origColors objectForKey:[[self class] valueKey:label]];
		
		if (color)
		{
			[label setTextColor:color];
		}
	}
	else
	{
		if (![origColors objectForKey:[[self class] valueKey:label]])
		{
			[origColors setObject:[label textColor] forKey:[[self class] valueKey:label]];
		}
		[label setTextColor:[NSColor disabledControlTextColor]];
	}
}

- (void)enableBoxTitle:(NSBox *)box value:(BOOL)enabled
{
	NSCell *titleCell = [box titleCell];
	NSValue *valueKey = [[self class] valueKey:box];
	
	if (enabled)
	{
		NSAttributedString *boxTitle = [origBoxTitles objectForKey:valueKey];
		
		if (boxTitle)
		{
			[titleCell setAttributedStringValue:boxTitle];
		}
	}
	else
	{
		if (![origBoxTitles objectForKey:valueKey])
		{
			[origBoxTitles setObject:[titleCell attributedStringValue] forKey:valueKey];
		}
		if (![disabledBoxTitles objectForKey:valueKey])
		{
			NSMutableAttributedString *titleString = [[titleCell attributedStringValue] mutableCopy];
			NSRange range;
			NSMutableDictionary *attributes = [[titleString attributesAtIndex:0 effectiveRange:&range] mutableCopy];
			
			[attributes setObject:[NSColor disabledControlTextColor] forKey:NSForegroundColorAttributeName];
			[titleString setAttributes:[attributes autorelease] range:range];
			[disabledBoxTitles setObject:[titleString autorelease] forKey:valueKey];
		}
		[titleCell setAttributedStringValue:[disabledBoxTitles objectForKey:valueKey]];
	}
	[box setNeedsDisplay:YES];
}

- (void)groupCheck:(id)sender name:(NSString *)groupName value:(bool)value
{
	[self setCheck:sender value:value];
	[self groupCheck:sender name:groupName init:YES];
}

- (void)groupCheck:(id)sender name:(NSString *)groupName
{
	[self groupCheck:sender name:groupName init:NO];
}

- (void)groupCheck:(id)sender name:(NSString *)groupName init:(BOOL)init
{
	if (!init)
	{
		[self valueChanged:sender];
	}
	[self setUISection:groupName enabled:[self getCheck:sender]];
}

- (void)setUISection:(NSString *)sectionName enabled:(BOOL)enabled
{
	NSString *selectorName;
	
	if (enabled)
	{
		selectorName = [@"enable" stringByAppendingString:sectionName];
	}
	else
	{
		selectorName = [@"disable" stringByAppendingString:sectionName];
	}
	// Try doing THIS in C++!
	[self performSelector:NSSelectorFromString(selectorName)];
}

- (IBAction)valueChanged:(id)sender
{
}

@end
