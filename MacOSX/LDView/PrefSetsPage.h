/* PrefSetsPage */

#import <Cocoa/Cocoa.h>

#import "PreferencePage.h"

@class PrefSetHotKeyPanel;

@interface PrefSetsPage : PreferencePage
{
    IBOutlet NSButton *deleteButton;
    IBOutlet NSButton *hotKeyButton;
    IBOutlet NSButton *newButton;
    IBOutlet NSTableView *tableView;
	
	NSMutableArray *sessionNames;
	NSMutableDictionary *hotKeys;
	PrefSetHotKeyPanel *hotKeyPanel;
}

- (IBAction)delete:(id)sender;
- (IBAction)hotKey:(id)sender;
- (IBAction)new:(id)sender;
- (IBAction)prefSetSelected:(id)sender;

- (void)updateLdPreferences;
- (void)setup;
- (NSString *)hotKeyLabel:(int)index;

@end
