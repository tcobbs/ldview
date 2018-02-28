//
//  AutoDeleter.mm
//  LDView
//
//  Created by Travis Cobbs on 4/5/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "AutoDeleter.h"


@implementation AutoDeleter

- (id)initWithBytePointer:(unsigned char *)pointer
{
	self = [super init];
	if (self != nil)
	{
		bytePointer = pointer;
	}
	return self;
}

+ (id)autoDeleterWithBytePointer:(unsigned char *)pointer
{
	return [[[self alloc] initWithBytePointer:pointer] autorelease];
}

- (void)dealloc
{
	delete[] bytePointer;
	[super dealloc];
}

@end
