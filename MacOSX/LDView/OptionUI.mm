//
//  OptionsUI.mm
//  LDView
//
//  Created by Travis Cobbs on 6/15/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "OptionUI.h"
#import "Options.h"
#import "LDViewCategories.h"

#include <LDExporter/LDExporterSetting.h>

@implementation OptionUI

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting
{
	self = [super init];
	if (self != nil)
	{
		options = theOptions;
		setting = &theSetting;
		docView = [theOptions docView];
	}
	return self;
}

- (CGFloat)updateLayoutX:(CGFloat)x y:(CGFloat)y width:(CGFloat)width update:(bool)update optimalWidth:(CGFloat &)optimalWidth
{
	// Must be implemented by subclasses.
	return 0;
}

- (void)commit
{
	// Must be implemented by subclasses.
}

- (void)setEnabled:(BOOL)enabled
{
	// Must be implemented by subclasses.
}

- (NSRect)frame
{
	// Must be implemented by subclasses.
	return NSMakeRect(0, 0, 0, 0);
}

- (bool)validate:(NSString *&)error
{
	return true;
}

- (LDExporterSetting *)setting
{
	return setting;
}

- (BOOL)groupEnabled
{
	return YES;
}

- (CGFloat)leftGroupMargin
{
	return leftGroupMargin;
}

- (CGFloat)rightGroupMargin
{
	return rightGroupMargin;
}

- (CGFloat)bottomGroupMargin
{
	return bottomGroupMargin;
}

- (NSString *)wrappedStringForString:(NSString *)string width:(CGFloat &)width height:(CGFloat &)height font:(NSFont *)font
{
	NSDictionary *attributes = [NSDictionary dictionaryWithObject:font forKey:NSFontAttributeName];
	NSSize size = [string sizeWithAttributes:attributes];

	if (size.width > width)
	{
		NSMutableString *retValue = [NSMutableString string];
		NSMutableString *remaining = [string mutableCopy];
		NSMutableString *line = nil;
		CGFloat maxWidth = 0.0f;

		height = 0.0f;
		while ([remaining length] > 0)
		{
			line = [remaining mutableCopy];
			size = [line sizeWithAttributes:attributes];
			while (size.width > width)
			{
				NSRange range = [line rangeOfString:@" " options:NSBackwardsSearch];

				if ([line length] == 0)
				{
					// We're SOL, but avoid an infinite loop.
					break;
				}
				if (range.location == NSNotFound)
				{
					range.location = [line length] - 1;
				}
				range.length = [line length] - range.location;
				[line deleteCharactersInRange:range];
				size = [line sizeWithAttributes:attributes];
			}
			if (size.width > maxWidth)
			{
				maxWidth = size.width;
			}
			height += size.height;
			if ([line length] > 0)
			{
				NSRange range = { 0, [line length] };
				int i;
				
				[remaining deleteCharactersInRange:range];
				for (i = 0; i < [remaining length] && isspace([remaining characterAtIndex:i]); i++)
				{
					// Don't do anything
				}
				if (i > 0)
				{
					range.length = i;
					[remaining deleteCharactersInRange:range];
				}
				[retValue appendString:line];
				if ([remaining length])
				{
					[retValue appendString:@"\n"];
				}
			}
			else
			{
				[remaining release];
				remaining = [@"" mutableCopy];
			}
			[line release];
		}
		[remaining release];
		width = maxWidth;
		return retValue;
	}
	else
	{
		width = size.width;
		height = size.height;
	}
	return string;
}

- (NSRect)calcControlLayout:(NSControl *)control value:(NSString *)value inRect:(NSRect)bounds optimalWidth:(CGFloat &)optimalWidth selector:(SEL)selector delta:(CGFloat)delta
{
	NSRect titleRect;
	CGFloat rightBorder;
	CGFloat leftBorder;

	bounds.size.width -= delta;
	[control performSelector:selector withObject:value];
	// Oddly enough, even though wrapped text doesn't work on button cells, the
	// following call returns a rect that behaves as if they do.  That's a good
	// thing, since that's really what we want.
	titleRect = [[control cell] titleRectForBounds:bounds];
	leftBorder = titleRect.origin.x - bounds.origin.x;
	rightBorder = bounds.origin.x + bounds.size.width - (titleRect.origin.x + titleRect.size.width);
	// Wrapped text doesn't work on button cells, so we have to calculate a new
	// title that has line feeds embedded in it.  That works.
	[control performSelector:selector withObject:[self wrappedStringForString:value width:titleRect.size.width height:titleRect.size.height font:[control font]]];
	bounds.size.height = titleRect.size.height;
	bounds.size.width = titleRect.size.width + leftBorder + rightBorder + delta + 2.0f;
	bounds = NSIntegralRect(bounds);
	if (bounds.size.width > optimalWidth)
	{
		optimalWidth = bounds.size.width;
	}
	return bounds;
}

- (NSRect)calcControlLayout:(NSControl *)control inRect:(NSRect)bounds optimalWidth:(CGFloat &)optimalWidth selector:(SEL)selector delta:(CGFloat)delta
{
	return [self calcControlLayout:control value:[NSString stringWithUCString:setting->getName()] inRect:bounds optimalWidth:optimalWidth selector:selector delta:delta];
}

- (NSRect)calcCheckLayout:(NSButton *)check inRect:(NSRect)bounds optimalWidth:(CGFloat &)optimalWidth
{
	return [self calcControlLayout:check inRect:bounds optimalWidth:optimalWidth selector:@selector(setTitle:) delta:0];
}

- (NSRect)calcLabelLayout:(NSTextField *)textField inRect:(NSRect)bounds optimalWidth:(CGFloat &)optimalWidth
{
	return [self calcLabelLayout:textField inRect:bounds optimalWidth:optimalWidth delta:0.0f];
}

- (NSRect)calcLabelLayout:(NSTextField *)textField inRect:(NSRect)bounds optimalWidth:(CGFloat &)optimalWidth delta:(CGFloat)delta
{
	bounds = [self calcControlLayout:textField inRect:bounds optimalWidth:optimalWidth selector:@selector(setStringValue:) delta:delta];
	// Don't ask me why, but it doesn't work without the following.
	bounds.size.width += 2.0f;
	return bounds;
}

- (NSButton *)newButton:(NSString *)title
{
	NSButton *button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];

	[button setTitle:title];
	[button setBezelStyle:NSRoundedBezelStyle];
	[[button cell] setRepresentedObject:self];
	[button setTarget:self];
	return button;
}

- (NSButton *)newCheck
{
	NSButton *check = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];
	[check setButtonType:NSSwitchButton];
	[check setCheck:setting->getBoolValue()];
	[[check cell] setWraps:NO];
	[[check cell] setRepresentedObject:self];
	return check;
}

- (NSTextField *)newLabel
{
	NSTextField *label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];
	[label setEnabled:NO];
	[label setSelectable:NO];
	[label setDrawsBackground:NO];
	[label setBezeled:NO];
	[label setBordered:NO];
	[[label cell] setWraps:NO];
	[[label cell] setRepresentedObject:self];
	return label;
}

- (NSView *)firstKeyView
{
	// Must be implemented by subclasses.
	return nil;
}

- (NSView *)lastKeyView
{
	// Most of these only have one key view.
	return [self firstKeyView];
}

- (void)addTooltip:(NSView *)view
{
	const ucstring &tooltip = setting->getTooltip();
	if (tooltip.size() > 0)
	{
		[view setToolTip:[NSString stringWithUCString:tooltip.c_str()]];
	}
}

- (void)valueChanged
{
}

- (void)reset
{
	setting->reset();
	[self valueChanged];
}

@end
