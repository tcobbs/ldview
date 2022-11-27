/* PartsList */

#import <Cocoa/Cocoa.h>
#import "GenericSheet.h"

@class ModelWindow;

class LDHtmlInventory;

@interface PartsList : GenericSheet
{
	IBOutlet NSButton *externalStyleButton;
	IBOutlet NSButton *showImagesButton;
	IBOutlet NSButton *showModelButton;
	IBOutlet NSButton *showWebPageButton;
	IBOutlet NSTableView *tableView;
	
	ModelWindow *modelWindow;
	LDHtmlInventory *htmlInventory;
	NSMutableArray *columns;
}

- (id)initWithModelWindow:(ModelWindow *)theModelWindow htmlInventory:(LDHtmlInventory *)theHtmlInventory;

- (IBAction)columnSelected:(id)sender;
- (IBAction)ok:(id)sender;

@end
