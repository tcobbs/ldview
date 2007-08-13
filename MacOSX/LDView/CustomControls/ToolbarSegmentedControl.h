//
//  ToolbarSegmentedControl.h
//  LDView
//
//  Created by Travis Cobbs on 8/12/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface ToolbarSegmentedControl : NSSegmentedControl {

}

+ (Class)cellClass;
- (id)initWithTemplate:(id)other;

@end
