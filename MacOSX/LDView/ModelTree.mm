//
//  ModelTree.mm
//  LDView
//
//  Created by Travis Cobbs on 3/17/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "ModelTree.h"
#import "ModelWindow.h"
#import "LDrawModelView.h"
#import "ModelTreeItem.h"

#include <LDLib/LDModelTree.h>
#include <LDLib/LDrawModelViewer.h>
#include <LDLoader/LDLMainModel.h>

@implementation ModelTree

- (id) initWithParent:(ModelWindow *)parent
{
	self = [super init];
	if (self != nil)
	{
		modelWindow = parent;	// Don't retain; we're a child.
		[NSBundle loadNibNamed:@"ModelTree.nib" owner:self];
	}
	return self;
}

- (void)dealloc
{
	[rootModelTreeItem release];
	TCObject::release(modelTree);
	[super dealloc];
}

- (void)awakeFromNib
{
	[drawer setParentWindow:[modelWindow window]];
	modelTree = new LDModelTree([[modelWindow modelView] modelViewer]->getMainModel());
	rootModelTreeItem = [[ModelTreeItem alloc] initWithModelTree:modelTree];
	[outlineView reloadData];
}

- (void)show
{
	[drawer open];
}

- (void)hide
{
	[drawer close];
}

- (ModelTreeItem *)modelTreeItem:(id)item
{
	if (item == nil)
	{
		return rootModelTreeItem;
	}
	else
	{
		return item;
	}
}

// NSOutlineView Data Source methods

- (int)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	return [[self modelTreeItem:item] numberOfChildren];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
	return [[self modelTreeItem:item] numberOfChildren] > 0;
}

- (id)outlineView:(NSOutlineView *)outlineView child:(int)index ofItem:(id)item
{
	return [[self modelTreeItem:item] childAtIndex:index];
}

- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
	return [[self modelTreeItem:item] objectValue];
}

@end
