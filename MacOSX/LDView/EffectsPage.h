/* EffectsPage */

#import <Cocoa/Cocoa.h>

#import "PreferencePage.h"

@interface EffectsPage : PreferencePage
{
    IBOutlet NSButton *alternateLightingCheck;
    IBOutlet NSButton *flatShadingCheck;
    IBOutlet NSButton *hideLightDatGeom;
    IBOutlet NSButton *highQualityCheck;
    IBOutlet NSMatrix *lightDirMatrix;
    IBOutlet NSButton *lightingCheck;
    IBOutlet NSSlider *opacitySlider;
    IBOutlet NSButton *replaceStandardLightCheck;
    IBOutlet NSButton *smoothCurvesCheck;
    IBOutlet NSButton *sortTransCheck;
    IBOutlet NSButton *specularCheck;
    IBOutlet NSSlider *stereoAmountSlider;
    IBOutlet NSButton *stereoCheck;
    IBOutlet NSMatrix *stereoMatrix;
    IBOutlet NSButton *stippleTransCheck;
    IBOutlet NSButton *subduedCheck;
    IBOutlet NSButton *useLightDatCheck;
    IBOutlet NSSlider *wcThicknessSlider;
    IBOutlet NSButton *wireframeCutawayCheck;
    IBOutlet NSMatrix *wireframeCutawayMatrix;
}
- (IBAction)lighting:(id)sender;
- (IBAction)useLightDats:(id)sender;
- (IBAction)stereo:(id)sender;
- (IBAction)wireframeCutaway:(id)sender;
@end
