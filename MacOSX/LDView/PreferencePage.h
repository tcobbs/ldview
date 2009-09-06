/* PreferencePage */

#import <Cocoa/Cocoa.h>
#import "ViewOwnerBase.h"

class LDPreferences;
@class Preferences;

@interface PreferencePage : ViewOwnerBase
{
	Preferences *preferences;
	LDPreferences *ldPreferences;

	IBOutlet NSTabViewItem *tabPage;
	NSNumber *one;
}

- (Preferences *)preferences;
- (void)apply;
- (bool)updateLdPreferences;
- (IBAction)resetPage:(id)sender;
- (IBAction)valueChanged:(id)sender;
- (void)setPreferences:(Preferences *)value;
- (void)setup;
- (BOOL)canSwitchPages;
- (void)browseForFolder:(void *)contextInfo;
- (void)browseForFolder:(void *)contextInfo initialDir:(NSString *)dir;
- (void)textDidChange:(NSNotification *)aNotification;

@end
