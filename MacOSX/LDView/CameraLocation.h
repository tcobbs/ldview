/* CameraLocation */

#import <Cocoa/Cocoa.h>
#import "GenericSheet.h"
#include <LDLib/LDrawModelViewer.h>

@interface CameraLocation : GenericSheet
{
    IBOutlet NSTextField *xField;
    IBOutlet NSTextField *yField;
	IBOutlet NSTextField *zField;
	IBOutlet NSButton *lookAtModelCheck;
	IBOutlet NSButton *lookAtOriginCheck;

	float x;
	float y;
	float z;
	LDVLookAt lookAt;
}

- (id)initWithX:(float)x y:(float)y z:(float)z;
- (float)x;
- (float)y;
- (float)z;
- (LDVLookAt)lookAt;

@end
