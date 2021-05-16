/* RotationCenter */

#import <Cocoa/Cocoa.h>
#import "GenericSheet.h"

static const NSModalResponse RotationCenterResetReturn = 10000;

@interface RotationCenter : GenericSheet
{
    IBOutlet NSTextField *xField;
    IBOutlet NSTextField *yField;
	IBOutlet NSTextField *zField;

	float x;
	float y;
	float z;
}

- (id)initWithX:(float)x y:(float)y z:(float)z;
- (float)x;
- (float)y;
- (float)z;

@end
