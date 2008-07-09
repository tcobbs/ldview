/* Options */

#import <Cocoa/Cocoa.h>

#include <LDExporter/LDExporterSetting.h>

@class OptionsPanel;
@class OptionUI;

@interface Options : NSObject
{
    IBOutlet OptionsPanel *panel;
    IBOutlet NSScrollView *scrollView;
	IBOutlet NSButton *okButton;
	IBOutlet NSButton *cancelButton;
	NSView *docView;
	LDExporterSettingList *settings;
	NSMutableArray *optionUIs;
	float margin;
	float spacing;
}

- (int)runModalWithSettings:(LDExporterSettingList &)theSettings titlePrefix:(NSString *)titlePrefix;

- (IBAction)ok:(id)sender;
- (IBAction)cancel:(id)sender;
- (id)docView;
- (void)updateEnabled;
- (void)makeOptionUIVisible:(OptionUI *)optionUI;

@end
