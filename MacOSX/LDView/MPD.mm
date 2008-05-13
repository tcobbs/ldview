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
				[modelNames addObject:[NSString stringWithASCIICString:models[i]->getName()]];
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
		[tableView selectRow:modelViewer->getMpdChildIndex() byExtendingSelection:NO];
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
	float width = [OCUserDefaults floatForKey:[self widthKey] defaultValue:-1.0f sessionSpecific:NO];

	modelNames = [[NSMutableArray alloc] init];
	[drawer setParentWindow:[modelWindow window]];
	if (width > [drawer contentSize].width)
	{
		[drawer setContentSize:NSMakeSize(width, [drawer contentSize].height)];
	}
	[self modelChanged];
}

- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return [modelNames count];
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)rowIndex
{
	return [modelNames objectAtIndex:rowIndex];
}

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
	LDrawModelViewer *modelViewer = [[modelWindow modelView] modelViewer];
	
	if (modelViewer)
	{
		modelViewer->setMpdChildIndex([tableView selectedRow]);
	}
}

@end
