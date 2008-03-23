/* LDrawPage */

#import <Cocoa/Cocoa.h>
#import "PreferencePage.h"

@interface LDrawPage : PreferencePage
{
    IBOutlet NSSegmentedControl *addRemoveExtraFolder;
    IBOutlet NSTableView *extraFoldersTableView;
    IBOutlet id ldrawDirField;
}

- (void)setup;
- (void)updateLdPreferences;

- (IBAction)addRemoveExtraFolder:(id)sender;
- (IBAction)extraFolderSelected:(id)sender;
- (IBAction)ldrawFolderBrowse:(id)sender;

@end
