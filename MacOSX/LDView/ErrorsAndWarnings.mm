#import "ErrorsAndWarnings.h"
#import "ModelWindow.h"
#import "ErrorItem.h"
#import "OCUserDefaults.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <LDLoader/LDLError.h>

@implementation ErrorsAndWarnings

NSString *LDErrorFilterChange = @"LDErrorFilterChange";
static ErrorsAndWarnings *sharedInstance = nil;

- (id)init
{
	if ((self = [super init]) != nil)
	{
		[NSBundle loadNibNamed:@"ErrorsAndWarnings.nib" owner:self];
	}
	return self;
}

+ (id)sharedInstance
{
	if (!sharedInstance)
	{
		sharedInstance = [[ErrorsAndWarnings alloc] init];
	}
	return sharedInstance;
}

- (void)dealloc
{
	[errorNames release];
	[enabledErrors release];
	[titleFormat release];
	[super dealloc];
}

- (NSString *)getErrorKey:(int)errorType
{
	return [NSString stringWithFormat:@"%s/LDLError%02d", SHOW_ERRORS_KEY, errorType];
}

- (BOOL)showsErrorType:(int)errorType
{
	return [OCUserDefaults longForKey:[self getErrorKey:errorType] defaultValue:1 sessionSpecific:NO] != 0;
}

- (void)setShowsErrorType:(int)errorType value:(BOOL)value
{
	[OCUserDefaults setLong:value forKey:[self getErrorKey:errorType] sessionSpecific:false];
	[enabledErrors replaceObjectAtIndex:errorType withObject:[NSNumber numberWithBool:value]];
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
		[enabledErrors addObject:[NSNumber numberWithBool:[self showsErrorType:i]]];
	}
	[enabledErrorsTable reloadData];
}

- (void)awakeFromNib
{
	[self initEnabledErrors];
	titleFormat = [[panel title] retain];
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
	[[NSNotificationCenter defaultCenter] postNotificationName:LDErrorFilterChange object:self];
}

- (void)setShowAll:(BOOL)show
{
	for (int i = LDLEFirstError; i < LDLELastError; i++)
	{
		[self setShowsErrorType:i value:show];
	}
	[enabledErrorsTable setNeedsDisplay:YES];
}

- (IBAction)showAll:(id)sender
{
	[self setShowAll:YES];
}

- (IBAction)showNone:(id)sender
{
	[self setShowAll:NO];
}

- (IBAction)enabledErrorSelected:(id)sender
{
	if ([[[[enabledErrorsTable tableColumns] objectAtIndex:[enabledErrorsTable clickedColumn]] identifier] isEqualToString:@"Checks"])
	{
		int row = [enabledErrorsTable clickedRow];
		BOOL newChecked = ![[enabledErrors objectAtIndex:row] boolValue];

		[self setShowsErrorType:row value:newChecked];
		[[NSNotificationCenter defaultCenter] postNotificationName:LDErrorFilterChange object:self];
	}
}

- (void)setRootErrorItem:(ErrorItem *)item
{
	if (item != rootErrorItem)
	{
		[rootErrorItem release];
		rootErrorItem = [item retain];
		[errorsOutline reloadData];
	}
}

// NSOutlineView Data Source methods

- (int)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	if (item == nil)
	{
		item = rootErrorItem;
	}
	return [item numberOfChildren];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
	if (item == nil)
	{
		item = rootErrorItem;
	}
	return [item numberOfChildren] != -1;
}

- (id)outlineView:(NSOutlineView *)outlineView child:(int)index ofItem:(id)item
{
	if (item == nil)
	{
		item = rootErrorItem;
	}
	return [item childAtIndex:index];
}

- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
	if (item == nil)
	{
		item = rootErrorItem;
	}
	return [item objectValue];
}

- (BOOL)isVisible
{
	return [panel isVisible];
}

- (ErrorItem *)filteredRootErrorItem:(ErrorItem *)unfilteredRoot
{
	ErrorItem *filteredRoot = [[ErrorItem alloc] init];
	int count = [unfilteredRoot numberOfChildren];
	
	for (int i = 0; i < count; i++)
	{
		ErrorItem *child = [unfilteredRoot childAtIndex:i];
		LDLError *error = [child error];

		if ([[enabledErrors objectAtIndex:error->getType()] boolValue] &&
			([includeWarningsButton state] || error->getLevel() == LDLAError))
		{			
			[filteredRoot addChild:child];
		}
	}
	return [filteredRoot autorelease];
}

- (void)update:(ModelWindow *)modelWindow
{
	NSWindow *window = [modelWindow window];
	NSString *representedFilename = [window representedFilename];

	[self setRootErrorItem: [modelWindow filteredRootErrorItem]];
	if ([representedFilename length] > 0)
	{
		[panel setTitle:[NSString stringWithFormat:titleFormat, [representedFilename lastPathComponent]]];
	}
	else
	{
		[panel setTitle:[NSString stringWithFormat:titleFormat, [window title]]];
	}
}

@end
