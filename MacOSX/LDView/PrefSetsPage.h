/* PrefSetsPage */

#import <Cocoa/Cocoa.h>

#import "PreferencePage.h"

@class PrefSetHotKeySheet;
@class PrefSetNewSheet;

@interface PrefSetsPage : PreferencePage
{
	IBOutlet NSButton *deleteButton;
	IBOutlet NSButton *hotKeyButton;
	IBOutlet NSButton *newButton;
	IBOutlet NSTableView *tableView;
	
	NSMutableArray *sessionNames;
	NSMutableDictionary *hotKeys;
	PrefSetHotKeySheet *hotKeySheet;
	PrefSetNewSheet *newSheet;
	NSString *defaultString;
}

- (IBAction)delete:(id)sender;
- (IBAction)hotKey:(id)sender;
- (IBAction)new:(id)sender;
- (IBAction)prefSetSelected:(id)sender;
- (void)prefSetSelected;

- (bool)updateLdPreferences;
- (void)setup;
- (NSString *)hotKeyLabel:(int)index;
- (void)hotKeyPressed:(int)index;
- (BOOL)canSwitchPages;

@end
