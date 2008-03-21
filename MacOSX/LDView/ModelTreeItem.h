//
//  ModelTreeItem.h
//  LDView
//
//  Created by Travis Cobbs on 3/17/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

class LDModelTree;

@interface ModelTreeItem : NSObject
{
	NSString *string;
	const LDModelTree *modelTree;
	NSMutableArray *children;
}

- (id)initWithModelTree:(const LDModelTree *)value;
- (int)numberOfChildren;				// Returns 0 for leaf nodes
- (ModelTreeItem *)childAtIndex:(int)index;	// Invalid to call on leaf nodes
- (id)objectValue;
- (const LDModelTree *)modelTree;
- (NSString *)stringValue;

@end
