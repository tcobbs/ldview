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

- (float)updateLayoutX:(float)x y:(float)y width:(float)width update:(bool)update optimalWidth:(float &)optimalWidth
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

- (int)leftGroupMargin
{
	return leftGroupMargin;
}

- (int)rightGroupMargin
{
	return rightGroupMargin;
}

- (int)bottomGroupMargin
{
	return bottomGroupMargin;
}

- (NSString *)wrappedStringForString:(NSString *)string width:(float &)width height:(float &)height font:(NSFont *)font
{
	NSDictionary *attributes = [NSDictionary dictionaryWithObject:font forKey:NSFontAttributeName];
	NSSize size = [string sizeWithAttributes:attributes];

	if (size.width > width)
	{
		NSMutableString *retValue = [NSMutableString string];
		NSMutableString *remaining = [string mutableCopy];
		NSMutableString *line = nil;
		float maxWidth = 0.0f;

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

- (NSRect)calcControlLayout:(NSControl *)control value:(NSString *)value inRect:(NSRect)bounds optimalWidth:(float &)optimalWidth selector:(SEL)selector
{
	NSRect titleRect;
	float rightBorder;
	float leftBorder;
	
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
	bounds.size.width = titleRect.size.width + leftBorder + rightBorder;
	if (bounds.size.width > optimalWidth)
	{
		optimalWidth = bounds.size.width;
	}
	return bounds;
}

- (NSRect)calcControlLayout:(NSControl *)control inRect:(NSRect)bounds optimalWidth:(float &)optimalWidth selector:(SEL)selector
{
	return [self calcControlLayout:control value:[NSString stringWithUCString:setting->getName()] inRect:bounds optimalWidth:optimalWidth selector:selector];
}

- (NSRect)calcCheckLayout:(NSButton *)check inRect:(NSRect)bounds optimalWidth:(float &)optimalWidth
{
	return [self calcControlLayout:check inRect:bounds optimalWidth:optimalWidth selector:@selector(setTitle:)];
}

- (NSRect)calcLabelLayout:(NSTextField *)textField inRect:(NSRect)bounds optimalWidth:(float &)optimalWidth
{
	bounds = [self calcControlLayout:textField inRect:bounds optimalWidth:optimalWidth selector:@selector(setStringValue:)];
	// Don't ask me why, but it doesn't work without the following.
	bounds.size.width += 2.0f;
	return bounds;
}

- (NSButton *)createCheck
{
	NSButton *check = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];
	[check setButtonType:NSSwitchButton];
	[check setCheck:setting->getBoolValue()];
	[[check cell] setWraps:NO];
	return check;
}

- (NSTextField *)createLabel
{
	NSTextField *label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];
	[label setEnabled:NO];
	[label setSelectable:NO];
	[label setDrawsBackground:NO];
	[label setBezeled:NO];
	[label setBordered:NO];
	[[label cell] setWraps:NO];
	return label;
}

@end
