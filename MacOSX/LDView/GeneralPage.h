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
    IBOutlet NSButton *processLDConfigCheck;
    IBOutlet NSButton *showErrorsCheck;
    IBOutlet NSButton *showFrameRateCheck;
    IBOutlet NSButton *transparentDefaultCheck;
}

- (void)setup;
- (void)updateLdPreferences;

@end
