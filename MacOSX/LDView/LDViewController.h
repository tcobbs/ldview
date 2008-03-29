/* LDViewController */

#import <Cocoa/Cocoa.h>

@class Preferences;
@class ModelWindow;

@interface LDViewController : NSResponder
{
	IBOutlet NSMenuItem *statusBarMenuItem;
	IBOutlet NSMenuItem *toolbarMenuItem;
	IBOutlet NSMenuItem *custToolbarMenuItem;
	IBOutlet NSMenuItem *alwaysOnTopMenuItem;
	IBOutlet NSMenuItem *examineMenuItem;
	IBOutlet NSMenuItem *latLongRotationMenuItem;
	IBOutlet NSMenuItem *flyThroughMenuItem;
	IBOutlet NSMenuItem *cancelMenuItem;
	IBOutlet NSMenu *fileMenu;
	IBOutlet NSMenu *viewingAngleMenu;

	NSArray *ldrawFileTypes;
	NSMutableArray *modelWindows;
	Preferences *preferences;

	BOOL launchFileOpened;
	//BOOL showStatusBar;
	
	NSString *statusBarMenuFormat;
	NSString *toolbarMenuFormat;
}

- (NSMenu *)viewingAngleMenu;
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
- (ModelWindow *)currentModelWindow;

@end
