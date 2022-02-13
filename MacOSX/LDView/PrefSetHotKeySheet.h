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
- (void)getHotKey:(NSNumber *)initialValue withCompletion:(void (^)(NSNumber *value))handler;

@end
