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
	NSPoint lastFrameMouseLocation;
	float originalZoomY;
	BOOL lButtonDown;
	BOOL rButtonDown;
	unsigned int mouseDownModifierFlags;
	unsigned int rightMouseDownModifierFlags;
	NSDate *lastMoveTime;
	LDVViewMode viewMode;
	BOOL loading;
	BOOL redisplayRequested;
	float fps;
	NSImage *resizeCornerImage;
}

- (BOOL)openModel:(NSString *)filename;
- (void)resetView:(id)sender;
- (LDrawModelViewer *)modelViewer;
- (void)reloadNeeded;
- (void)setFps:(float)value;

@end
