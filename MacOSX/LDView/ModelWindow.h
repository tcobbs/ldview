/* ModelWindow */

#import <Cocoa/Cocoa.h>

@class LDrawModelView;
@class LDViewController;
//@class ErrorsAndWarnings;
@class ErrorItem;

class LDLError;
class TCProgressAlert;
class TCAlert;
class AlertHandler;

@interface ModelWindow : NSObject
{
    IBOutlet NSWindow *window;
	IBOutlet LDrawModelView *modelView;
	IBOutlet NSProgressIndicator *progress;
	IBOutlet NSTextField *progressMessage;
	IBOutlet NSView *statusBar;

	IBOutlet NSSegmentedControl *actionsSegments;
	IBOutlet NSSegmentedControl *openButton;
	IBOutlet NSSegmentedControl *snapshotButton;
	IBOutlet NSSegmentedControl *reloadButton;
	IBOutlet NSSegmentedControl *featuresSegments;
	IBOutlet NSPopUpButton *viewPopUp;
	IBOutlet NSSegmentedControl *prefsSegments;
	IBOutlet NSSegmentedControl *viewModeSegments;

	LDViewController *controller;
	AlertHandler *alertHandler;
	NSToolbar *toolbar;
	NSDate *fpsReferenceDate;
	int fpsFrameCount;
	float fps;
	float progressAdjust;
	BOOL showStatusBar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *defaultIdentifiers;
	NSMutableArray *otherIdentifiers;
	NSMutableArray *allIdentifiers;
	ErrorItem *unfilteredRootErrorItem;
	ErrorItem *filteredRootErrorItem;
}

- (LDViewController *)controller;
- (BOOL)openModel:(NSString *)filename;
- (id)initWithController:(LDViewController *)value;
- (LDrawModelView *)modelView;

- (void)ldlErrorCallback:(LDLError *)error;
- (void)progressAlertCallback:(TCProgressAlert *)alert;
- (void)modelViewerAlertCallback:(TCAlert *)alert;
- (void)redrawAlertCallback:(TCAlert *)alert;
- (void)captureAlertCallback:(TCAlert *)alert;
- (void)releaseAlertCallback:(TCAlert *)alert;
- (void)lightVectorChanged:(TCAlert *)alert;
//- (void)peekMouseUpAlertCallback:(TCAlert *)alert;

- (void)modelWillReload;
- (void)updateFps;
- (void)clearFps;
//- (void)setShowStatusBar:(BOOL)value;
- (BOOL)showStatusBar;
- (void)show;
- (NSWindow *)window;
- (NSToolbar *)toolbar;
- (ErrorItem *)filteredRootErrorItem;

- (IBAction)actions:(id)sender;
- (IBAction)open:(id)sender;
- (IBAction)saveSnapshot:(id)sender;
- (IBAction)reload:(id)sender;
- (IBAction)features:(id)sender;
- (IBAction)viewingAngle:(id)sender;
- (IBAction)saveViewingAngle:(id)sender;
- (IBAction)preferences:(id)sender;
- (IBAction)viewMode:(id)sender;

@end
