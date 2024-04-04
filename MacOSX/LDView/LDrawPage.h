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
	IBOutlet id ldrawZipField;

	NSMutableArray *extraFolders;
	TableViewReorder *tableViewReorder;
}

- (void)setup;
- (bool)updateLdPreferences;

+ (bool)verifyLDrawDir:(NSString *)ldrawDir ldPreferences:(LDPreferences*)ldPreferences;
- (void)updateLDrawDir:(NSString *)ldrawDir;

- (NSString *)ldrawDir;
- (NSString *)ldrawZipPath;

- (IBAction)addRemoveExtraFolder:(id)sender;
- (IBAction)extraFolderSelected:(id)sender;
- (IBAction)ldrawFolderBrowse:(id)sender;
- (IBAction)ldrawZipBrowse:(id)sender;
- (NSMutableArray *)tableRows:(TableViewReorder *)sender;

@end
