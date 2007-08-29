/* LDrawModelView */

#import <Cocoa/Cocoa.h>

class LDrawModelViewer;
class TCImage;

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
	NSPoint lastFrameMouseLocation;
	float originalZoomY;
	BOOL lButtonDown;
	BOOL rButtonDown;
	unsigned int mouseDownModifierFlags;
	unsigned int rightMouseDownModifierFlags;
	//NSDate *lastMoveTime;
	LDVViewMode viewMode;
	BOOL loading;
	BOOL redisplayRequested;
	float fps;
	int resizeCornerTextureId;
}

- (BOOL)openModel:(NSString *)filename;
- (void)resetView:(id)sender;
- (LDrawModelViewer *)modelViewer;
- (void)reloadNeeded;
- (void)reload;
- (void)setFps:(float)value;
- (void)setViewingAngle:(int)value;
- (IBAction)zoomToFit:(id)sender;

@end
