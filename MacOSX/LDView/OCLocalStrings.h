//
//  OCLocalStrings.h
//  LDView
//
//  Created by Travis Cobbs on 7/8/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface OCLocalStrings : NSObject {
}

+ (NSString *)get:(NSString *)key;
+ (BOOL)loadStringTable:(NSString *)filename replace:(bool)replace;

@end
