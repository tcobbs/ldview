#import "ErrorsAndWarnings.h"
#import "ModelWindow.h"
#include <LDLoader/LDLError.h>

@implementation ErrorsAndWarnings

- (id)init
{
	[NSBundle loadNibNamed:@"ErrorsAndWarnings.nib" owner:self];
	return [super init];
}

- (void)dealloc
{
	[errorNames release];
	[enabledErrors release];
	[super dealloc];
}

- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{
	if (aTableView == enabledErrorsTable)
	{
		return [errorNames count];
	}
	else if (aTableView == errorsOutline)
	{
		return 0;
	}
	else
	{
		return 0;
	}
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)rowIndex
{
	if (aTableView == enabledErrorsTable)
	{
		return [errorNames objectAtIndex:rowIndex];
	}
	else if (aTableView == errorsOutline)
	{
		return @"";
	}
	else
	{
		return @"";
	}
}

/*
- (BOOL)tableView:(NSTableView *)aTableView shouldEditTableColumn:(NSTableColumn *)aTableColumn row:(int)rowIndex
{
	if (aTableView == enabledErrorsTable)
	{
		return NO;//[[aTableColumn identifier] isEqualToString:@"Checks"];
	}
	else
	{
		return NO;
	}
}
*/

- (void)tableView:(NSTableView *)aTableView willDisplayCell:(id)aCell forTableColumn:(NSTableColumn *)aTableColumn row:(int)rowIndex
{
	if (aTableView == enabledErrorsTable)
	{
		[aCell setTitle:[errorNames objectAtIndex:rowIndex]];
		[aCell setState:[[enabledErrors objectAtIndex:rowIndex] intValue]];
	}
}

- (void)initEnabledErrors
{
	errorNames = [[NSMutableArray alloc] init];
	enabledErrors = [[NSMutableArray alloc] init];
	for (int i = LDLEFirstError; i < LDLELastError; i++)
	{
		// ToDo: Unicode fix
		[errorNames addObject:[NSString stringWithCString:LDLError::getTypeName((LDLErrorType)i) encoding:NSASCIIStringEncoding]];
		[enabledErrors addObject:[NSNumber numberWithBool:i % 2 == 0]];
	}
	[enabledErrorsTable reloadData];
}

- (void)awakeFromNib
{
	[self initEnabledErrors];
}

- (IBAction)show:(id)sender
{
	[panel makeKeyAndOrderFront:sender];
}

- (IBAction)copyError:(id)sender
{
}

- (IBAction)errorSelected:(id)sender
{
}

- (IBAction)includeWarnings:(id)sender
{
}

- (IBAction)showAll:(id)sender
{
}

- (IBAction)showNone:(id)sender
{
}

- (IBAction)enabledErrorSelected:(id)sender
{
	if ([[[[enabledErrorsTable tableColumns] objectAtIndex:[enabledErrorsTable clickedColumn]] identifier] isEqualToString:@"Checks"])
	{
		int row = [enabledErrorsTable clickedRow];
		BOOL checked = [[enabledErrors objectAtIndex:row] boolValue];

		[enabledErrors replaceObjectAtIndex:row withObject:[NSNumber numberWithBool:!checked]];
	}
}

@end
