//
//  ErrorItem.h
//  LDView
//
//  Created by Travis Cobbs on 8/19/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MacSetup.h"

class LDLError;

@interface ErrorItem : NSObject {
	NSString *string;
	id objectValue;
	NSMutableArray *children;
	LDLError *error;
}

- (NSString *)stringValue;
- (id)initWithString:(NSString *)value error:(LDLError *)error includeIcon:(BOOL)includeIcon;
- (id)initWithAttributedString:(NSAttributedString *)value error:(LDLError *)error;
- (NSInteger)numberOfChildren;				// Returns -1 for leaf nodes
- (ErrorItem *)childAtIndex:(NSInteger)index;	// Invalid to call on leaf nodes
- (id)objectValue;
- (ErrorItem *)addChild:(ErrorItem *)child;
- (LDLError *)error;

@end
