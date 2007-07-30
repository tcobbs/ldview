#import "PrefSetNewSheet.h"
#import "PrefSetsPage.h"

@implementation PrefSetNewSheet

- (id)initWithParent:(PrefSetsPage *)value
{
	parent = value;	// Don't retain.
	return [super initWithNibName:@"PrefSetNew.nib"];
}

- (NSString *)getName
{
	if ([self runSheetInWindow:[[parent preferences] window]] == NSOKButton)
	{
		return [nameField stringValue];
	}
	else
	{
		return nil;
	}
}

@end
