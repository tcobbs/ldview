#import "PrefSetNewSheet.h"
#import "PrefSetsPage.h"

@implementation PrefSetNewSheet

- (id)initWithParent:(PrefSetsPage *)value
{
	if ((self = [super initWithNibName:@"PrefSetNew.nib"]) != nil)
	{
		parent = value;	// Don't retain.
	}
	return self;
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
