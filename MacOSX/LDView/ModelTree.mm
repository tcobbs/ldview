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
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(modelChanged:) name:@"ModelLoaded" object:modelWindow];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(modelChanged:) name:@"ModelLoadCanceled" object:modelWindow];
		[NSBundle loadNibNamed:@"ModelTree.nib" owner:self];
	}
	return self;
}

- (void)dealloc
{
	[rootModelTreeItem release];
	TCObject::release(modelTree);
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[super dealloc];
}

- (void)setModel:(LDLMainModel *)value
{
	if (model != value)
	{
		TCObject::release(model);
		TCObject::release(modelTree);
		[rootModelTreeItem release];
		model = value;
		if (model)
		{
			model->retain();
			modelTree = new LDModelTree(model);
			rootModelTreeItem = [[ModelTreeItem alloc] initWithModelTree:modelTree];
		}
		else
		{
			modelTree = NULL;
			rootModelTreeItem = nil;
		}
	}
}

- (void)modelChanged
{
	[self setModel:[[modelWindow modelView] modelViewer]->getMainModel()];
	[outlineView reloadData];
}

- (void)awakeFromNib
{
	[drawer setParentWindow:[modelWindow window]];
	[self modelChanged];
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

- (void)modelChanged:(NSNotification *)notification
{
	if ([[notification name] isEqualToString:@"ModelLoaded"])
	{
		[self modelChanged];
	}
	else if ([[notification name] isEqualToString:@"ModelLoadCanceled"])
	{
		[self modelChanged];
	}
}

@end
