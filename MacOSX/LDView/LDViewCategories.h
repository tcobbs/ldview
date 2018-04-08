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

@interface NSOpenPanel(LDView)

- (NSString *)ldvFilename;

@end

@interface NSSavePanel(LDView)

- (NSString *)ldvFilename;

@end

@interface NSURL(LDView)

@property (readonly) const char *ldvFileSystemRepresentation NS_RETURNS_INNER_POINTER;

@end

@interface NSBundle(LDView)

- (BOOL)ldvLoadNibNamed:(NSNibName)nibName owner:(id)owner topLevelObjects:(NSArray **)topLevelObjects;

@end

@interface NSObject(LDView)

- (BOOL)haveTopLevelObjectsArray;
- (BOOL)releaseTopLevelObjects:(NSArray *)topLevelObjects;
- (void)releaseTopLevelObjects:(NSArray *)topLevelObjects orTopLevelObject:(id)topLevelObject;
- (BOOL)ldvLoadNibNamed:(NSNibName)nibName topLevelObjects:(NSArray **)topLevelObjects;

@end
