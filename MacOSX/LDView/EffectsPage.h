/* EffectsPage */

#import <Cocoa/Cocoa.h>

#import "PreferencePage.h"

class TCAlert;

@interface EffectsPage : PreferencePage
{
	IBOutlet NSButton *alternateLightingCheck;
	IBOutlet NSButton *flatShadingCheck;
	IBOutlet NSButton *hideLightDatGeom;
	IBOutlet NSButton *highQualityCheck;
	IBOutlet NSBox *lightDirBox;
	IBOutlet NSMatrix *lightDirMatrix;
	IBOutlet NSButton *lightingCheck;
	IBOutlet NSTextField *opacityLabel;
	IBOutlet NSSlider *opacitySlider;
	IBOutlet NSButton *replaceStandardLightCheck;
	IBOutlet NSButton *smoothCurvesCheck;
	IBOutlet NSButton *sortTransCheck;
	IBOutlet NSButton *specularCheck;
	IBOutlet NSTextField *stereoAmountLabel;
	IBOutlet NSSlider *stereoAmountSlider;
	IBOutlet NSButton *stereoCheck;
	IBOutlet NSButton *crossEyedRadio;
	IBOutlet NSButton *parallelRadio;
	IBOutlet NSButton *stippleTransCheck;
	IBOutlet NSButton *subduedCheck;
	IBOutlet NSButton *useLightDatCheck;
	IBOutlet NSTextField *wcThicknessLabel;
	IBOutlet NSSlider *wcThicknessSlider;
	IBOutlet NSButton *wireframeCutawayCheck;
	IBOutlet NSButton *cutawayColorRadio;
	IBOutlet NSButton *cutawayMonochromeRadio;
}

- (IBAction)lighting:(id)sender;
- (IBAction)useLightDats:(id)sender;
- (IBAction)stereo:(id)sender;
- (IBAction)wireframeCutaway:(id)sender;
- (IBAction)sort:(id)sender;
- (IBAction)stipple:(id)sender;
- (IBAction)flatShading:(id)sender;
- (IBAction)smoothCurves:(id)sender;
- (IBAction)stereoModeChanged:(id)sender;
- (IBAction)cutawayModeChanged:(id)sender;

- (void)lightVectorChanged:(TCAlert *)alert;

- (void)setup;
- (bool)updateLdPreferences;

@end
