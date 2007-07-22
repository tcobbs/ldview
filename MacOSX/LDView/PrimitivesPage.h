/* PrimitivesPage */

#import <Cocoa/Cocoa.h>

#import "PreferencePage.h"

@interface PrimitivesPage : PreferencePage
{
    IBOutlet NSTextField *anisoLevelLabel;
    IBOutlet NSSlider *anisoSlider;
	IBOutlet NSTextField *curveQualityLabel;
    IBOutlet NSSlider *curveQualitySlider;
    IBOutlet NSMatrix *filteringMatrix;
    IBOutlet NSButton *hiResCheck;
    IBOutlet NSButton *lowQualityStudsCheck;
    IBOutlet NSButton *primitiveSubstitutionCheck;
    IBOutlet NSButton *textureStudsCheck;
}

- (void)setup;
- (void)updateLdPreferences;
- (IBAction)anisoLevel:(id)sender;
- (IBAction)primitiveSubstitution:(id)sender;
- (IBAction)textureStuds:(id)sender;
- (IBAction)filterType:(id)sender;

@end
