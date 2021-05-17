#import "CameraLocation.h"
#import "LDViewCategories.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>

@implementation CameraLocation

- (id)initWithX:(float)x y:(float)y z:(float)z
{
	if ((self = [super initWithX:x y:y z:z]) != nil)
	{
		lookAt = (LDVLookAt)TCUserDefaults::longForKey(CAMERA_LOCATION_LOOK_AT_KEY, LDVLookAtModel, false);
		[super finishInitWithNibName:@"CameraLocation"];
	}
	return self;
}

- (void)updateLookAtChecks
{
	[lookAtModelCheck setCheck:lookAt == LDVLookAtModel];
	[lookAtOriginCheck setCheck:lookAt == LDVLookAtOrigin];
}

- (void)awakeFromNib
{
	[self updateLookAtChecks];
	[super awakeFromNib];
}

- (LDVLookAt)lookAt
{
	return lookAt;
}

- (IBAction)ok:(id)sender
{
	TCUserDefaults::setLongForKey(lookAt, CAMERA_LOCATION_LOOK_AT_KEY, false);
	[super ok:sender];
}

- (void)updateLookAtFrom:(NSButton*)check to:(LDVLookAt)newLookAt
{
	if ([check getCheck])
	{
		lookAt = newLookAt;
	}
	else
	{
		lookAt = LDVLookAtNone;
	}
	[self updateLookAtChecks];
}

- (IBAction)lookAtModel:(id)sender
{
	[self updateLookAtFrom:lookAtModelCheck to:LDVLookAtModel];
}

-(IBAction)lookAtOrigin:(id)sender
{
	[self updateLookAtFrom:lookAtOriginCheck to:LDVLookAtOrigin];
}

@end
