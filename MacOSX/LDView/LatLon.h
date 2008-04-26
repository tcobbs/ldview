/* LatLon */

#import <Cocoa/Cocoa.h>
#import "GenericSheet.h"

@interface LatLon : GenericSheet
{
    IBOutlet NSTextField *latField;
    IBOutlet NSTextField *lonField;

	float lat;
	float lon;
}

- (id)init;
- (float)lat;
- (float)lon;

@end
