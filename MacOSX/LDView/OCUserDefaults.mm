//
//  OCUserDefaults.m
//  LDView
//
//  Created by Travis Cobbs on 7/8/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import "OCUserDefaults.h"
#include <TCFoundation/TCUserDefaults.h>

@implementation OCUserDefaults

+ (void)setAppName:(NSString *)value
{
	TCUserDefaults::setAppName([value cStringUsingEncoding:
		NSASCIIStringEncoding]);
}

+ (NSString *)stringForKey:(NSString *)key
{
	return [self stringForKey:key defaultValue:nil sessionSpecific:YES];
}

+ (NSString *)stringForKey:(NSString *)key
			  defaultValue:(NSString *)defaultValue
		   sessionSpecific:(BOOL)sessionSpecific
{
	char *string = TCUserDefaults::stringForKey([key cStringUsingEncoding:
		NSASCIIStringEncoding], [defaultValue cStringUsingEncoding:
		NSASCIIStringEncoding], sessionSpecific ? true : false);
		
	if (string)
	{
		NSString *retValue = [NSString stringWithCString:string encoding:
			NSASCIIStringEncoding];

		delete string;
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
	TCUserDefaults::setStringForKey([value cStringUsingEncoding:
		NSASCIIStringEncoding], [key cStringUsingEncoding:
		NSASCIIStringEncoding], sessionSpecific);
}

+ (long)longForKey:(NSString *)key
{
	return [self longForKey:key defaultValue:0 sessionSpecific:YES];
}

+ (long)longForKey:(NSString *)key
	  defaultValue:(long)defaultValue
   sessionSpecific:(BOOL)sessionSpecific
{
	return TCUserDefaults::longForKey([key cStringUsingEncoding:
		NSASCIIStringEncoding], defaultValue, sessionSpecific ? true : false);
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
	 [key cStringUsingEncoding:NSASCIIStringEncoding], sessionSpecific);
}


@end
