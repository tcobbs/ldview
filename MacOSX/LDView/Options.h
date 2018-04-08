/* Options */

#import <Cocoa/Cocoa.h>
#import "MacSetup.h"

#include <LDExporter/LDExporterSetting.h>

@class OptionsPanel;
@class OptionUI;

@interface Options : NSObject
{
    IBOutlet OptionsPanel *panel;
    IBOutlet NSScrollView *scrollView;
	IBOutlet NSButton *okButton;
	IBOutlet NSButton *cancelButton;
	IBOutlet NSView *docView;
	LDExporterSettingList *settings;
	NSMutableArray *optionUIs;
	CGFloat margin;
	CGFloat spacing;
	NSArray *topLevelObjects;
}

- (NSInteger)runModalWithSettings:(LDExporterSettingList &)theSettings titlePrefix:(NSString *)titlePrefix;

- (IBAction)ok:(id)sender;
- (IBAction)cancel:(id)sender;
- (IBAction)resetAll:(id)sender;
- (id)docView;
- (void)updateEnabled;
- (void)makeOptionUIVisible:(OptionUI *)optionUI;

@end
