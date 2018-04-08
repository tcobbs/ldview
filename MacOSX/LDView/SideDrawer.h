//
//  SideDrawer.h
//  LDView
//
//  Created by Travis Cobbs on 5/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class ModelWindow;

class LDLMainModel;

@interface SideDrawer : NSObject
{
    IBOutlet NSView *contentView;
    IBOutlet NSDrawer *drawer;

	ModelWindow *modelWindow;
	LDLMainModel *model;
	NSString *className;
	NSArray *topLevelObjects;
}

- (id)initWithParent:(ModelWindow *)parent;
- (void)open;
- (void)close;
- (void)toggle;
- (bool)isOpen;
- (NSString *)widthKey;

@end
