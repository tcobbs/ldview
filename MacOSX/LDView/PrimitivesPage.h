/* PrimitivesPage */

#import <Cocoa/Cocoa.h>

#import "PreferencePage.h"

@interface PrimitivesPage : PreferencePage
{
    IBOutlet NSTextField *anisoLevelLabel;
    IBOutlet NSSlider *anisoSlider;
    IBOutlet NSSlider *curveQualitySlider;
    IBOutlet NSMatrix *filteringMatrix;
    IBOutlet NSButton *hiResCheck;
    IBOutlet NSButton *lowQualityStudsCheck;
    IBOutlet NSButton *primitiveSubstitutionCheck;
    IBOutlet NSButton *textureStudsCheck;
}
@end
