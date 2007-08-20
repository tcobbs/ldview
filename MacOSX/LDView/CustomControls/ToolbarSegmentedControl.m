//
//  ToolbarSegmentedControl.m
//  LDView
//
//  Created by Travis Cobbs on 8/12/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "ToolbarSegmentedControl.h"
#import "ToolbarSegmentedCell.h"

@implementation ToolbarSegmentedControl

+ (void)initialize
{
	[[self class] setCellClass:[ToolbarSegmentedCell class]];
}

+ (Class)cellClass
{
	return [ToolbarSegmentedCell class];
}

- (id)initWithTemplate:(id)other
{
	if ((self = [super initWithFrame:[other frame]]) != nil)
	{
		int segmentCount = [other segmentCount];
		int i;
		NSSegmentedCell *otherCell = [other cell];
		ToolbarSegmentedCell *cell;

		cell = [self cell];
		[self setSegmentCount:segmentCount];
		[self setTarget:[other target]];
		[self setAction:[other action]];
		[self setEnabled:[other isEnabled]];
		for (i = 0; i < segmentCount; i++)
		{
			[self setWidth:[other widthForSegment:i] forSegment:i];
			[self setImage:[other imageForSegment:i] forSegment:i];
			[self setLabel:[other labelForSegment:i] forSegment:i];
			[self setMenu:[other menuForSegment:i] forSegment:i];
			[self setSelected:[other isSelectedForSegment:i] forSegment:i];
			[self setEnabled:[other isEnabledForSegment:i] forSegment:i];
			[cell setTag:[otherCell tagForSegment:i] forSegment:i];
			[cell setTrackingMode:[otherCell trackingMode]];
		}
	}
	return self;
}

@end
