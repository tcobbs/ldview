/* PrefSetHotKeyPanel */

#import <Cocoa/Cocoa.h>
#import "GenericSheet.h"

@class PrefSetsPage;

@interface PrefSetHotKeySheet : GenericSheet
{
    IBOutlet NSPopUpButton *popUp;
	
	PrefSetsPage *parent;
}

- (id)initWithParent:(PrefSetsPage *)value;
- (NSNumber *)getHotKey:(NSNumber *)initialValue;

@end
