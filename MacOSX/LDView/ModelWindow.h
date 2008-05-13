/* ModelWindow */

#import <Cocoa/Cocoa.h>

@class LDrawModelView;
@class LDViewController;
//@class ErrorsAndWarnings;
@class ErrorItem;
@class SnapshotTaker;
@class SaveSnapshotViewOwner;
@class ModelTree;
@class MPD;

class LDLError;
class TCProgressAlert;
class TCAlert;
class AlertHandler;
class LDHtmlInventory;
class LDPartsList;

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
	IBOutlet NSSegmentedControl *prefsSegments;
	IBOutlet NSSegmentedControl *viewModeSegments;
	IBOutlet NSSegmentedControl *viewingAngleSegments;
	IBOutlet NSSegmentedControl *printSegments;
	IBOutlet NSSegmentedControl *customizeSegments;
	IBOutlet NSSegmentedControl *stepSegments;
	IBOutlet NSSegmentedControl *stepSegments2;
	IBOutlet NSSegmentedControl *stepNextSegments;
	IBOutlet NSSegmentedControl *stepNextSegments2;
	IBOutlet NSSegmentedControl *stepPrevSegments;
	IBOutlet NSSegmentedControl *stepPrevSegments2;
	IBOutlet NSSegmentedControl *stepFirstSegments;
	IBOutlet NSSegmentedControl *stepLastSegments;
	IBOutlet NSTextField *stepField;
	
	IBOutlet NSBox *latLonBox;
	IBOutlet NSTextField *latField;
	IBOutlet NSTextField *lonField;
	
	NSArray *stepToolbarControls;
	
	IBOutlet NSMenu *stepsMenu;

	LDViewController *controller;
	AlertHandler *alertHandler;
	NSToolbar *toolbar;
	NSDate *fpsReferenceDate;
	int fpsFrameCount;
	bool examineLatLong;
	float fps;
	float progressAdjust;
	BOOL showStatusBar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *defaultIdentifiers;
	NSMutableArray *otherIdentifiers;
	NSMutableArray *allIdentifiers;
	ErrorItem *unfilteredRootErrorItem;
	ErrorItem *filteredRootErrorItem;
	NSArray *imageFileTypes;
	SnapshotTaker *snapshotTaker;
	SaveSnapshotViewOwner *saveSnapshotViewOwner;
	ModelTree *modelTree;
	MPD *mpd;
	bool loadCanceled;
	bool loading;
	bool parsing;
	NSString *initialTitle;
	bool sheetBusy;
	LDHtmlInventory *htmlInventory;
	LDPartsList *partsList;
	bool replaceSegments;
	NSTimer *pollingTimer;
	NSDate *lastWriteTime;
	bool pollingUpdateNeeded;
	bool forceProgress;
	float latLonDelta;
}

- (bool)sheetBusy;
- (bool)loading;
- (bool)parsing;
- (bool)loadCanceled;
- (LDViewController *)controller;
- (BOOL)openModel:(NSString *)filename;
- (id)initWithController:(LDViewController *)value;
- (LDrawModelView *)modelView;
- (bool)examineLatLong;
- (bool)flyThroughMode;
- (bool)fullScreen;

- (void)setFlyThroughMode:(bool)value;
- (void)updateStatusLatLon;
- (void)ldlErrorCallback:(LDLError *)error;
- (void)progressAlertCallback:(TCProgressAlert *)alert;
- (void)modelViewerAlertCallback:(TCAlert *)alert;
- (void)redrawAlertCallback:(TCAlert *)alert;
- (void)loadAlertCallback:(TCAlert *)alert;
- (void)captureAlertCallback:(TCAlert *)alert;
- (void)releaseAlertCallback:(TCAlert *)alert;
- (void)lightVectorChanged:(TCAlert *)alert;
//- (void)peekMouseUpAlertCallback:(TCAlert *)alert;

- (void)modelWillReload;
- (void)updateFps;
- (void)clearFps;
- (float)fps;
//- (void)setShowStatusBar:(BOOL)value;
- (BOOL)showStatusBar;
- (void)show;
- (NSWindow *)window;
- (NSToolbar *)toolbar;
- (ErrorItem *)filteredRootErrorItem;
- (LDrawModelView *)modelView;
- (NSString *)filename;
- (BOOL)canCopy;
- (void)changeStep:(int)delta;

- (void)copyStringToPasteboard:(NSString *)string;
- (IBAction)actions:(id)sender;
- (IBAction)open:(id)sender;
- (IBAction)saveSnapshot:(id)sender;
- (IBAction)reload:(id)sender;
- (IBAction)features:(id)sender;
- (IBAction)viewingAngle:(id)sender;
- (IBAction)saveViewingAngle:(id)sender;
- (IBAction)preferences:(id)sender;
- (IBAction)viewMode:(id)sender;
- (IBAction)latLongRotation:(id)sender;
- (IBAction)partsList:(id)sender;

- (bool)isModelTreeOpen;
- (bool)isMPDOpen;

@end
