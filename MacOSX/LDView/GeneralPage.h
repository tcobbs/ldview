/* GeneralPage */

#import <Cocoa/Cocoa.h>
#import "PreferencePage.h"

#include <LDLib/LDPreferences.h>

@interface GeneralPage : PreferencePage
{
	IBOutlet NSButton *antialiasedLinesCheck;
	IBOutlet NSColorWell *backgroundColorWell;
	IBOutlet NSColorWell *defaultColorWell;
	IBOutlet NSTextField *fovField;
	IBOutlet NSPopUpButton *memoryUsagePopUp;
	IBOutlet NSButton *disableSmpCheck;
	IBOutlet NSButton *newModelWindowsCheck;
	IBOutlet NSButton *promptAtStartupCheck;
	IBOutlet NSButton *processLDConfigCheck;
	IBOutlet NSTextField *customConfigPathField;
	IBOutlet NSButton *randomColorsCheck;
	IBOutlet NSButton *showErrorsCheck;
	IBOutlet NSButton *showFrameRateCheck;
	IBOutlet NSButton *showAxesCheck;
	IBOutlet NSButton *transparentDefaultCheck;
	
	IBOutlet NSPopUpButton *snapshotsDirPopUp;
	IBOutlet NSPopUpButton *partsListsDirPopUp;
	IBOutlet NSPopUpButton *exportsDirPopUp;
	IBOutlet NSTextField *snapshotsDirField;
	IBOutlet NSTextField *partsListsDirField;
	IBOutlet NSTextField *exportsDirField;
	IBOutlet NSButton *snapshotsBrowseButton;
	IBOutlet NSButton *partsListsBrowseButton;
	IBOutlet NSButton *exportsBrowseButton;

	//NSString *snapshotsDir;
	//NSString *partsListsDir;
	NSArray *saveDirPopUps;
	NSArray *saveDirFields;
	NSArray *saveDirButtons;
	NSMutableArray *saveDirs;
}

- (bool)showErrorsIfNeeded;
- (bool)promptAtStartup;
- (bool)newModelWindows;

- (void)setup;
- (bool)updateLdPreferences;

- (int)saveDirModeForOp:(LDPreferences::SaveOp)op;
- (NSString *)saveDirForOp:(LDPreferences::SaveOp)op;
- (NSString *)defaultSaveDirForOp:(LDPreferences::SaveOp)op modelFilename:(NSString *)filename;

@end
