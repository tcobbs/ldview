/* Options */

#import <Cocoa/Cocoa.h>

#include <LDExporter/LDExporterSetting.h>

@interface Options : NSObject
{
    IBOutlet NSPanel *panel;
    IBOutlet NSScrollView *scrollView;
	IBOutlet NSButton *okButton;
	IBOutlet NSButton *cancelButton;
	NSView *docView;
	LDExporterSettingList *settings;
	NSMutableArray *optionUIs;
	float margin;
	float spacing;
}

- (int)runModalWithSettings:(LDExporterSettingList &)theSettings;

- (IBAction)ok:(id)sender;
- (IBAction)cancel:(id)sender;
- (id)docView;
- (void)updateEnabled;

@end
