//
//  LDViewCategories.mm
//  LDView
//
//  Created by Travis Cobbs on 3/22/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LDViewCategories.h"


@implementation NSButton(LDView)

- (void)setCheck:(bool)value
{
	[self setState:value ? NSOnState : NSOffState];
}

- (bool)getCheck
{
	return [self state] == NSOnState;
}

@end

@implementation NSString(LDView)

+ (id)stringWithUCString:(const ucstring &)ucstring
{
	return [[self alloc] initWithUCString:ucstring];
}

- (id)initWithUCString:(const ucstring &)ucstring
{
#ifdef TC_NO_UNICODE
	std::wstring wstring;
	
	stringtowstring(wstring, ucstring);
#else // TC_NO_UNICODE
	const std::wstring &wstring = ucstring;
#endif // TC_NO_UNICODE
	int len = wstring.size();
	int i;
	unichar *uniChars = new unichar[len];
	id retValue;

	for (i = 0; i < len; i++)
	{
		uniChars[i] = (unichar)wstring[i];
	}
	retValue = [self initWithCharacters:uniChars length:len];
	delete uniChars;
	return retValue;
}

- (const char *)asciiCString
{
	return [self cStringUsingEncoding:NSASCIIStringEncoding];
}

+ (id)stringWithASCIICString:(const char *)cString
{
	return [self stringWithCString:cString encoding:NSASCIIStringEncoding];
}

- (id)initWithASCIICString:(const char *)cString
{
	return [self initWithCString:cString encoding:NSASCIIStringEncoding];
}

@end
