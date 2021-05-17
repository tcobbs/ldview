#import "RotationCenter.h"
#import "LDViewCategories.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>

@implementation RotationCenter

- (id)initWithX:(float)x y:(float)y z:(float)z
{
	if ((self = [super initWithX:x y:y z:z]) != nil)
	{
		[super finishInitWithNibName:@"RotationCenter"];
	}
	return self;
}

- (IBAction)reset:(id)sender
{
	[self stopModalWithCode:RotationCenterResetReturn];
}

@end
