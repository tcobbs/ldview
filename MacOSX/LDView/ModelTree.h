//
//  ModelTree.h
//  LDView
//
//  Created by Travis Cobbs on 3/17/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "SideDrawer.h"

@class ModelTreeItem;

class LDModelTree;

@interface ModelTree : SideDrawer
{
	IBOutlet NSOutlineView *outlineView;
	IBOutlet NSTextField *optionsBoxLabel;
	IBOutlet NSBox *optionsBox;
	IBOutlet NSButton *showHideOptionsButton;
	IBOutlet NSMatrix *optionsMatrix;
	LDModelTree *modelTree;
	ModelTreeItem *rootModelTreeItem;
	float showHideStartY;
	NSAnimation *optionsAnimation;
	BOOL optionsShown;
}

- (void)modelChanged:(NSNotification *)notification;
- (void)hideOptionsInstantly:(BOOL)instantly;
- (void)hideOptions;
- (void)showOptions;
- (NSOutlineView *)outlineView;
- (BOOL)canCopy;
- (IBAction)copy:(id)sender;

@end
