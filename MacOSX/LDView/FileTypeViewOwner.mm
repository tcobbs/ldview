//
//  FileTypeViewOwner.mm
//  LDView
//
//  Created by Travis Cobbs on 10/27/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "FileTypeViewOwner.h"
#import "LDViewCategories.h"
#import "OCUserDefaults.h"

#include <LDLib/LDUserDefaultsKeys.h>
#include <LDLib/LDSnapshotTaker.h>
#include <TCFoundation/TCUserDefaults.h>

@implementation FileTypeViewOwner

- (id)init
{
	self = [super init];
	if (self)
	{
		fileTypes = [[NSMutableArray alloc] init];
		extensions = [[NSMutableArray alloc] init];
	}
	return self;
}

- (void) dealloc
{
	[savePanel release];
	if (![self haveTopLevelObjectsArray])
	{
		[accessoryView release];
	}
	[fileTypes release];
	[udTypeKey release];
	[super dealloc];
}

- (NSArray *)allowedFileTypes
{
	return fileTypes;
}

- (NSString *)requiredFileType
{
	return [extensions objectAtIndex:[fileTypePopUp indexOfSelectedItem]];
}

- (void)updateRequiredFileType
{
	NSString *requiredFileType = [self requiredFileType];

	savePanel.allowedFileTypes = [NSArray arrayWithObject:requiredFileType];
	[savePanel validateVisibleColumns];
	[fileTypeOptionsButton setEnabled:[self haveOptions]];
}

- (void)setFileTypeExtension:(NSString *)extension
{
	NSUInteger index = NSNotFound;
	
	if (extension != nil)
	{
		index = [extensions indexOfObject:extension];
	}
	if (index == NSNotFound)
	{
		index = 0;
	}
	[fileTypePopUp selectItemAtIndex:index];
}

- (void)setSavePanel:(NSSavePanel *)aSavePanel
{
	if (aSavePanel != nil)
	{
		// IB won't let you remove all items, so there's one dummy item.
		[fileTypePopUp removeAllItems];
		[fileTypePopUp addItemsWithTitles:fileTypes];
		if (udTypeKey != nil)
		{
			NSString *extension = [OCUserDefaults stringForKey:udTypeKey defaultValue:nil sessionSpecific:NO];
			
			[self setFileTypeExtension:extension];
		}
	}
	if (aSavePanel != savePanel)
	{
		[savePanel release];
		savePanel = [aSavePanel retain];
	}
	if (savePanel != nil)
	{
		[savePanel setAccessoryView:accessoryView];
	}
	[self updateRequiredFileType];
}

- (void)saveSettings
{
	if (udTypeKey)
	{
		[OCUserDefaults setString:[self requiredFileType] forKey:udTypeKey sessionSpecific:NO];
	}
}

- (IBAction)fileType:(id)sender
{
	[self updateRequiredFileType];
}

- (IBAction)fileTypeOptions:(id)sender
{
}

- (BOOL)haveOptions
{
	return NO;
}

@end
