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
	IBOutlet NSButton *texmapsCheck;
	IBOutlet NSButton *transparentTexturesLastCheck;
	IBOutlet NSSlider *textureOffsetSlider;
}

- (void)setup;
- (bool)updateLdPreferences;
- (IBAction)anisoLevel:(id)sender;
- (IBAction)primitiveSubstitution:(id)sender;
- (IBAction)textureStuds:(id)sender;
- (IBAction)filterType:(id)sender;
- (IBAction)texmap:(id)sender;

@end
