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
	int count = (int)[item numberOfChildren];
	NSTableColumn *column = [errorsOutline outlineTableColumn];
	NSFont *font = [[column dataCell] font];
	float width = [column width];
	float maxWidth = [column maxWidth];
	bool widthChanged = false;
	
	for (int i = 0; i < count; i++)
	{
		ErrorItem *child = [item childAtIndex:i];
		float indent = [errorsOutline indentationPerLevel] * [errorsOutline levelForItem:child] + 20.0f;
		float rowWidth = 0.0f;
		if ([[child objectValue] isKindOfClass:[NSAttributedString class]])
		{
			NSAttributedString *objectValue = [child objectValue];
			NSFont *oldFont = [objectValue attribute:NSFontAttributeName atIndex:0 effectiveRange:NULL];
			if (oldFont)
			{
				rowWidth = [objectValue size].width + indent;
			}
		}
		if (rowWidth == 0.0f)
		{
			rowWidth = [[child stringValue] sizeWithAttributes:[NSDictionary dictionaryWithObject:font forKey: NSFontAttributeName]].width + indent;
		}
		if (rowWidth > width)
		{
			width = ceil(rowWidth);
			widthChanged = true;
		}
	}
	if (widthChanged)
	{
		if (width > maxWidth)
		{
			[column setMaxWidth:width];
		}
		[column setWidth:width];
		[errorsOutline reloadItem:item reloadChildren:YES];
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

- (void)appendItem:(ErrorItem *)item toClipString:(NSMutableString *)clipString level:(NSInteger)level
{
	NSInteger itemLevel = level;
	if (level == -1)
	{
		itemLevel = [errorsOutline levelForItem:item];
	}
	NSMutableString *itemString = [[NSMutableString alloc] init];
	for (NSInteger i = 0; i < itemLevel; ++i)
	{
		[itemString appendString:@"\t"];
	}
	[itemString appendString:[item stringValue]];
	[itemString appendString:@"\n"];
	[clipString appendString:itemString];
	[itemString release];
	if (level == -1 && ![errorsOutline isItemExpanded:item])
	{
		NSInteger numChildren = [item numberOfChildren];
		for (NSInteger i = 0; i < numChildren; ++i)
		{
			[self appendItem:[item childAtIndex:i] toClipString:clipString level:itemLevel + 1];
		}
	}
}

- (void)appendItem:(ErrorItem *)item toClipString:(NSMutableString *)clipString
{
	return [self appendItem:item toClipString:clipString level:-1];
}

- (IBAction)copyError:(id)sender
{
	NSMutableString *clipString = [[NSMutableString alloc] init];
	NSIndexSet *selectedIndexes = [errorsOutline selectedRowIndexes];
	[selectedIndexes enumerateIndexesUsingBlock:^(NSUInteger index, BOOL *stop)
	{
		ErrorItem *item = [errorsOutline itemAtRow:index];
		[self appendItem:item toClipString:clipString];
	}];
	if ([clipString length] > 0)
	{
		NSPasteboard *pb = [NSPasteboard generalPasteboard];
		
		[pb declareTypes:[NSArray arrayWithObject:NSPasteboardTypeString] owner:self];
		[pb setString:clipString forType:NSPasteboardTypeString];
	}
	[clipString release];
}

- (IBAction)copy:(id)sender
{
	[self copyError:sender];
}

- (BOOL)canPerformAction:(SEL)action withSender:(id)sender
{
	if (action == @selector(copy:))
	{
		return [copyErrorButton isEnabled];
	}
	return NO;
}

- (IBAction)errorSelected:(id)sender
{
	NSIndexSet *selectedIndexes = [errorsOutline selectedRowIndexes];
	[copyErrorButton setEnabled:[selectedIndexes count] > 0];
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
		int row = (int)[enabledErrorsTable clickedRow];
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
		[errorsOutline setAutoresizesOutlineColumn:NO];
		[errorsOutline reloadData];
		[errorsOutline deselectAll:nil];
		[copyErrorButton setEnabled:NO];
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
	int count = (int)[unfilteredRoot numberOfChildren];
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
