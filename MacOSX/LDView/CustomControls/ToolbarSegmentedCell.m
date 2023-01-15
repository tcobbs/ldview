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

- (NSControlSize)controlSize
{
	if (@available(macOS 11.0, *)) {
		return NSControlSizeSmall;
	} else {
		// Fallback on earlier versions
		return NSControlSizeRegular;
	}
}

@end
