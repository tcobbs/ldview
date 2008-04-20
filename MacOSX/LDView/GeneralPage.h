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
	IBOutlet NSButton *showAxesCheck;
    IBOutlet NSButton *transparentDefaultCheck;
	
	IBOutlet NSPopUpButton *snapshotsDirPopUp;
	IBOutlet NSPopUpButton *partsListsDirPopUp;
	IBOutlet NSTextField *snapshotsDirField;
	IBOutlet NSTextField *partsListsDirField;
	IBOutlet NSButton *snapshotsBrowseButton;
	IBOutlet NSButton *partsListsBrowseButton;

	NSString *snapshotsDir;
	NSString *partsListsDir;
}

- (bool)showErrorsIfNeeded;
- (bool)promptAtStartup;
- (bool)newModelWindows;

- (void)setup;
- (bool)updateLdPreferences;

- (int)snapshotsDirMode;
- (NSString *)snapshotsDir;
- (int)partsListsDirMode;
- (NSString *)partsListsDir;

@end
