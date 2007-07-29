/* PrefSetHotKeyPanel */

#import <Cocoa/Cocoa.h>

@class PrefSetsPage;

@interface PrefSetHotKeyPanel : NSObject
{
    IBOutlet NSPanel *panel;
    IBOutlet NSPopUpButton *popUp;
	
	PrefSetsPage *parent;
	BOOL okPressed;
}

- (IBAction)cancel:(id)sender;
- (IBAction)ok:(id)sender;

- (id)initWithParent:(PrefSetsPage *)value;
- (NSNumber *)getHotKey:(NSNumber *)initialValue;

@end
