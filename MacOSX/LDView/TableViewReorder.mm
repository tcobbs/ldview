//
//  TableViewReorder.mm
//  LDView
//
//  Created by Travis Cobbs on 3/23/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "TableViewReorder.h"


@implementation TableViewReorder

- (id)initWithTableView:(NSTableView *)theTableView owner:(id<TableViewReorder>)theOwner dragType:(NSString *)theDragType
{
	self = [super init];
	if (self != nil)
	{
		tableView = theTableView;
		owner = theOwner;
		dragType = [theDragType retain];
		[tableView registerForDraggedTypes:[NSArray arrayWithObject:dragType]];
		tableViewDataSource = [tableView dataSource];
		if (tableViewDataSource == nil)
		{
			tableViewDataSource = owner;
		}
		[tableView setDataSource:self];
	}
	return self;
}

- (void)dealloc
{
	[tableView setDataSource:tableViewDataSource];
	[dragType release];
	[super dealloc];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return [[owner tableRows:self] count];
}

- (NSArray *)tableView:(NSTableView *)aTableView namesOfPromisedFilesDroppedAtDestination:(NSURL *)dropDestination forDraggedRowsWithIndexes:(NSIndexSet *)indexSet
{
	if ([tableViewDataSource respondsToSelector:@selector(tableView:namesOfPromisedFilesDroppedAtDestination:forDraggedRowsWithIndexes:)])
	{
		return [tableViewDataSource tableView:aTableView namesOfPromisedFilesDroppedAtDestination:dropDestination forDraggedRowsWithIndexes:indexSet];
	}
	else
	{
		return @[];
	}
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
	if ([tableViewDataSource respondsToSelector:@selector(tableView:objectValueForTableColumn:row:)])
	{
		return [tableViewDataSource tableView:aTableView objectValueForTableColumn:aTableColumn row:rowIndex];
	}
	else
	{
		return nil;
	}
}

- (void)tableView:(NSTableView *)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
	if ([tableViewDataSource respondsToSelector:@selector(tableView:setObjectValue:forTableColumn:row:)])
	{
		[tableViewDataSource tableView:aTableView setObjectValue:anObject forTableColumn:aTableColumn row:rowIndex];
	}
}

- (void)tableView:(NSTableView *)aTableView sortDescriptorsDidChange:(NSArray *)oldDescriptors
{
	if ([tableViewDataSource respondsToSelector:@selector(tableView:sortDescriptorsDidChange:)])
	{
		return [tableViewDataSource tableView:aTableView sortDescriptorsDidChange:oldDescriptors];
	}
}

- (BOOL)tableView:(NSTableView *)tv writeRowsWithIndexes:(NSIndexSet *)rowIndexes toPasteboard:(NSPasteboard*)pboard
{
	if ([rowIndexes count] == 1)
	{
		// Copy the row to the pasteboard.
		NSInteger row = [rowIndexes firstIndex];
		NSDictionary *rowDict = [NSDictionary dictionaryWithObjectsAndKeys:[[owner tableRows:self] objectAtIndex:row], @"Object", [NSNumber numberWithInteger:row], @"OldRow", nil];
        if (@available(macOS 10.11, *))
        {
            NSData *data = [NSKeyedArchiver archivedDataWithRootObject:rowDict];
            [pboard declareTypes:[NSArray arrayWithObject:dragType] owner:self];
            [pboard setData:data forType:dragType];
        }
        else
        {
            NSBeep();
        }
		return YES;
	}
    return NO;
}

- (NSDragOperation)tableView:(NSTableView*)tv validateDrop:(id <NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)op
{
	if ([[info draggingPasteboard] dataForType:dragType])
	{
		if (op == NSTableViewDropOn)
		{
			[tableView setDropRow:row dropOperation:NSTableViewDropAbove];
		}
		return NSDragOperationMove;
	}
	else
	{
		return NSDragOperationNone;
	}
}

- (BOOL)tableView:(NSTableView *)aTableView acceptDrop:(id <NSDraggingInfo>)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)operation
{
	NSPasteboard* pboard = [info draggingPasteboard];
	NSData* rowData = [pboard dataForType:dragType];
	NSDictionary *rowDict = [NSKeyedUnarchiver unarchiveObjectWithData:rowData];
	int oldRow = [[rowDict objectForKey:@"OldRow"] intValue];
	NSMutableArray *rows = [owner tableRows:self];
	
	[rows removeObjectAtIndex:oldRow];
	if (oldRow < row)
	{
		row--;
	}
	[rows insertObject:[rowDict objectForKey:@"Object"] atIndex:row];
	[tableView reloadData];
	id idOwner = owner;
	if ([idOwner respondsToSelector:@selector(tableViewReorderDidOccur:)])
	{
		[idOwner tableViewReorderDidOccur:self];
	}
	return YES;
}

- (IBAction)copy:(id)sender
{
	NSLog(@"copy:\n");
}

@end
