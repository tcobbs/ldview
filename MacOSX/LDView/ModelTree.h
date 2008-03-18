//
//  ModelTree.h
//  LDView
//
//  Created by Travis Cobbs on 3/17/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class ModelWindow;
@class ModelTreeItem;

class LDModelTree;

@interface ModelTree : NSObject
{
	IBOutlet NSOutlineView *outlineView;
	IBOutlet NSDrawer *drawer;
	ModelWindow *modelWindow;
	LDModelTree *modelTree;
	ModelTreeItem *rootModelTreeItem;
}

- (id)initWithParent:(ModelWindow *)parent;
- (void)show;
- (void)hide;

@end
