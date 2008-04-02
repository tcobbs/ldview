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
class LDLMainModel;

@interface ModelTree : NSObject
{
	IBOutlet NSOutlineView *outlineView;
	IBOutlet NSDrawer *drawer;
	IBOutlet NSTextField *optionsBoxLabel;
	IBOutlet NSBox *optionsBox;
	IBOutlet NSButton *showHideOptionsButton;
	IBOutlet NSView *contentView;
	IBOutlet NSMatrix *optionsMatrix;
	ModelWindow *modelWindow;
	LDModelTree *modelTree;
	ModelTreeItem *rootModelTreeItem;
	LDLMainModel *model;
	float showHideStartY;
	NSAnimation *optionsAnimation;
	BOOL optionsShown;
}

- (void)modelChanged:(NSNotification *)notification;
- (id)initWithParent:(ModelWindow *)parent;
- (void)open;
- (void)close;
- (void)toggle;
- (bool)isOpen;
- (void)hideOptionsInstantly:(BOOL)instantly;
- (void)hideOptions;
- (void)showOptions;

@end
