/* UpdatesPage */

#import <Cocoa/Cocoa.h>

#import "PreferencePage.h"

@interface UpdatesPage : PreferencePage
{
	IBOutlet NSButton *automaticCheck;
	IBOutlet NSTextField *missingDaysField;
	IBOutlet NSTextField *missingDaysLabel;
	IBOutlet NSTextField *updatedDaysField;
	IBOutlet NSTextField *updatedDaysLabel;
	IBOutlet NSTextField *portField;
	IBOutlet NSTextField *portLabel;
	IBOutlet NSTextField *proxyField;
	IBOutlet NSTextField *proxyLabel;
	IBOutlet NSMatrix *proxyMatrix;
}

- (IBAction)proxyType:(id)sender;
- (IBAction)resetTimes:(id)sender;

- (void)setup;
- (bool)updateLdPreferences;

@end
