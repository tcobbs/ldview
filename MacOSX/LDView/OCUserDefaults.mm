//
//  OCUserDefaults.m
//  LDView
//
//  Created by Travis Cobbs on 7/8/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import "OCUserDefaults.h"
#import "LDViewCategories.h"
#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDUserDefaultsKeys.h>

@implementation OCUserDefaults

+ (void)setAppName:(NSString *)value
{
	TCUserDefaults::setAppName([value UTF8String]);
	setDebugLevel((int)TCUserDefaults::longForKey(DEBUG_LEVEL_KEY));
}

+ (void)setCommandLine:(argvArray)argv
{
	TCUserDefaults::setCommandLine(argv);
}

+ (NSString *)stringForKey:(NSString *)key
{
	return [self stringForKey:key defaultValue:nil sessionSpecific:YES];
}

+ (NSString *)stringForKey:(NSString *)key
			  defaultValue:(NSString *)defaultValue
		   sessionSpecific:(BOOL)sessionSpecific
{
	char *string = TCUserDefaults::stringForKey([key UTF8String], defaultValue ? [defaultValue UTF8String] : NULL, sessionSpecific ? true : false);
		
	if (string)
	{
		NSString *retValue = [NSString stringWithUTF8String:string];

		delete[] string;
		return retValue;
	}
	else
	{
		return nil;
	}
}

+ (void)setString:(NSString *)value forKey:(NSString *)key
{
	return [self setString:value forKey:key sessionSpecific:YES];
}

+ (void)setString:(NSString *)value
		   forKey:(NSString *)key
  sessionSpecific:(BOOL)sessionSpecific
{
	TCUserDefaults::setStringForKey([value UTF8String], [key UTF8String], sessionSpecific);
}

+ (long)longForKey:(NSString *)key
{
	return [self longForKey:key defaultValue:0 sessionSpecific:YES];
}

+ (long)longForKey:(NSString *)key
	  defaultValue:(long)defaultValue
   sessionSpecific:(BOOL)sessionSpecific
{
	return TCUserDefaults::longForKey([key UTF8String], defaultValue, sessionSpecific ? true : false);
}

+ (void)setLong:(long)value forKey:(NSString *)key
{
	return [self setLong:value forKey:key sessionSpecific:YES];
}

+ (void)setLong:(long)value
		 forKey:(NSString *)key
sessionSpecific:(BOOL)sessionSpecific
{
	TCUserDefaults::setLongForKey(value,
	 [key UTF8String], sessionSpecific);
}

+ (float)floatForKey:(NSString *)key
{
	return [self floatForKey:key defaultValue:0 sessionSpecific:YES];
}

+ (float)floatForKey:(NSString *)key
	  defaultValue:(float)defaultValue
   sessionSpecific:(BOOL)sessionSpecific
{
	return TCUserDefaults::floatForKey([key UTF8String], defaultValue, sessionSpecific ? true : false);
}

+ (void)setFloat:(float)value forKey:(NSString *)key
{
	return [self setFloat:value forKey:key sessionSpecific:YES];
}

+ (void)setFloat:(float)value
		 forKey:(NSString *)key
sessionSpecific:(BOOL)sessionSpecific
{
	TCUserDefaults::setFloatForKey(value, [key UTF8String], sessionSpecific);
}

+ (void)removeValueForKey:(NSString *)key
		  sessionSpecific:(BOOL)sessionSpecific
{
	TCUserDefaults::removeValue([key UTF8String], sessionSpecific);
}

+ (void)initSession
{
	char *sessionName = TCUserDefaults::getSavedSessionNameFromKey(PREFERENCE_SET_KEY);
	if (sessionName && sessionName[0])
	{
		TCUserDefaults::setSessionName(sessionName, NULL, false);
	}
	delete[] sessionName;
}

@end
