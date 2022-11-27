/* ModelWindow */

#import <Cocoa/Cocoa.h>

@class LDrawModelView;
@class LDViewController;
//@class ErrorsAndWarnings;
@class ErrorItem;
@class SnapshotTaker;
@class SaveSnapshotViewOwner;
@class SaveExportViewOwner;
@class ModelTree;
@class MPD;

class LDLError;
class TCProgressAlert;
class TCAlert;
class AlertHandler;
class LDHtmlInventory;
class LDPartsList;

#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1060
@interface ModelWindow : NSObject < NSToolbarDelegate >
#else
@interface ModelWindow : NSObject
#endif
{
	IBOutlet NSWindow *window;
	IBOutlet LDrawModelView *modelView;
	IBOutlet NSProgressIndicator *progress;
	IBOutlet NSTextField *progressMessage;
	IBOutlet NSView *statusBar;

	IBOutlet NSSegmentedControl *fileActionsSegments;
	IBOutlet NSSegmentedControl *openButton;
	IBOutlet NSSegmentedControl *snapshotButton;
	IBOutlet NSSegmentedControl *exportButton;
	IBOutlet NSSegmentedControl *reloadButton;
	IBOutlet NSSegmentedControl *featuresSegments;
	IBOutlet NSSegmentedControl *partsAuthorSegments;
	IBOutlet NSSegmentedControl *wireframeSegments;
	IBOutlet NSSegmentedControl *seamsSegments;
	IBOutlet NSSegmentedControl *edgesSegments;
	IBOutlet NSSegmentedControl *primitivesSegments;
	IBOutlet NSSegmentedControl *lightingSegments;
	IBOutlet NSSegmentedControl *axesSegments;
	IBOutlet NSSegmentedControl *randomColorsSegments;
	IBOutlet NSSegmentedControl *bfcSegments;
	IBOutlet NSSegmentedControl *allConditionalsSegments;
	IBOutlet NSSegmentedControl *conditionalControlsSegments;
	IBOutlet NSSegmentedControl *texmapsSegments;
	IBOutlet NSSegmentedControl *prefsSegments;
	IBOutlet NSSegmentedControl *viewModeSegments;
	IBOutlet NSSegmentedControl *viewingAngleSegments;
	IBOutlet NSSegmentedControl *printSegments;
	IBOutlet NSSegmentedControl *stepSegments;
	IBOutlet NSSegmentedControl *stepSegments2;
	IBOutlet NSSegmentedControl *stepNextSegments;
	IBOutlet NSSegmentedControl *stepNextSegments2;
	IBOutlet NSSegmentedControl *stepPrevSegments;
	IBOutlet NSSegmentedControl *stepPrevSegments2;
	IBOutlet NSSegmentedControl *stepFirstSegments;
	IBOutlet NSSegmentedControl *stepLastSegments;
	IBOutlet NSSegmentedControl *viewFrontSegments;
	IBOutlet NSSegmentedControl *viewBackSegments;
	IBOutlet NSSegmentedControl *viewLeftSegments;
	IBOutlet NSSegmentedControl *viewRightSegments;
	IBOutlet NSSegmentedControl *viewTopSegments;
	IBOutlet NSSegmentedControl *viewBottomSegments;
	IBOutlet NSSegmentedControl *viewLatLonSegments;
	IBOutlet NSSegmentedControl *viewTwoThirdsSegments;
	IBOutlet NSSegmentedControl *viewSegments1;
	IBOutlet NSSegmentedControl *viewSegments2;
	IBOutlet NSSegmentedControl *otherActionsSegments;
	IBOutlet NSSegmentedControl *fullScreenSegments;
	IBOutlet NSSegmentedControl *zoomToFitSegments;
	IBOutlet NSSegmentedControl *errorsSegments;
	IBOutlet NSSegmentedControl *partsListSegments;
	IBOutlet NSSegmentedControl *modelTreeSegments;
	IBOutlet NSSegmentedControl *mpdSegments;
	IBOutlet NSSegmentedControl *povCameraSegments;
	IBOutlet NSSegmentedControl *helpSegments;
	IBOutlet NSSegmentedControl *otherFeaturesSegments;
	IBOutlet NSSegmentedControl *transDefaultSegments;
	IBOutlet NSSegmentedControl *partBBoxesSegments;
	IBOutlet NSSegmentedControl *wireframeCutawaySegments;
	IBOutlet NSSegmentedControl *flatShadingSegments;
	IBOutlet NSSegmentedControl *smoothCurvesSegments;
	IBOutlet NSSegmentedControl *studLogosSegments;
	IBOutlet NSSegmentedControl *lowResStudsSegments;
	IBOutlet NSSegmentedControl *latLonRotationSegments;
	IBOutlet NSSegmentedControl *boundingBoxSegments;
	IBOutlet NSLayoutConstraint *progressMessageLeft;
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
	double fps;
	CGFloat progressAdjust;
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
	SaveExportViewOwner *saveExportViewOwner;
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
	NSInteger mainFileSize;
	bool pollingUpdateNeeded;
	bool forceProgress;
	CGFloat latLonDelta;
	BOOL closing;
	CGFloat progressWidth;
	CGFloat progressMargin;
	NSArray *topLevelObjects;
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
- (bool)examineMode;
- (long)viewMode;
- (bool)keepRightSideUp;

- (void)setViewMode:(long)newViewMode;
- (void)setKeepRightSideUp:(bool)value;
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
- (double)fps;
//- (void)setShowStatusBar:(BOOL)value;
- (BOOL)showStatusBar;
- (void)show;
- (NSWindow *)window;
- (NSToolbar *)toolbar;
- (ErrorItem *)filteredRootErrorItem;
- (NSString *)filename;
- (BOOL)canCopy;
- (void)changeStep:(int)delta;
- (void)setMainViewWidth:(CGFloat)width height:(CGFloat)height;
- (NSSize)mainMarginSize;

- (void)copyStringToPasteboard:(NSString *)string;
- (IBAction)fileActions:(id)sender;
- (IBAction)open:(id)sender;
- (IBAction)saveSnapshot:(id)sender;
- (IBAction)reload:(id)sender;
- (IBAction)features:(id)sender;
- (IBAction)viewingAngle:(id)sender;
- (IBAction)saveViewingAngle:(id)sender;
- (IBAction)preferences:(id)sender;
- (IBAction)viewMode:(id)sender;
- (IBAction)cameraLocation:(id)sender;
- (IBAction)rotationCenter:(id)sender;
- (IBAction)latLongRotation:(id)sender;
- (IBAction)povCameraAspect:(id)sender;
- (IBAction)partsList:(id)sender;
- (IBAction)statistics:(id)sender;
- (IBAction)examineMode:(id)sender;
- (IBAction)flyThroughMode:(id)sender;
- (IBAction)walkMode:(id)sender;

- (bool)isModelTreeOpen;
- (bool)isMPDOpen;
- (void)escapePressed;

@end
