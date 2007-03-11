//
//  OCLocalStrings.mm
//  LDView
//
//  Created by Travis Cobbs on 7/8/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import "OCLocalStrings.h"
#include <TCFoundation/TCLocalStrings.h>


@implementation OCLocalStrings

+ (NSString *)get:(NSString *)key
{
	const char *string = TCLocalStrings::get([key cStringUsingEncoding:
		NSASCIIStringEncoding]);

	if (string)
	{
		return [NSString stringWithCString:string
			encoding:NSASCIIStringEncoding];
	}
	else
	{
		return nil;
	}
}

+ (BOOL)loadStringTable:(NSString *)filename
{
	if (TCLocalStrings::loadStringTable([filename cStringUsingEncoding:
		NSASCIIStringEncoding]))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

@end
