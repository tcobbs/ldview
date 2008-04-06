//
//  AutoDeleter.h
//  LDView
//
//  Created by Travis Cobbs on 4/5/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

// Class designed to let you get C++ pointers to be autoreleased.  Since delete
// of void* has undefined behavior, each type that needs to be supported has to
// have specific support added.  Currently, only BYTE data is supported.
@interface AutoDeleter : NSObject
{
	unsigned char *bytePointer;
}

- (id)initWithBytePointer:(unsigned char *)pointer;
+ (id)autoDeleterWithBytePointer:(unsigned char *)pointer;

@end
