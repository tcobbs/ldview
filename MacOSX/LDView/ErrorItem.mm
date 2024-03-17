//
//  ErrorItem.mm
//  LDView
//
//  Created by Travis Cobbs on 8/19/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "ErrorItem.h"
#include <LDLoader/LDLError.h>

@implementation ErrorItem

static NSMutableDictionary *templateDict = [[NSMutableDictionary alloc] init];
static NSDictionary *iconDict = nil;

+ (void)initialize
{
	iconDict = [[NSDictionary alloc] initWithObjectsAndKeys:
		@"error_parse", [NSNumber numberWithInt:LDLEParse],
		@"error_fnf", [NSNumber numberWithInt:LDLEFileNotFound],
		@"error_matrix", [NSNumber numberWithInt:LDLEMatrix],
		@"error_determinant", [NSNumber numberWithInt:LDLEPartDeterminant],
		@"error_non_flat_quad", [NSNumber numberWithInt:LDLENonFlatQuad],
		@"error_concave_quad", [NSNumber numberWithInt:LDLEConcaveQuad],
		@"error_vertex_order", [NSNumber numberWithInt:LDLEVertexOrder],
		@"error_matching_points", [NSNumber numberWithInt:LDLEMatchingPoints],
		@"error_colinear", [NSNumber numberWithInt:LDLEColinear],
		@"error_parse", [NSNumber numberWithInt:LDLEBFCWarning],
		@"error_parse", [NSNumber numberWithInt:LDLEBFCError],
		@"error_parse", [NSNumber numberWithInt:LDLEMPDError],
		@"error_loop", [NSNumber numberWithInt:LDLEModelLoop],
		@"error_parse", [NSNumber numberWithInt:LDLEMetaCommand],
		@"error_parse", [NSNumber numberWithInt:LDLETooManyRequests],
		nil];	
}

- (id)initWithAttributedString:(NSAttributedString *)value error:(LDLError *)theError
{
	if ((self = [super init]) != nil)
	{
		objectValue = [value copy]; // Guarantee non-mutable
		string = [[objectValue string] retain];
		error = TCObject::retain(theError);
	}
	return self;
}

- (id)initWithString:(NSString *)value error:(LDLError *)theError includeIcon:(BOOL)includeIcon
{
	if ((self = [super init]) != nil)
	{
		string = [value retain];
		theError->retain();
		error = theError;
		if (includeIcon)
		{
			NSNumber *errorTypeKey = [NSNumber numberWithInt:theError->getType()];
			NSMutableAttributedString *templateString = [templateDict objectForKey:errorTypeKey];

			if (!templateString)
			{
				NSString *iconName = [iconDict objectForKey:errorTypeKey];
				
				if (!iconName)
				{
					iconName = @"error_info";
				}
				NSString *imagePath = [[NSBundle mainBundle] pathForImageResource:iconName];
				NSData *imageData = [[NSData alloc] initWithContentsOfFile:imagePath];
				NSFileWrapper *fileWrapper = [[NSFileWrapper alloc] initRegularFileWithContents:imageData];
				[imageData release];
				[fileWrapper setPreferredFilename:[iconName stringByAppendingString:@".png"]];
				NSTextAttachment *attachment = [[NSTextAttachment alloc] initWithFileWrapper:fileWrapper];
				[fileWrapper release];
				templateString = [[[NSMutableAttributedString alloc] initWithString:@" "] autorelease];
				[templateString insertAttributedString:[NSAttributedString attributedStringWithAttachment:attachment] atIndex:0];
				[templateDict setObject:templateString forKey:errorTypeKey];
				[attachment release];
			}
			NSMutableAttributedString *attribString = [templateString mutableCopy];
			[attribString appendAttributedString:[[[NSAttributedString alloc] initWithString:string] autorelease]];
			[attribString addAttribute:NSFontAttributeName value:[NSFont systemFontOfSize:[NSFont systemFontSize]] range:NSMakeRange(0, [attribString length])];
			objectValue = [attribString copy]; // Non-mutable
			[attribString release];
		}
		else
		{
			objectValue = [string retain];
		}
	}
	return self;
}

- (void)dealloc
{
	[children release];
	[string release];
	[objectValue release];
	TCObject::release(error);
	[super dealloc];
}

- (ErrorItem *)addChild:(ErrorItem *)child
{
	if (!children)
	{
		children = [[NSMutableArray alloc] init];
	}
	[children addObject:child];
	return child;
}

- (NSInteger)numberOfChildren
{
	if (children)
	{
		return [children count];
	}
	else
	{
		return -1;
	}
}

- (ErrorItem *)childAtIndex:(NSInteger)index
{
	return [children objectAtIndex:index];
}

- (id)objectValue
{
	return objectValue;
}

- (LDLError *)error
{
	return error;
}

- (NSString *)stringValue
{
	return string;
}

@end
