//
//  OCLocalStrings.mm
//  LDView
//
//  Created by Travis Cobbs on 7/8/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import "OCLocalStrings.h"
#import "LDViewCategories.h"

#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/mystring.h>


@implementation OCLocalStrings

+ (NSString *)get:(NSString *)key
{
	CUCSTR string = TCLocalStrings::get([key ucString].c_str());

	if (string)
	{
		return [NSString stringWithUCString:string];
	}
	else
	{
		return nil;
	}
}

+ (BOOL)loadStringTable:(NSString *)filename replace:(bool)replace
{
	if (TCLocalStrings::loadStringTable([filename UTF8String], replace))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

@end
