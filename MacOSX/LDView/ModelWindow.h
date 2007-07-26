/* ModelWindow */

#import <Cocoa/Cocoa.h>

@class LDrawModelView;
@class LDViewController;

class LDLError;
class TCProgressAlert;
class AlertHandler;

@interface ModelWindow : NSObject
{
    IBOutlet NSWindow *window;
	IBOutlet LDrawModelView *modelView;
	IBOutlet NSProgressIndicator *progress;
	IBOutlet NSTextField *progressMessage;
	IBOutlet NSView *statusBar;

	LDViewController *controller;
	AlertHandler *alertHandler;
	NSToolbar *toolbar;
	NSDate *fpsReferenceDate;
	int fpsFrameCount;
	float fps;
	float progressAdjust;
	BOOL showStatusBar;
}

- (BOOL)openModel:(NSString *)filename;
- (id)initWithController:(LDViewController *)value;
- (LDrawModelView *)modelView;

- (void)ldlErrorCallback:(LDLError *)error;
- (void)progressAlertCallback:(TCProgressAlert *)alert;
- (void)modelWillReload;
- (void)updateFps;
- (void)clearFps;
- (void)setShowStatusBar:(BOOL)value;
- (BOOL)showStatusBar;
- (void)show;
- (NSWindow *)window;

@end
