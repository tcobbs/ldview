/* ErrorsAndWarnings */

#import <Cocoa/Cocoa.h>

@class ModelWindow;
@class ErrorItem;

extern NSString *LDErrorFilterChange;

@interface ErrorsAndWarnings : NSObject
{
	IBOutlet NSButton *copyErrorButton;
	IBOutlet NSTableView *enabledErrorsTable;
	IBOutlet NSOutlineView *errorsOutline;
	IBOutlet NSButton *includeWarningsButton;
	IBOutlet NSPanel *panel;
	IBOutlet NSTextField *statusField;
	
	NSMutableArray *errorNames;
	NSMutableArray *enabledErrors;
	ErrorItem *rootErrorItem;
	NSString *titleFormat;
	int errors;
	int warnings;
	NSArray *topLevelObjects;
}

- (id)init;
+ (id)sharedInstance;
+ (BOOL)sharedInstanceIsVisible;

- (IBAction)showIfNeeded;
- (IBAction)copyError:(id)sender;
- (IBAction)errorSelected:(id)sender;
- (IBAction)includeWarnings:(id)sender;
- (IBAction)showAll:(id)sender;
- (IBAction)showNone:(id)sender;
- (IBAction)show:(id)sender;
- (IBAction)enabledErrorSelected:(id)sender;

- (void)setRootErrorItem:(ErrorItem *)item;
- (ErrorItem *)filteredRootErrorItem:(ErrorItem *)unfilteredRoot;
- (BOOL)isVisible;
- (void)update:(ModelWindow *)modelWindow;

@end
