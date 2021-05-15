#import "CameraLocation.h"
#import "LDViewCategories.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>

@implementation CameraLocation

- (id)initWithX:(float)x y:(float)y z:(float)z
{
	if ((self = [super init]) != nil)
	{
		self->x = x;
		self->y = y;
		self->z = z;
		lookAt = (LDVLookAt)TCUserDefaults::longForKey(CAMERA_LOCATION_LOOK_AT, LDVLookAtModel, false);
		[super finishInitWithNibName:@"CameraLocation"];
	}
	return self;
}

- (void)setField:(NSTextField *)textField toFloat:(double)value
{
	ucstring valueString = ftoucstr(value, 2);
	
	[textField setStringValue:[NSString stringWithUCString:valueString]];
}

- (void)updateLookAtChecks
{
	[lookAtModelCheck setCheck:lookAt == LDVLookAtModel];
	[lookAtOriginCheck setCheck:lookAt == LDVLookAtOrigin];
}

- (void)awakeFromNib
{
	[self setField:xField toFloat:x];
	[self setField:yField toFloat:y];
	[self setField:zField toFloat:z];
	[self updateLookAtChecks];
}

- (void)updateValue:(float &)value fromField:(NSTextField *)textField min:(float)min max:(float)max
{
	bool changed = false;

	value = [textField floatValue];
	if (value < min)
	{
		value = min;
		changed = true;
	}
	if (value > max)
	{
		value = max;
		changed = true;
	}
	if (changed)
	{
		[self setField:textField toFloat:value];
		[textField selectText:self];
		NSBeep();
	}
}

- (void)controlTextDidChange:(NSNotification *)notification
{
	NSTextField *textField = [notification object];
	
	if (textField == xField)
	{
		[self updateValue:x fromField:textField min:-FLT_MAX max:FLT_MAX];
	}
	else if (textField == yField)
	{
		[self updateValue:y fromField:textField min:-FLT_MAX max:FLT_MAX];
	}
	else
	{
		[self updateValue:z fromField:textField min:-FLT_MAX max:FLT_MAX];
	}
}

- (float)x
{
	return x;
}

- (float)y
{
	return y;
}

- (float)z
{
	return z;
}

- (LDVLookAt)lookAt
{
	return lookAt;
}

- (IBAction)ok:(id)sender
{
	TCUserDefaults::setLongForKey(lookAt, CAMERA_LOCATION_LOOK_AT, false);
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
