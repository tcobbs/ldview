/* PreferencePage */

#import <Cocoa/Cocoa.h>

class LDPreferences;
@class Preferences;

@interface PreferencePage : NSObject
{
	Preferences *preferences;
	LDPreferences *ldPreferences;

	IBOutlet NSTabViewItem *tabPage;
	NSMutableDictionary *changedTextFields;
	NSMutableDictionary *origColors;
	NSMutableDictionary *origBoxTitles;
	NSMutableDictionary *disabledBoxTitles;
	NSNumber *one;
}
- (void)apply;
- (void)updateLdPreferences;
- (IBAction)resetPage:(id)sender;
- (IBAction)valueChanged:(id)sender;
- (void)setPreferences:(Preferences *)value;
- (void)setup;
- (void)setCheck:(NSButton *)check value:(bool)value;
- (bool)getCheck:(NSButton *)check;
- (void)enableLabel:(NSTextField *)label value:(BOOL)enabled;
- (void)enableBoxTitle:(NSBox *)box value:(BOOL)enabled;
- (void)groupCheck:(id)sender name:(NSString *)groupName;
- (void)groupCheck:(id)sender name:(NSString *)groupName init:(BOOL)init;
- (void)textDidEndEditing:(NSNotification *)aNotification;
@end
