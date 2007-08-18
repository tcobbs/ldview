/* Preferences */

#import <Cocoa/Cocoa.h>

@class EffectsPage;
@class GeneralPage;
@class GeometryPage;
@class PrefSetsPage;
@class PrimitivesPage;
@class UpdatesPage;
@class LDViewController;
@class ModelWindow;

class LDPreferences;

extern NSString *LDPreferencesDidUpdateNotification;

@interface Preferences : NSObject
{
    IBOutlet NSButton *applyButton;
    IBOutlet NSButton *cancelButton;
    IBOutlet EffectsPage *effectsPage;
    IBOutlet GeneralPage *generalPage;
    IBOutlet GeometryPage *geometryPage;
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
	int geometryIndex;
	int effectsIndex;
	int primitivesIndex;
	int updatesIndex;
	int prefSetsIndex;
}
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

@end
