/* GeneralPage */

#import <Cocoa/Cocoa.h>

#import "PreferencePage.h"

@interface GeneralPage : PreferencePage
{
    IBOutlet NSButton *antialiasedLinesCheck;
    IBOutlet NSColorWell *backgroundColorWell;
    IBOutlet NSColorWell *defaultColorWell;
    IBOutlet NSTextField *fovField;
    IBOutlet NSPopUpButton *memoryUsagePopUp;
	IBOutlet NSButton *newModelWindowsCheck;
	IBOutlet NSButton *promptAtStartupCheck;
    IBOutlet NSButton *processLDConfigCheck;
    IBOutlet NSButton *showErrorsCheck;
    IBOutlet NSButton *showFrameRateCheck;
    IBOutlet NSButton *transparentDefaultCheck;
}

- (bool)showErrorsIfNeeded;
- (bool)promptAtStartup;
- (bool)newModelWindows;

- (void)setup;
- (bool)updateLdPreferences;

@end
