/* LDrawModelView */

#import <Cocoa/Cocoa.h>

class LDrawModelViewer;

typedef enum
{
	LDVViewExamine,
	LDVViewFlythrough
} LDVViewMode;

@interface LDrawModelView : NSOpenGLView
{
	LDrawModelViewer *modelViewer;

	float rotationSpeed;
	NSPoint lastMouseLocation;
	float originalZoomY;
	BOOL lButtonDown;
	BOOL rButtonDown;
	unsigned int mouseDownModifierFlags;
	unsigned int rightMouseDownModifierFlags;
	NSDate *lastMoveTime;
	LDVViewMode viewMode;
	BOOL loading;
}

- (BOOL)openModel:(NSString *)filename;
- (void)resetView:(id)sender;

@end
