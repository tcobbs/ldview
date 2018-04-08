#import "PrefSetNewSheet.h"
#import "PrefSetsPage.h"
#import "Preferences.h"

@implementation PrefSetNewSheet

- (id)initWithParent:(PrefSetsPage *)value
{
	if ((self = [super initWithNibName:@"PrefSetNew"]) != nil)
	{
		parent = value;	// Don't retain.
	}
	return self;
}

- (NSString *)getName
{
	if ([self runSheetInWindow:[[parent preferences] window]] == NSModalResponseOK)
	{
		return [nameField stringValue];
	}
	else
	{
		return nil;
	}
}

@end
