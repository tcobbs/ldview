/* ModelWindow */

#import <Cocoa/Cocoa.h>

@class LDrawModelView;

class LDLError;
class TCProgressAlert;
class AlertHandler;

@interface ModelWindow : NSObject
{
    IBOutlet NSWindow *window;
	IBOutlet LDrawModelView *modelView;
	IBOutlet NSProgressIndicator *progress;
	IBOutlet NSTextField *progressMessage;

	AlertHandler *alertHandler;
	NSToolbar *toolbar;
}

- (BOOL)openModel:(NSString *)filename;
- (id)init;
- (LDrawModelView *)modelView;

- (void)ldlErrorCallback:(LDLError *)error;
- (void)progressAlertCallback:(TCProgressAlert *)alert;

@end
