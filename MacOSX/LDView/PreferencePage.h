/* PreferencePage */

#import <Cocoa/Cocoa.h>

class LDPreferences;
@class Preferences;

@interface PreferencePage : NSObject
{
	Preferences *preferences;
	LDPreferences *ldPreferences;
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
- (void)groupCheck:(id)sender name:(NSString *)groupName;
@end
