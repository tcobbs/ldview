/* LDrawPage */

#import <Cocoa/Cocoa.h>
#import "PreferencePage.h"
#import "Preferences.h"
#import "TableViewReorder.h"

@interface LDrawPage : PreferencePage <TableViewReorder>
{
	IBOutlet NSSegmentedControl *addRemoveExtraFolder;
	IBOutlet NSTableView *extraFoldersTableView;
	IBOutlet NSButton *generateThumbnailsCheck;
	IBOutlet id ldrawDirField;

	NSMutableArray *extraFolders;
	TableViewReorder *tableViewReorder;
}

- (void)setup;
- (bool)updateLdPreferences;

+ (bool)verifyLDrawDir:(NSString *)ldrawDir;
- (void)updateLDrawDir:(NSString *)ldrawDir;

- (NSString *)ldrawDir;

- (IBAction)addRemoveExtraFolder:(id)sender;
- (IBAction)extraFolderSelected:(id)sender;
- (IBAction)ldrawFolderBrowse:(id)sender;
- (NSMutableArray *)tableRows:(TableViewReorder *)sender;

@end
