/* PreferencePage */

#import <Cocoa/Cocoa.h>

class LDPreferences;
@class Preferences;

@interface PreferencePage : NSObject
{
	Preferences *preferences;
	LDPreferences *ldPreferences;

	IBOutlet NSTabViewItem *tabPage;
	NSMutableDictionary *origColors;
	NSMutableDictionary *origBoxTitles;
	NSMutableDictionary *disabledBoxTitles;
	NSNumber *one;
}

- (Preferences *)preferences;
- (void)apply;
- (void)updateLdPreferences;
- (IBAction)resetPage:(id)sender;
- (IBAction)valueChanged:(id)sender;
- (void)setPreferences:(Preferences *)value;
- (void)setup;
- (void)setCheck:(NSButton *)check value:(bool)value;
- (bool)getCheck:(NSButton *)check;
- (void)setColorWell:(NSColorWell *)colorWell r:(int)r g:(int)g b:(int)b;
- (void)getColorWell:(NSColorWell *)colorWell r:(int *)r g:(int *)g b:(int *)b a:(int *)a;
- (void)getColorWell:(NSColorWell *)colorWell r:(int *)r g:(int *)g b:(int *)b;
- (void)enableLabel:(NSTextField *)label value:(BOOL)enabled;
- (void)enableBoxTitle:(NSBox *)box value:(BOOL)enabled;
- (void)setUISection:(NSString *)sectionName enabled:(BOOL)enabled;
- (void)groupCheck:(id)sender name:(NSString *)groupName;
- (void)groupCheck:(id)sender name:(NSString *)groupName value:(bool)value;
- (void)groupCheck:(id)sender name:(NSString *)groupName init:(BOOL)init;
- (BOOL)canSwitchPages;

@end
