/* PrefSetsPage */

#import <Cocoa/Cocoa.h>

#import "PreferencePage.h"

@interface PrefSetsPage : PreferencePage
{
    IBOutlet NSButton *deleteButton;
    IBOutlet NSButton *hotKeyButton;
    IBOutlet NSButton *newButton;
    IBOutlet NSTableView *tableView;
}
- (IBAction)delete:(id)sender;
- (IBAction)hotKey:(id)sender;
- (IBAction)new:(id)sender;
@end
