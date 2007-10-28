//
//  SaveSnapshotViewOwner.mm
//  LDView
//
//  Created by Travis Cobbs on 10/27/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "SaveSnapshotViewOwner.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>


@implementation SaveSnapshotViewOwner

- (id)init
{
	self = [super init];
	if (self)
	{
		[NSBundle loadNibNamed:@"SaveSnapshotView" owner:self];
	}
	return self;
}

- (void) dealloc
{
	[savePanel release];
	[accessoryView release];
	[super dealloc];
}

- (void)enableSaveSeriesUI:(BOOL)enabled
{
	[self enableLabel:digitsLabel value:enabled];
	[digitsField setEnabled:enabled];
	[digitsStepper setEnabled:enabled];
}

- (void)enableSaveSeries
{
	[self enableSaveSeriesUI:YES];
	[digitsField setIntValue:TCUserDefaults::longForKey(SAVE_DIGITS_KEY, 1, false)];
}

- (void)disableSaveSeries
{
	[self enableSaveSeriesUI:NO];
	[digitsField setStringValue:@""];
}

- (void)enableSizeUI:(BOOL)enabled
{
	[sizeForm setEnabled:enabled];
	[zoomToFitCheck setEnabled:enabled];
}

- (void)enableSize
{
	[self enableSizeUI:YES];
	[[sizeForm cellAtIndex:0] setIntValue:TCUserDefaults::longForKey(SAVE_WIDTH_KEY, 640, false)];
	int height = TCUserDefaults::longForKey(SAVE_HEIGHT_KEY, 480, false);
	[[sizeForm cellAtIndex:1] setIntValue:height];
	[self setCheck:zoomToFitCheck value:TCUserDefaults::longForKey(SAVE_ZOOM_TO_FIT_KEY, true, false)];
}

- (void)disableSize
{
	[self enableSizeUI:NO];
	[[sizeForm cellAtIndex:0] setStringValue:@""];
	[[sizeForm cellAtIndex:1] setStringValue:@""];
	[self setCheck:zoomToFitCheck value:false];
}

- (void)awakeFromNib
{
	[super awakeFromNib];
	[accessoryView retain];
}

- (NSArray *)allowedFileTypes
{
	if ([[fileTypePopUp selectedCell] tag] == 2)
	{
		return [NSArray arrayWithObjects:@"bmp", @"png", nil];
	}
	else
	{
		return [NSArray arrayWithObjects:@"png", @"bmp", nil];
	}
}

- (NSString *)requiredFileType
{
	switch ([[fileTypePopUp selectedCell] tag])
	{
		case 1:
			return @"png";
		case 2:
			return @"bmp";
	}
	return @"png";
}

- (void)setSavePanel:(NSSavePanel *)aSavePanel
{
	if (aSavePanel != nil)
	{
		int fileType = TCUserDefaults::longForKey(SAVE_IMAGE_TYPE_KEY, 1, false);
		
		[fileTypePopUp selectItemWithTag:fileType];
		[self groupCheck:saveSeriesCheck name:@"SaveSeries" value:TCUserDefaults::boolForKey(SAVE_SERIES_KEY, false, false)];
		[self groupCheck:sizeCheck name:@"Size" value:!TCUserDefaults::boolForKey(SAVE_ACTUAL_SIZE_KEY, true, false)];
		[self setCheck:transparentCheck value:TCUserDefaults::boolForKey(SAVE_ALPHA_KEY, true, false)];
		[self setCheck:autocropCheck value:TCUserDefaults::boolForKey(AUTO_CROP_KEY, false, false)];
		[aSavePanel setRequiredFileType:[self requiredFileType]];
	}
	if (aSavePanel != savePanel)
	{
		[savePanel release];
		savePanel = [aSavePanel retain];
	}
	if (savePanel)
	{
		[savePanel setAccessoryView:accessoryView];
	}
}

- (void)saveSettings
{
	bool checked;

	TCUserDefaults::setLongForKey([[fileTypePopUp selectedItem] tag], SAVE_IMAGE_TYPE_KEY, false);
	checked = [self getCheck:saveSeriesCheck];
	TCUserDefaults::setBoolForKey(checked, SAVE_SERIES_KEY, false);
	if (checked)
	{
		TCUserDefaults::setLongForKey([digitsField intValue], SAVE_DIGITS_KEY, false);
	}
	TCUserDefaults::setBoolForKey([self getCheck:transparentCheck], SAVE_ALPHA_KEY, false);
	TCUserDefaults::setBoolForKey([self getCheck:autocropCheck], AUTO_CROP_KEY, false);
	checked = [self getCheck:sizeCheck];
	TCUserDefaults::setBoolForKey(!checked, SAVE_ACTUAL_SIZE_KEY, false);
	if (checked)
	{
		TCUserDefaults::setLongForKey([[sizeForm cellAtIndex:0] intValue], SAVE_WIDTH_KEY, false);
		TCUserDefaults::setLongForKey([[sizeForm cellAtIndex:1] intValue], SAVE_HEIGHT_KEY, false);
		TCUserDefaults::setBoolForKey([self getCheck:zoomToFitCheck], SAVE_ZOOM_TO_FIT_KEY, false);
	}
}

- (IBAction)saveSeries:(id)sender
{
	[self groupCheck:sender name:@"SaveSeries"];
}

- (IBAction)size:(id)sender
{
	[self groupCheck:sender name:@"Size"];
}

- (IBAction)fileType:(id)sender
{
	[savePanel setRequiredFileType:[self requiredFileType]];
	[savePanel validateVisibleColumns];
}

- (bool)transparentBackground
{
	return [self getCheck:transparentCheck];
}

- (bool)autocrop
{
	return [self getCheck:autocropCheck];
}

- (LDSnapshotTaker::ImageType)imageType
{
	if ([[fileTypePopUp selectedCell] tag] == 2)
	{
		return LDSnapshotTaker::ITBmp;
	}
	else
	{
		return LDSnapshotTaker::ITPng;
	}
}

- (int)width:(int)refWidth
{
	if ([self getCheck:sizeCheck])
	{
		return [[sizeForm cellAtIndex:0] intValue];
	}
	else
	{
		return refWidth;
	}
}

- (int)height:(int)refHeight
{
	if ([self getCheck:sizeCheck])
	{
		return [[sizeForm cellAtIndex:1] intValue];
	}
	else
	{
		return refHeight;
	}
}

- (bool)zoomToFit
{
	if ([self getCheck:sizeCheck])
	{
		return [self getCheck:zoomToFitCheck];
	}
	else
	{
		return false;
	}
}

@end
