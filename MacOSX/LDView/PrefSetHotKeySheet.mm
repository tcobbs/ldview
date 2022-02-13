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

- (void)getHotKey:(NSNumber *)initialValue withCompletion:(void (^)(NSNumber *value))handler
{
	if (initialValue)
	{
		[popUp selectItemAtIndex:[initialValue intValue] + 1];
	}
	else
	{
		[popUp selectItemAtIndex:0];
	}
	[self beginSheetInWindow:[[parent preferences] window] completionHandler:^(NSModalResponse response){
		if (response == NSModalResponseOK)
		{
			handler([NSNumber numberWithInteger:[popUp indexOfSelectedItem] - 1]);
		}
		else
		{
			handler(nil);
		}
	}];
}

- (void)awakeFromNib
{
	for (int i = 0; i < 10; i++)
	{
		[popUp addItemWithTitle:[parent hotKeyLabel:i]];
	}
}

@end
