/* PrimitivesPage */

#import <Cocoa/Cocoa.h>

#import "PreferencePage.h"

@interface PrimitivesPage : PreferencePage
{
	IBOutlet NSTextField *anisoLevelLabel;
	IBOutlet NSSlider *anisoSlider;
	IBOutlet NSTextField *curveQualityLabel;
	IBOutlet NSSlider *curveQualitySlider;
	IBOutlet NSButton *nearestRadio;
	IBOutlet NSButton *bilinearRadio;
	IBOutlet NSButton *trilinearRadio;
	IBOutlet NSButton *anisoRadio;
	IBOutlet NSButton *hiResCheck;
	IBOutlet NSButton *lowQualityStudsCheck;
	IBOutlet NSButton *primitiveSubstitutionCheck;
	IBOutlet NSButton *textureStudsCheck;
	IBOutlet NSButton *texmapsCheck;
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
