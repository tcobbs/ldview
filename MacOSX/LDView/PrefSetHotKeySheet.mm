#import "PrefSetHotKeySheet.h"
#import "PrefSetsPage.h"

@implementation PrefSetHotKeySheet

- (id)initWithParent:(PrefSetsPage *)value
{
	parent = value;	// Don't retain.
	return [super initWithNibName:@"PrefSetHotKey.nib"];
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
	if ([self runSheetInWindow:[[parent preferences] window]] == NSOKButton)
	{
		return [NSNumber numberWithInt:[popUp indexOfSelectedItem] - 1];
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
