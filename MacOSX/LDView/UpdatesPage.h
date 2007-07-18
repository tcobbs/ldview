/* UpdatesPage */

#import <Cocoa/Cocoa.h>

#import "PreferencePage.h"

@interface UpdatesPage : PreferencePage
{
    IBOutlet NSButton *automaticCheck;
    IBOutlet NSTextField *missingDaysField;
    IBOutlet NSTextField *missingUpdatesField;
    IBOutlet NSTextField *portField;
    IBOutlet NSTextField *proxyField;
    IBOutlet NSMatrix *proxyMatrix;
}
@end
