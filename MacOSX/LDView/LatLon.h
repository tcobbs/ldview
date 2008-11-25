/* LatLon */

#import <Cocoa/Cocoa.h>
#import "GenericSheet.h"

@interface LatLon : GenericSheet
{
    IBOutlet NSTextField *latField;
    IBOutlet NSTextField *lonField;
	IBOutlet NSButton *distCheck;
	IBOutlet NSTextField *distField;
	IBOutlet NSButton *defaultDistButton;

	float lat;
	float lon;
	bool haveDist;
	float dist;
	float defaultDist;
}

- (void)setDefaultDist:(float)value;
- (id)init;
- (float)lat;
- (float)lon;
- (bool)haveDist;
- (float)dist;

@end
