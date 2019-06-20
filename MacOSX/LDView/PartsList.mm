#import "PartsList.h"
#import "LDViewCategories.h"
#include <LDLib/LDHtmlInventory.h>

#define DragType @"LDViewPartsListDragType"

@implementation PartsList

- (NSMutableDictionary *)columnDictWithID:(LDPartListColumn)theId state:(BOOL)state
{
	return [NSMutableDictionary dictionaryWithObjectsAndKeys:[NSString stringWithUCString:LDHtmlInventory::getColumnNameUC(theId)], @"Name", [NSNumber numberWithBool:state], @"State", [NSNumber numberWithInt:theId], @"ID", nil];
}

- (id)initWithModelWindow:(ModelWindow *)theModelWindow htmlInventory:(LDHtmlInventory *)theHtmlInventory
{
	if ((self = [super init]) != nil)
	{
		int i;

		modelWindow = theModelWindow;		// Don't retain
		htmlInventory = theHtmlInventory;	// Don't retain
		const LDPartListColumnVector &columnOrder = htmlInventory->getColumnOrder();
		columns = [[NSMutableArray alloc] init];
		for (i = 0; i < columnOrder.size(); i++)
		{
			LDPartListColumn column = columnOrder[i];

			[columns addObject:[self columnDictWithID:column state:YES]];
		}
		for (int i = LDPLCFirst; i <= LDPLCLast; i++)
		{
			LDPartListColumn column = (LDPartListColumn)i;

			if (!htmlInventory->isColumnEnabled(column))
			{
				[columns addObject:[self columnDictWithID:column state:NO]];
			}
		}
		[self ldvLoadNibNamed:@"PartsList" topLevelObjects:&topLevelObjects];
		[topLevelObjects retain];
	}
	return self;
}

- (void)dealloc
{
	[columns release];
	[super dealloc];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return [columns count];
}

- (LDPartListColumn)columnAtIndex:(int)index
{
	return (LDPartListColumn)[[columns objectAtIndex:index] intValue];
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
	if ([[aTableColumn identifier] isEqualToString:@"Checks"])
	{
		return @"";
	}
	else
	{
		return [[columns objectAtIndex:rowIndex] objectForKey:@"Name"];
	}
}

- (void)tableView:(NSTableView *)aTableView willDisplayCell:(id)aCell forTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
	if ([[aTableColumn identifier] isEqualToString:@"Checks"])
	{
		[aCell setState:[[[columns objectAtIndex:rowIndex] objectForKey:@"State"] boolValue] ? NSOnState : NSOffState];
	}
}

- (BOOL)tableView:(NSTableView *)tv writeRowsWithIndexes:(NSIndexSet *)rowIndexes toPasteboard:(NSPasteboard*)pboard
{
	if ([rowIndexes count] == 1)
	{
		// Copy the row to the pasteboard.
		NSMutableDictionary *rowDict = [columns objectAtIndex:[rowIndexes firstIndex]];
        if (@available(macOS 10.11, *))
        {
            NSData *data = [NSKeyedArchiver archivedDataWithRootObject:rowDict];
            [pboard declareTypes:[NSArray arrayWithObject:DragType] owner:self];
            [pboard setData:data forType:DragType];
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
	if ([[info draggingPasteboard] dataForType:DragType])
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
	NSData* rowData = [pboard dataForType:DragType];
	NSMutableDictionary *rowDict = [NSKeyedUnarchiver unarchiveObjectWithData:rowData];
	NSString *oldColumnName = [rowDict objectForKey:@"Name"];

	for (int i = 0; i < [columns count]; i++)
	{
		if ([[[columns objectAtIndex:i] objectForKey:@"Name"] isEqualToString:oldColumnName])
		{
			[columns removeObjectAtIndex:i];
			if (i < row)
			{
				row--;
			}
			[columns insertObject:rowDict atIndex:row];
			[tableView reloadData];
			return YES;
		}
	}
	return NO;
}

- (void)awakeFromNib
{
	[showModelButton setCheck:htmlInventory->getShowModelFlag()];
	[externalStyleButton setCheck:htmlInventory->getExternalCssFlag()];
	[showImagesButton setCheck:htmlInventory->getPartImagesFlag()];
	[showWebPageButton setCheck:htmlInventory->getShowFileFlag()];
	[tableView registerForDraggedTypes:[NSArray arrayWithObject:DragType]];
}

- (IBAction)columnSelected:(id)sender
{
	NSInteger column = [tableView clickedColumn];
	if (column >= 0 && [[[[tableView tableColumns] objectAtIndex:column] identifier] isEqualToString:@"Checks"])
	{
		NSInteger row = [tableView clickedRow];
		
		if (row >= 0)
		{
			NSMutableDictionary *columnDict = [columns objectAtIndex:row];
			BOOL newChecked = ![[columnDict objectForKey:@"State"] boolValue];

			[columnDict setObject:[NSNumber numberWithBool:newChecked] forKey:@"State"];
		}
	}
}

- (IBAction)ok:(id)sender
{
	LDPartListColumnVector newColumnOrder;

	htmlInventory->setShowModelFlag([showModelButton getCheck]);
	htmlInventory->setExternalCssFlag([externalStyleButton getCheck]);
	htmlInventory->setPartImagesFlag([showImagesButton getCheck]);
	htmlInventory->setShowFileFlag([showWebPageButton getCheck]);
	for (int i = 0; i < [columns count]; i++)
	{
		NSMutableDictionary *columnDict = [columns objectAtIndex:i];
		
		if ([[columnDict objectForKey:@"State"] boolValue])
		{
			newColumnOrder.push_back((LDPartListColumn)[[columnDict objectForKey:@"ID"] intValue]);
		}
	}
	if (newColumnOrder.size() > 0)
	{
		htmlInventory->setColumnOrder(newColumnOrder);
		[super ok:sender];
	}
	else
	{
		NSRunCriticalAlertPanel(@"No Columns Selected", @"You must select at least one of the columns in the list.", @"OK", nil, nil);
	}
}

@end
