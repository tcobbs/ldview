/* Preferences */

#import <Cocoa/Cocoa.h>

@class EffectsPage;
@class GeneralPage;
@class GeometryPage;
@class LDrawPage;
@class PrefSetsPage;
@class PrimitivesPage;
@class UpdatesPage;
@class LDViewController;
@class ModelWindow;
@class PreferencePage;

class LDPreferences;
class TCAlert;

extern NSString *LDPreferencesDidUpdateNotification;

@interface Preferences : NSObject
{
	IBOutlet NSButton *applyButton;
	IBOutlet NSButton *cancelButton;
	IBOutlet EffectsPage *effectsPage;
	IBOutlet GeneralPage *generalPage;
	IBOutlet GeometryPage *geometryPage;
	IBOutlet LDrawPage *ldrawPage;
	IBOutlet NSButton *okButton;
	IBOutlet PrefSetsPage *prefSetsPage;
	IBOutlet PrimitivesPage *primitivesPage;
	IBOutlet NSTabView *tabView;
	IBOutlet UpdatesPage *updatesPage;
	IBOutlet NSWindow *window;
	LDViewController *controller;
	LDPreferences *ldPreferences;
	NSMutableArray *pages;
	int generalIndex;
	int ldrawIndex;
	int geometryIndex;
	int effectsIndex;
	int primitivesIndex;
	int updatesIndex;
	int prefSetsIndex;
	PreferencePage *applyFailedPage;
	BOOL applyEnabled;
	BOOL loading;
	BOOL openGLInitialized;
	NSArray *topLevelObjects;
}

- (LDrawPage *)ldrawPage;
- (GeneralPage *)generalPage;
- (bool)applyFailed;
- (void)setApplyFailed:(PreferencePage *)page;
- (id)initWithController:(LDViewController *)value;
- (void)awakeFromNib;
- (void)show;
- (void)initModelWindow:(ModelWindow *)value;
- (IBAction)apply:(id)sender;
- (IBAction)cancel:(id)sender;
- (IBAction)ok:(id)sender;
- (LDPreferences *)ldPreferences;
- (void)enableApply:(BOOL)enabled;
- (BOOL)isApplyEnabled;
- (NSWindow *)window;
- (void)loadSettings;
- (void)hotKeyPressed:(int)index;
- (void)saveViewingAngle:(ModelWindow*)modelWindow;
- (void)lightVectorChanged:(TCAlert *)alert;
- (void)openGLInitialized;

- (void)takeWireframeFrom:(id)sender;
- (void)takeSeamsFrom:(id)sender;
- (void)takeEdgesFrom:(id)sender;
- (void)takePrimSubFrom:(id)sender;
- (void)takeLightingFrom:(id)sender;
- (void)takeBfcFrom:(id)sender;
- (void)takeAllConditionalsFrom:(id)sender;
- (void)takeConditionalControlsFrom:(id)sender;
- (void)takeTexmapsFrom:(id)sender;
- (void)takeTransDefaultFrom:(id)sender;
- (void)takePartBBoxesFrom:(id)sender;
- (void)takeWireframeCutawayFrom:(id)sender;
- (void)takeFlatShadingFrom:(id)sender;
- (void)takeSmoothCurvesFrom:(id)sender;
- (void)takeStudLogosFrom:(id)sender;
- (void)takeLowResStudsFrom:(id)sender;
- (void)takeShowAxesFrom:(id)sender;
- (void)takeRandomColorsFrom:(id)sender;
- (void)modelWindowWillClose:(ModelWindow *)modelWindow;

@end
