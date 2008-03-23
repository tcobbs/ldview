//
//  LDViewCategories.mm
//  LDView
//
//  Created by Travis Cobbs on 3/22/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LDViewCategories.h"


@implementation NSButton(LDView)

- (void)setCheck:(bool)value
{
	[self setState:value ? NSOnState : NSOffState];
}

- (bool)getCheck
{
	return [self state] == NSOnState;
}

@end
