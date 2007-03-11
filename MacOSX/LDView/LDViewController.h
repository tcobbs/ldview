/* LDViewController */

#import <Cocoa/Cocoa.h>

@interface LDViewController : NSObject
{
	NSArray *ldrawFileTypes;

	BOOL launchFileOpened;
}

- (IBAction)newWindow:(id)sender;
- (IBAction)openModel:(id)sender;
- (IBAction)resetView:(id)sender;

@end
