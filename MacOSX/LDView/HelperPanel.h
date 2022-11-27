//
//  HelperPanel.h
//  LDView
//
//  Created by Travis Cobbs on 5/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class ModelWindow;

class LDLMainModel;

@interface HelperPanel : NSObject
{
	IBOutlet NSPanel *panel;

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

@end
