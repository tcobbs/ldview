/* LDViewController */

#import <Cocoa/Cocoa.h>

@class Preferences;

@interface LDViewController : NSObject
{
	IBOutlet NSMenuItem *statusBarMenuItem;
	
	NSArray *ldrawFileTypes;
	NSMutableArray *modelWindows;
	Preferences *preferences;

	BOOL launchFileOpened;
	BOOL showStatusBar;
}

- (IBAction)newWindow:(id)sender;
- (IBAction)openModel:(id)sender;
- (IBAction)preferences:(id)sender;
- (Preferences *)preferences;
- (IBAction)resetView:(id)sender;
- (IBAction)toggleStatusBar:(id)sender;
- (NSArray *)modelWindows;

@end
