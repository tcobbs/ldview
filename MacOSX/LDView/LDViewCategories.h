//
//  LDViewCategories.h
//  LDView
//
//  Created by Travis Cobbs on 3/22/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include <TCFoundation/mystring.h>


@interface NSButton(LDView)

- (void)setCheck:(bool)value;
- (bool)getCheck;

@end

@interface NSColorWell(LDView)

- (void)setR:(int)r g:(int)g b:(int)b;
- (void)getR:(int *)r g:(int *)g b:(int *)b;
- (void)getR:(int *)r g:(int *)g b:(int *)b a:(int *)a;

@end

@interface NSString(LDView)

+ (id)stringWithUCString:(const ucstring &)ucstring;
+ (id)stringWithASCIICString:(const char *)cString;
- (const char *)asciiCString;
- (ucstring)ucString;
- (id)initWithUCString:(const ucstring &)ucstring;
- (id)initWithASCIICString:(const char *)cString;

@end