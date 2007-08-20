/* ErrorsAndWarnings */

#import <Cocoa/Cocoa.h>

@class ModelWindow;
@class ErrorItem;

@interface ErrorsAndWarnings : NSObject
{
    IBOutlet NSButton *copyErrorButton;
    IBOutlet NSTableView *enabledErrorsTable;
    IBOutlet NSOutlineView *errorsOutline;
    IBOutlet NSButton *includeWarningsButton;
	IBOutlet NSPanel *panel;
    IBOutlet NSTextField *statusField;
	
	ModelWindow *modelWindow;
	NSMutableArray *errorNames;
	NSMutableArray *enabledErrors;
	ErrorItem *rootErrorItem;
}

- (id)init;

- (IBAction)copyError:(id)sender;
- (IBAction)errorSelected:(id)sender;
- (IBAction)includeWarnings:(id)sender;
- (IBAction)showAll:(id)sender;
- (IBAction)showNone:(id)sender;
- (IBAction)show:(id)sender;
- (IBAction)enabledErrorSelected:(id)sender;

- (void)setRootErrorItem:(ErrorItem *)item;

@end
