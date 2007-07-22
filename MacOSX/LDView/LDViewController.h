/* LDViewController */

#import <Cocoa/Cocoa.h>

@class Preferences;

@interface LDViewController : NSObject
{
	NSArray *ldrawFileTypes;
	NSMutableArray *modelWindows;
	Preferences *preferences;

	BOOL launchFileOpened;
}

- (IBAction)newWindow:(id)sender;
- (IBAction)openModel:(id)sender;
- (IBAction)preferences:(id)sender;
- (Preferences *)preferences;
- (IBAction)resetView:(id)sender;
- (NSArray *)modelWindows;

@end
