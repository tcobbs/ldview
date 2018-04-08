#import "PrefSetHotKeySheet.h"
#import "PrefSetsPage.h"
#import "LDViewCategories.h"
#import "Preferences.h"

@implementation PrefSetHotKeySheet

- (id)initWithParent:(PrefSetsPage *)value
{
	if ((self = [super init]) != nil)
	{
		parent = value;	// Don't retain.
		[self ldvLoadNibNamed:@"PrefSetHotKey" topLevelObjects:&topLevelObjects];
		[topLevelObjects retain];
	}
	return self;
}

- (NSNumber *)getHotKey:(NSNumber *)initialValue
{
	if (initialValue)
	{
		[popUp selectItemAtIndex:[initialValue intValue] + 1];
	}
	else
	{
		[popUp selectItemAtIndex:0];
	}
	if ([self runSheetInWindow:[[parent preferences] window]] == NSModalResponseOK)
	{
		return [NSNumber numberWithInteger:[popUp indexOfSelectedItem] - 1];
	}
	else
	{
		return nil;
	}
}

- (void)awakeFromNib
{
	for (int i = 0; i < 10; i++)
	{
		[popUp addItemWithTitle:[parent hotKeyLabel:i]];
	}
}

@end
