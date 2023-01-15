//
//  ModelTreeItem.mm
//  LDView
//
//  Created by Travis Cobbs on 3/17/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "ModelTreeItem.h"

#include <LDLib/LDModelTree.h>

@implementation ModelTreeItem

- (id)initWithModelTree:(const LDModelTree *)value
{
	self = [super init];
	if (self != nil)
	{
		modelTree = value;
		string = [[NSString alloc] initWithUTF8String:modelTree->getText().c_str()];
		if (string == nil)
		{
			string = [[NSString alloc] initWithCString:modelTree->getText().c_str() encoding:NSASCIIStringEncoding];
		}
	}
	return self;
}

- (void) dealloc
{
	[string release];
	[children release];
	[super dealloc];
}

- (size_t)numberOfChildren
{
	return modelTree->getNumChildren();
}

- (ModelTreeItem *)childAtIndex:(NSInteger)index
{
	if (!children)
	{
		children = [[NSMutableArray alloc] init];
		for (size_t i = 0; i < modelTree->getNumChildren(); i++)
		{
			ModelTreeItem *child = [[ModelTreeItem alloc] initWithModelTree:(*modelTree->getChildren())[i]];
			[children addObject:child];
			[child release];
		}
	}
	return [children objectAtIndex:index];
}

- (id)objectValue
{
	return string;
}

- (const LDModelTree *)modelTree
{
	return modelTree;
}

- (NSString *)stringValue
{
	return string;
}

@end
