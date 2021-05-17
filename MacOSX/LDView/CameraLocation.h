/* CameraLocation */

#import <Cocoa/Cocoa.h>
#import "LocationSheet.h"
#include <LDLib/LDrawModelViewer.h>

@interface CameraLocation : LocationSheet
{
	IBOutlet NSButton *lookAtModelCheck;
	IBOutlet NSButton *lookAtOriginCheck;

	LDVLookAt lookAt;
}

- (id)initWithX:(float)x y:(float)y z:(float)z;
- (LDVLookAt)lookAt;

@end
