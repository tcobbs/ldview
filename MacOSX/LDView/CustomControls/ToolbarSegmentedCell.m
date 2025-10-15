//
//  ToolbarSegmentedCell.m
//  LDView
//
//  Created by Travis Cobbs on 8/12/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "ToolbarSegmentedCell.h"


@implementation ToolbarSegmentedCell

- (void)setControlSize:(NSControlSize)size
{
}

- (BOOL)needsPadding
{
	if (@available(macOS 26.0, *))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

- (NSControlSize)controlSize
{
	if (@available(macOS 11.0, *))
	{
		return NSControlSizeSmall;
	}
	else
	{
		// Fallback on earlier versions
		return NSControlSizeRegular;
	}
}

- (NSSize)cellSizeForBounds:(NSRect)bounds
{
	NSSize result = [super cellSizeForBounds:bounds];
	NSWindow *window = [[self controlView] window];
	if ([self needsPadding] && [[window toolbar] displayMode] != NSToolbarDisplayModeIconAndLabel)
	{
		BOOL toolbarIsCompact = NO;
		if (@available(macOS 11.0, *))
		{
			toolbarIsCompact = [window toolbarStyle] == NSWindowToolbarStyleUnifiedCompact;
		}
		if (!toolbarIsCompact)
		{
			// When the toolbar isn't in compact mode, it looks really strange
			// with the round buttons pushed up against the edges of the round
			// toolbar item that they are contained in since that contain's
			// radius is much larger than the buttons. This adds 6 points of
			// space on both sides to make things look natural.
			result.width += 12.0;
		}
	}
	return result;
}

@end
