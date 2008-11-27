#import "LatLon.h"
#import "LDViewCategories.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>

@implementation LatLon

- (id)init
{
	return [super initWithNibName:@"LatLon"];
}

- (void)setField:(NSTextField *)textField toFloat:(double)value
{
	ucstring valueString = ftoucstr(value, 2);
	
	[textField setStringValue:[NSString stringWithUCString:valueString]];
}

- (void)updateEnabled
{
	[distField setEnabled:[distCheck getCheck]];
	[defaultDistButton setEnabled:[distCheck getCheck]];
	if ([distCheck getCheck])
	{
		[self setField:distField toFloat:dist];
	}
	else
	{
		[distField setStringValue:@""];
	}
}

- (void)awakeFromNib
{
	lat = TCUserDefaults::floatForKey(LAST_LAT_KEY, 30.0f, false);
	lon = TCUserDefaults::floatForKey(LAST_LON_KEY, 45.0f, false);
	haveDist = TCUserDefaults::boolForKey(LAST_HAVE_DIST_KEY, false, false);
	dist = TCUserDefaults::floatForKey(LAST_DIST_KEY, defaultDist, false);
	[self setField:latField toFloat:lat];
	[self setField:lonField toFloat:lon];
	[distCheck setCheck:haveDist];
	[self updateEnabled];
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
	
	if (textField == latField)
	{
		[self updateValue:lat fromField:textField min:-90.0f max:90.0f];
	}
	else if (textField == lonField)
	{
		[self updateValue:lon fromField:textField min:-180.0f max:180.0f];
	}
	else
	{
		[self updateValue:dist fromField:textField min:0.0f max:1e500f];
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

- (float)dist
{
	if (haveDist)
	{
		return dist;
	}
	else
	{
		return -1;
	}
}

- (bool)haveDist
{
	return haveDist;
}

- (IBAction)ok:(id)sender
{
	TCUserDefaults::setFloatForKey(lat, LAST_LAT_KEY, false);
	TCUserDefaults::setFloatForKey(lon, LAST_LON_KEY, false);
	TCUserDefaults::setBoolForKey(haveDist, LAST_HAVE_DIST_KEY, false);
	if (haveDist)
	{
		TCUserDefaults::setFloatForKey(dist, LAST_DIST_KEY, false);
	}
	[super ok:sender];
}

- (IBAction)distance:(id)sender
{
	haveDist = [distCheck getCheck];
	if (haveDist)
	{
		dist = defaultDist;
	}
	[self updateEnabled];
}

- (IBAction)defaultLat:(id)sender
{
	lat = 30.0f;
	[self setField:latField toFloat:lat];
	[self updateEnabled];
}

- (IBAction)defaultLon:(id)sender
{
	lon = 45.0f;
	[self setField:lonField toFloat:lon];
}

- (IBAction)defaultDist:(id)sender
{
	dist = defaultDist;
	[self updateEnabled];
}

- (IBAction)currentDist:(id)sender
{
	dist = currentDist;
	[self updateEnabled];
}

- (void)setDefaultDist:(float)value
{
	defaultDist = value;
}

- (void)setCurrentDist:(float)value
{
	currentDist = value;
}

@end
