/* Updater */

#import <Cocoa/Cocoa.h>

class TCProgressAlert;
class LDLibraryUpdater;
class UpdaterAlertHandler;

@interface Updater : NSObject
{
	IBOutlet NSButton *cancelButton;
	IBOutlet NSButton *okButton;
	IBOutlet NSProgressIndicator *progress;
	IBOutlet NSTextField *textField;
	IBOutlet NSPanel *panel;
	
	BOOL canceled;
	BOOL done;
	BOOL error;
	LDLibraryUpdater *updater;
	UpdaterAlertHandler *alertHandler;
	NSArray *topLevelObjects;
}

- (id)init;
- (bool)downloadLDraw:(NSString *)targetDir;
- (bool)checkForUpdates:(NSString *)targetDir isParent:(bool)isParent;
- (void)progressCallback:(TCProgressAlert *)alert;

- (IBAction)cancel:(id)sender;
- (IBAction)ok:(id)sender;

@end
