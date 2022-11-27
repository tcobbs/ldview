/* GeometryPage */

#import <Cocoa/Cocoa.h>

#import "PreferencePage.h"

@interface GeometryPage : PreferencePage
{
	IBOutlet NSButton *bfcCheck;
	IBOutlet NSButton *blackEdgesCheck;
	IBOutlet NSButton *blueNeutralFacesCheck;
	IBOutlet NSButton *boundingBoxesOnlyCheck;
	IBOutlet NSButton *conditionalLinesCheck;
	IBOutlet NSButton *edgeLinesCheck;
	IBOutlet NSButton *edgesOnlyCheck;
	IBOutlet NSTextField *edgeThicknessLabel;
	IBOutlet NSSlider *edgeThicknessSlider;
	IBOutlet NSButton *fogCheck;
	IBOutlet NSButton *greenFrontFacesCheck;
	IBOutlet NSButton *highQualityEdgesCheck;
	IBOutlet NSButton *redBackFacesCheck;
	IBOutlet NSButton *removeHiddenCheck;
	IBOutlet NSButton *seamWidthCheck;
	IBOutlet NSStepper *seamWidthStepper;
	IBOutlet NSTextField *seamWidthField;
	IBOutlet NSButton *showAllConditionalsCheck;
	IBOutlet NSButton *showControlPtsCheck;
	IBOutlet NSButton *wireframeCheck;
	IBOutlet NSTextField *wireframeThicknessLabel;
	IBOutlet NSSlider *wireframeThicknessSlider;
}
- (void)setup;
- (bool)updateLdPreferences;
- (IBAction)seamWidth:(id)sender;
- (IBAction)wireframe:(id)sender;
- (IBAction)bfc:(id)sender;
- (IBAction)edgeLines:(id)sender;
- (IBAction)conditionalLines:(id)sender;
@end
