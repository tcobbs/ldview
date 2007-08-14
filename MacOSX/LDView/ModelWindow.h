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

	IBOutlet NSSegmentedControl *actionsSegments;
	IBOutlet NSSegmentedControl *featuresSegments;
	IBOutlet NSPopUpButton *viewPopUp;
	IBOutlet NSSegmentedControl *prefsSegments;

	LDViewController *controller;
	AlertHandler *alertHandler;
	NSToolbar *toolbar;
	NSDate *fpsReferenceDate;
	int fpsFrameCount;
	float fps;
	float progressAdjust;
	BOOL showStatusBar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarItemIdentifiers;
}

- (BOOL)openModel:(NSString *)filename;
- (id)initWithController:(LDViewController *)value;
- (LDrawModelView *)modelView;

- (void)ldlErrorCallback:(LDLError *)error;
- (void)progressAlertCallback:(TCProgressAlert *)alert;
- (void)modelWillReload;
- (void)updateFps;
- (void)clearFps;
//- (void)setShowStatusBar:(BOOL)value;
- (BOOL)showStatusBar;
- (void)show;
- (NSWindow *)window;
- (NSToolbar *)toolbar;

- (IBAction)actions:(id)sender;
- (IBAction)features:(id)sender;
- (IBAction)viewingAngle:(id)sender;
- (IBAction)saveViewingAngle:(id)sender;
- (IBAction)preferences:(id)sender;

@end
