//
//  MPD.mm
//  LDView
//
//  Created by Travis Cobbs on 5/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MPD.h"
#import "ModelWindow.h"
#import "LDrawModelView.h"
#import "LDViewCategories.h"
#import "OCUserDefaults.h"

#include <LDLib/LDModelTree.h>
#include <LDLib/LDrawModelViewer.h>
#include <LDLoader/LDLMainModel.h>

@implementation MPD

- (void)dealloc
{
	[modelNames release];
	[super dealloc];
}

- (void)setModel:(LDLMainModel *)value
{
	if (model != value)
	{
		TCObject::release(model);
		[modelNames removeAllObjects];
		model = value;
		if (model)
		{
			LDLModelVector &models = model->getMpdModels();
			size_t i;

			model->retain();
			for (i = 0; i < models.size(); i++)
			{
				[modelNames addObject:[NSString stringWithUTF8String:models[i]->getName()]];
			}
		}
	}
}

- (void)reloadTableView
{
	[tableView reloadData];
	LDrawModelViewer *modelViewer = [[modelWindow modelView] modelViewer];

	if (modelViewer)
	{
		[tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:modelViewer->getMpdChildIndex()] byExtendingSelection:NO];
	}
}

- (void)modelChanged
{
	[self setModel:[[modelWindow modelView] modelViewer]->getMainModel()];
	[self reloadTableView];
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

- (void)awakeFromNib
{
	modelNames = [[NSMutableArray alloc] init];
	[self modelChanged];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return [modelNames count];
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
	return [modelNames objectAtIndex:rowIndex];
}

- (void)setMpdChildIndex:(int)index
{
	LDrawModelViewer *modelViewer = [[modelWindow modelView] modelViewer];
	
	if (modelViewer)
	{
		modelViewer->setMpdChildIndex(index);
	}
}

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
	[self setMpdChildIndex:(int)[tableView selectedRow]];
}

- (void)close
{
	[super close];
	[self setMpdChildIndex:0];
	if ([modelNames count] > 0)
	{
		[tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:0] byExtendingSelection:NO];
	}
}

- (BOOL)isKeyWindow
{
	return [panel isKeyWindow];
}

@end
