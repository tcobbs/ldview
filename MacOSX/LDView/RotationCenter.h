/* RotationCenter */

#import <Cocoa/Cocoa.h>
#import "LocationSheet.h"

static const NSModalResponse RotationCenterResetReturn = 10000;

@interface RotationCenter : LocationSheet

- (id)initWithX:(float)x y:(float)y z:(float)z;

@end
