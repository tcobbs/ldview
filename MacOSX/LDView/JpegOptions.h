/* JpegOptions */

#import <Cocoa/Cocoa.h>

class TCJpegOptions;

@interface JpegOptions : NSObject
{
	IBOutlet NSPanel *panel;
    IBOutlet NSButton *progressiveButton;
    IBOutlet NSTextField *qualityField;
    IBOutlet NSSlider *qualitySlider;
    IBOutlet NSPopUpButton *subSamplingPopUp;
	
	TCJpegOptions *options;
	NSArray *topLevelObjects;
}
- (id)init;

- (IBAction)cancel:(id)sender;
- (IBAction)ok:(id)sender;

- (void)runModal;

@end
