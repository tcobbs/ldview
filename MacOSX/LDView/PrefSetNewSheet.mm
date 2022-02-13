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

- (void)getNameWithCompletionHandler:(void (^)(NSString *name))handler {
	[self beginSheetInWindow:[[parent preferences] window] completionHandler:^(NSModalResponse response){
		if (response == NSModalResponseOK)
		{
			handler([nameField stringValue]);
		}
		else
		{
			handler(nil);
		}
	}];
}

@end
