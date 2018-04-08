/* BoundingBox */

#import <Cocoa/Cocoa.h>

@class ModelWindow;

@interface BoundingBox : NSObject
{
    IBOutlet NSTextField *maxField;
    IBOutlet NSTextField *minField;
    IBOutlet NSPanel *panel;
	
	ModelWindow *modelWindow;
	NSArray *topLevelObjects;
}

+ (id)sharedInstance;
+ (BOOL)sharedInstanceIsVisible;

- (IBAction)show:(id)sender;
- (void)update:(ModelWindow *)modelWindow;
- (BOOL)isVisible;

@end
