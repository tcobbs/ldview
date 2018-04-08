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
	return [[[self alloc] initWithUCString:ucstring] autorelease];
}

- (id)initWithUCString:(const ucstring &)ucstring
{
#ifdef TC_NO_UNICODE
	std::wstring wstring;
	
	stringtowstring(wstring, ucstring);
#else // TC_NO_UNICODE
	const std::wstring &wstring = ucstring;
#endif // TC_NO_UNICODE
	size_t len = wstring.size();
	size_t i;
	unichar *uniChars = new unichar[len];
	id retValue;

	for (i = 0; i < len; i++)
	{
		uniChars[i] = (unichar)wstring[i];
	}
	retValue = [self initWithCharacters:uniChars length:len];
	delete[] uniChars;
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
	delete[] characters;
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


@implementation NSOpenPanel(LDView)

- (NSString *)ldvFilename
{
	return [[[self URLs] lastObject] path];
}

@end // NSOpenPanel(LDView)


@implementation NSSavePanel(LDView)

- (NSString *)ldvFilename
{
	return [[self URL] path];
}

@end // NSOpenPanel(LDView)


@implementation NSURL(LDView)

- (const char *)ldvFileSystemRepresentation
{
	if (@available(macOS 10.9, *))
	{
		return self.fileSystemRepresentation;
	}
	else
	{
		return self.path.UTF8String;
	}
}

@end // NSURL(LDView)


@implementation NSBundle(LDView)

- (BOOL)ldvLoadNibNamed:(NSNibName)nibName owner:(id)owner topLevelObjects:(NSArray **)topLevelObjects;
{
	BOOL retValue;
	if (@available(macOS 10.8, *))
	{
		retValue = [self loadNibNamed:nibName owner:owner topLevelObjects:topLevelObjects];
	}
	else
	{
		START_IGNORE_DEPRECATION
		retValue = [[self class] loadNibNamed:[nibName stringByAppendingString:@".nib"] owner:owner];
		END_IGNORE_DEPRECATION
		*topLevelObjects = nil;
	}
	return retValue;
}

@end // NSBundle(LDView)


@implementation NSObject(LDView)

- (BOOL)haveTopLevelObjectsArray
{
	if (@available(macOS 10.8, *))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

- (BOOL)releaseTopLevelObjects:(NSArray *)topLevelObjects
{
	if (@available(macOS 10.8, *))
	{
		[topLevelObjects release];
		return YES;
	}
	else
	{
		return NO;
	}
}

- (void)releaseTopLevelObjects:(NSArray *)topLevelObjects orTopLevelObject:(id)topLevelObject
{
	if (![self releaseTopLevelObjects:topLevelObjects])
	{
		[topLevelObject release];
	}
}

- (BOOL)ldvLoadNibNamed:(NSNibName)nibName topLevelObjects:(NSArray **)topLevelObjects
{
	return [[NSBundle bundleForClass:[self class]] ldvLoadNibNamed:nibName owner:self topLevelObjects:topLevelObjects];
}

@end // NSObject(LDView)
