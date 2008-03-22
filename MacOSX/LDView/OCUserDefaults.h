//
//  OCUserDefaults.h
//  LDView
//
//  Created by Travis Cobbs on 7/8/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface OCUserDefaults : NSObject {

}

typedef char *argvArray[];

+ (void)initSession;
+ (void)setAppName:(NSString *)value;
+ (void)setCommandLine:(argvArray)argv;
+ (NSString *)stringForKey:(NSString *)key;
+ (NSString *)stringForKey:(NSString *)key
			  defaultValue:(NSString *)defaultValue
		   sessionSpecific:(BOOL)sessionSpecific;
+ (void)setString:(NSString *)value forKey:(NSString *)key;
+ (void)setString:(NSString *)value
		   forKey:(NSString *)key
  sessionSpecific:(BOOL)sessionSpecific;
+ (long)longForKey:(NSString *)key;
+ (long)longForKey:(NSString *)key
	  defaultValue:(long)defaultValue
   sessionSpecific:(BOOL)sessionSpecific;
+ (void)setLong:(long)value forKey:(NSString *)key;
+ (void)setLong:(long)value
		 forKey:(NSString *)key
sessionSpecific:(BOOL)sessionSpecific;
+ (float)floatForKey:(NSString *)key;
+ (float)floatForKey:(NSString *)key
	  defaultValue:(float)defaultValue
   sessionSpecific:(BOOL)sessionSpecific;
+ (void)setFloat:(float)value forKey:(NSString *)key;
+ (void)setFloat:(float)value
		 forKey:(NSString *)key
sessionSpecific:(BOOL)sessionSpecific;
+ (void)removeValueForKey:(NSString *)key
		  sessionSpecific:(BOOL)sessionSpecific;

@end
