#import "LatLon.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>

@implementation LatLon

- (id) init
{
	return [super initWithNibName:@"LatLon"];
}

- (void)awakeFromNib
{
	lat = TCUserDefaults::floatForKey(LAST_LAT_KEY, 30.0f, false);
	lon = TCUserDefaults::floatForKey(LAST_LON_KEY, 45.0f, false);
	[latField setFloatValue:lat];
	[lonField setFloatValue:lon];
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
		[textField setFloatValue:value];
		[textField selectText:self];
		NSBeep();
	}
}

- (void)controlTextDidChange:(NSNotification *)notification
{
	NSTextField *textField = [notification object];
	
	if (textField == latField)
	{
		[self updateValue:lat fromField:textField min:-90.0f max:90.0f];
	}
	else
	{
		[self updateValue:lon fromField:textField min:-180.0f max:180.0f];
	}
}

- (float)lat
{
	return lat;
}

- (float)lon
{
	return lon;
}

- (IBAction)ok:(id)sender
{
	TCUserDefaults::setFloatForKey(lat, LAST_LAT_KEY, false);
	TCUserDefaults::setFloatForKey(lon, LAST_LON_KEY, false);
	[super ok:sender];
}

@end
