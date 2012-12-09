//
//  LDViewCategories.mm
//  LDView
//
//  Created by Travis Cobbs on 3/22/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LDViewCategories.h"
#import "MacSetup.h"

@implementation NSButton(LDView)

- (void)setCheck:(bool)value
{
	[self setState:value ? NSOnState : NSOffState];
}

- (bool)getCheck
{
	return [self state] == NSOnState;
}

@end // NSButton(LDView)

@implementation NSColorWell(LDView)

- (void)setR:(int)r g:(int)g b:(int)b
{
	[self setColor:[NSColor colorWithCalibratedRed:r / 255.0 green:g / 255.0 blue:b / 255.0 alpha:1.0f]];
}

- (void)getR:(int *)r g:(int *)g b:(int *)b
{
	[self getR:r g:g b:b a:nil];
}

- (void)getR:(int *)r g:(int *)g b:(int *)b a:(int *)a
{
	CGFloat rf, gf, bf, af;
	
	[[[self color] colorUsingColorSpaceName:NSCalibratedRGBColorSpace] getRed:&rf green:&gf blue:&bf alpha:&af];
	*r = (int)(rf * 255.0 + 0.5);
	*g = (int)(gf * 255.0 + 0.5);
	*b = (int)(bf * 255.0 + 0.5);
	if (a)
	{
		*a = (int)(af * 255.0 + 0.5);
	}
}

@end // NSColorWell(LDView)

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

- (ucstring)ucString
{
#ifdef TC_NO_UNICODE
	return [self asciiCString];
#else // TC_NO_UNICODE
	int i;
	size_t len = [self length];
	unichar *characters = new unichar[len];
	std::wstring retValue;

	[self getCharacters:characters];
	retValue.resize(len);
	for (i = 0; i < len; i++)
	{
		retValue[i] = (wchar_t)characters[i];
	}
	delete characters;
	return retValue;
#endif // TC_NO_UNICODE
}

+ (id)stringWithASCIICString:(const char *)cString
{
	return [self stringWithCString:cString encoding:NSASCIIStringEncoding];
}

- (id)initWithASCIICString:(const char *)cString
{
	return [self initWithCString:cString encoding:NSASCIIStringEncoding];
}

@end // NSString(LDView)
