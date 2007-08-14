/* LDViewController */

#import <Cocoa/Cocoa.h>

@class Preferences;

@interface LDViewController : NSResponder
{
	IBOutlet NSMenuItem *statusBarMenuItem;
	IBOutlet NSMenuItem *toolbarMenuItem;
	IBOutlet NSMenuItem *custToolbarMenuItem;
	
	NSArray *ldrawFileTypes;
	NSMutableArray *modelWindows;
	Preferences *preferences;

	BOOL launchFileOpened;
	//BOOL showStatusBar;
	
	NSString *statusBarMenuFormat;
	NSString *toolbarMenuFormat;
}

- (IBAction)newWindow:(id)sender;
- (IBAction)openModel:(id)sender;
- (IBAction)preferences:(id)sender;
- (Preferences *)preferences;
- (IBAction)resetView:(id)sender;
//- (IBAction)toggleStatusBar:(id)sender;
//- (IBAction)toggleToolbar:(id)sender;
//- (IBAction)customizeToolbar:(id)sender;
- (NSArray *)modelWindows;
- (void)updateStatusBarMenuItem:(BOOL)showStatusBar;

@end
