#import "ErrorsAndWarnings.h"
#import "ModelWindow.h"
#import "ErrorItem.h"
#import "OCUserDefaults.h"
#import "OCLocalStrings.h"
#import "LDViewCategories.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <LDLoader/LDLError.h>
#include <TCFoundation/TCUserDefaults.h>

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

+ (BOOL)sharedInstanceIsVisible
{
	if (sharedInstance != nil)
	{
		return [sharedInstance isVisible];
	}
	else
	{
		return NO;
	}
}

- (void)dealloc
{
	[panel release];
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

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
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

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
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

- (void)tableView:(NSTableView *)aTableView willDisplayCell:(id)aCell forTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
	if (aTableView == enabledErrorsTable)
	{
		[aCell setTitle:[errorNames objectAtIndex:rowIndex]];
		[aCell setState:[[enabledErrors objectAtIndex:rowIndex] intValue]];
//		NSLog(@"%@\n", aCell);
//		[aCell setBackgroundColor:[NSColor colorWithCalibratedRed:1.0f green:0.85f blue:0.85f alpha:1.0f]];
//		if ([aCell respondsToSelector:@selector(setDrawsBackground:)])
//		{
//			[aCell setDrawsBackground:YES];
//		}
	}
}

- (void)resizeIfNeeded:(ErrorItem *)item
{
	int count = [item numberOfChildren];
	NSTableColumn *column = [errorsOutline outlineTableColumn];
	NSFont *font = [[column dataCell] font];
	float width = [column width];
	bool widthChanged = false;
	float indent = [errorsOutline indentationPerLevel] * [errorsOutline levelForItem:item] + 24.0f;
	
	for (int i = 0; i < count; i++)
	{
		ErrorItem *child = [item childAtIndex:i];
		float rowWidth = [[child stringValue] sizeWithAttributes:[NSDictionary dictionaryWithObject:font forKey: NSFontAttributeName]].width + indent;
		//float rowWidth = [font widthOfString:[child stringValue]] + indent;
		if (rowWidth > width)
		{
			width = rowWidth;
			widthChanged = true;
		}
	}
	if (widthChanged)
	{
		[column setWidth:width];
	}
}

- (void)initEnabledErrors
{
	errorNames = [[NSMutableArray alloc] init];
	enabledErrors = [[NSMutableArray alloc] init];
	for (int i = LDLEFirstError; i <= LDLELastError; i++)
	{
#ifdef TC_NO_UNICODE
		[errorNames addObject:[NSString stringWithASCIICString:LDLError::getTypeName((LDLErrorType)i)]];
#else // TC_NO_UNICODE
		[errorNames addObject:[NSString stringWithUCString:LDLError::getTypeNameW((LDLErrorType)i)]];
#endif // TC_NO_UNICODE
		[enabledErrors addObject:[NSNumber numberWithBool:[self showsErrorType:i]]];
	}
	[enabledErrorsTable reloadData];
}

- (void)awakeFromNib
{
	[self initEnabledErrors];
	titleFormat = [[panel title] retain];
	[includeWarningsButton setCheck:TCUserDefaults::boolForKey(SHOW_WARNINGS_KEY, false, false)];
}

- (IBAction)showIfNeeded
{
	if (errors > 0)
	{
		[self show:self];
	}
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
	TCUserDefaults::setBoolForKey([includeWarningsButton getCheck], SHOW_WARNINGS_KEY, false);
}

- (void)setShowAll:(BOOL)show
{
	for (int i = LDLEFirstError; i <= LDLELastError; i++)
	{
		[self setShowsErrorType:i value:show];
	}
	[enabledErrorsTable setNeedsDisplay:YES];
	[[NSNotificationCenter defaultCenter] postNotificationName:LDErrorFilterChange object:self];
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
		[[errorsOutline outlineTableColumn] setWidth:100];
		[self resizeIfNeeded:rootErrorItem];
	}
}

// NSOutlineView Data Source methods

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	if (item == nil)
	{
		item = rootErrorItem;
	}
	NSInteger retValue = [item numberOfChildren];
	if (retValue < 0)
	{
		retValue = 0;
	}
	return retValue;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
	if (item == nil)
	{
		item = rootErrorItem;
	}
	return [item numberOfChildren] != -1;
}

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
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
	NSString *errorText = @"";
	NSString *warningText = @"";
	NSString *spaceText = @"";

	errors = 0;
	warnings = 0;
	for (int i = 0; i < count; i++)
	{
		ErrorItem *child = [unfilteredRoot childAtIndex:i];
		LDLError *error = [child error];

		if ([[enabledErrors objectAtIndex:error->getType()] boolValue] &&
			([includeWarningsButton state] || error->getLevel() == LDLAError))
		{			
			[filteredRoot addChild:child];
			if (error->getLevel() == LDLAWarning)
			{
				warnings++;
			}
			else
			{
				errors++;
			}
		}
	}
	if (errors > 0)
	{
		if (errors == 1)
		{
			errorText = [OCLocalStrings get:@"ErrorTreeOneError"];
		}
		else
		{
			errorText = [NSString stringWithFormat:[OCLocalStrings get:@"ErrorTreeNErrors"], errors];
		}
		if (warnings > 0)
		{
			spaceText = @" ";
		}
	}
	if (warnings > 0)
	{
		if (warnings == 1)
		{
			warningText = [OCLocalStrings get:@"ErrorTreeOneWarning"];
		}
		else
		{
			warningText = [NSString stringWithFormat:[OCLocalStrings get:@"ErrorTreeNWarnings"], warnings];
		}
	}
	[statusField setStringValue:[NSString stringWithFormat:@"%@%@%@", errorText, spaceText, warningText]];
	return [filteredRoot autorelease];
}

- (void)update:(ModelWindow *)modelWindow
{
	if (modelWindow)
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
	else
	{
		[self setRootErrorItem:nil];
		[statusField setStringValue:@""];
		[panel setTitle:@""];
	}
}

- (void)outlineViewItemDidExpand:(NSNotification *)notification
{
	[self resizeIfNeeded:[[notification userInfo] objectForKey:@"NSObject"]];
}

@end
