/* GoToStep */

#import "GenericSheet.h"

@interface GoToStep : GenericSheet
{
	IBOutlet id okButton;
	IBOutlet NSTextField *stepField;
	
	int step;
}

- (id)init;
- (int)step;

@end
