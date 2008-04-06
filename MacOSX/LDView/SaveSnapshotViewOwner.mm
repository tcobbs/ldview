//
//  SaveSnapshotViewOwner.mm
//  LDView
//
//  Created by Travis Cobbs on 10/27/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "SaveSnapshotViewOwner.h"
#import "JpegOptions.h"
#import "LDViewCategories.h"
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
	[self enableLabel:widthLabel value:enabled];
	[widthField setEnabled:enabled];
	[self enableLabel:heightLabel value:enabled];
	[heightField setEnabled:enabled];
	[zoomToFitCheck setEnabled:enabled];
}

- (void)enableSize
{
	[self enableSizeUI:YES];
	[widthField setIntValue:TCUserDefaults::longForKey(SAVE_WIDTH_KEY, 640, false)];
	[heightField setIntValue:TCUserDefaults::longForKey(SAVE_HEIGHT_KEY, 480, false)];
	[zoomToFitCheck setCheck:TCUserDefaults::longForKey(SAVE_ZOOM_TO_FIT_KEY, true, false)];
}

- (void)disableSize
{
	[self enableSizeUI:NO];
	[widthField setStringValue:@""];
	[heightField setStringValue:@""];
	[zoomToFitCheck setCheck:false];
}

- (NSArray *)allowedFileTypes
{
	if ([[fileTypePopUp selectedCell] tag] == 2)
	{
		return [NSArray arrayWithObjects:@"bmp", @"png", @"jpg", nil];
	}
	else
	{
		return [NSArray arrayWithObjects:@"png", @"bmp", @"jpg", nil];
	}
}

- (NSString *)requiredFileType
{
	BOOL hasOptions = FALSE;
	NSString *retValue = @"png";

	switch ([[fileTypePopUp selectedCell] tag])
	{
		case 1:
			retValue = @"png";
			break;
		case 2:
			retValue = @"bmp";
			break;
		case 3:
			hasOptions = TRUE;
			retValue = @"jpg";
			break;
	}
	[fileTypeOptionsButton setEnabled:hasOptions];
	return retValue;
}

- (bool)alphaSupported:(NSString *)fileType
{
	return [fileType isEqualToString:@"png"];
}

- (void)updateRequiredFileType
{
	NSString *requiredFileType = [self requiredFileType];

	[savePanel setRequiredFileType:requiredFileType];
	[savePanel validateVisibleColumns];
	if ([self alphaSupported:requiredFileType])
	{
		[transparentCheck setEnabled:YES];
		[transparentCheck setCheck:TCUserDefaults::boolForKey(SAVE_ALPHA_KEY, true, false)];
	}
	else
	{
		[transparentCheck setEnabled:NO];
		[transparentCheck setCheck:false];
	}
}

- (void)setSavePanel:(NSSavePanel *)aSavePanel
{
	if (aSavePanel != nil)
	{
		int fileType = TCUserDefaults::longForKey(SAVE_IMAGE_TYPE_KEY, 1, false);
		
		[fileTypePopUp selectItemWithTag:fileType];
		[self groupCheck:saveSeriesCheck name:@"SaveSeries" value:TCUserDefaults::boolForKey(SAVE_SERIES_KEY, false, false)];
		[self groupCheck:sizeCheck name:@"Size" value:!TCUserDefaults::boolForKey(SAVE_ACTUAL_SIZE_KEY, true, false)];
		[autocropCheck setCheck:TCUserDefaults::boolForKey(AUTO_CROP_KEY, false, false)];
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
	[self updateRequiredFileType];
}

- (void)saveSettings
{
	bool checked;

	TCUserDefaults::setLongForKey([[fileTypePopUp selectedItem] tag], SAVE_IMAGE_TYPE_KEY, false);
	checked = [saveSeriesCheck getCheck];
	TCUserDefaults::setBoolForKey(checked, SAVE_SERIES_KEY, false);
	if (checked)
	{
		TCUserDefaults::setLongForKey([digitsField intValue], SAVE_DIGITS_KEY, false);
	}
	TCUserDefaults::setBoolForKey([transparentCheck getCheck], SAVE_ALPHA_KEY, false);
	TCUserDefaults::setBoolForKey([autocropCheck getCheck], AUTO_CROP_KEY, false);
	checked = [sizeCheck getCheck];
	TCUserDefaults::setBoolForKey(!checked, SAVE_ACTUAL_SIZE_KEY, false);
	if (checked)
	{
		TCUserDefaults::setLongForKey([widthField intValue], SAVE_WIDTH_KEY, false);
		TCUserDefaults::setLongForKey([heightField intValue], SAVE_HEIGHT_KEY, false);
		TCUserDefaults::setBoolForKey([zoomToFitCheck getCheck], SAVE_ZOOM_TO_FIT_KEY, false);
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
	[self updateRequiredFileType];
}

- (void)jpegOptions
{
	JpegOptions *options = [[JpegOptions alloc] init];

	[options runModal];
	[options release];
}

- (IBAction)fileTypeOptions:(id)sender
{
	switch ([[fileTypePopUp selectedItem] tag])
	{
	case 3:
		[self jpegOptions];
		break;
	}
}

- (bool)transparentBackground
{
	return [transparentCheck getCheck];
}

- (bool)autocrop
{
	return [autocropCheck getCheck];
}

- (LDSnapshotTaker::ImageType)imageType
{
	switch ([[fileTypePopUp selectedCell] tag])
	{
		case 2:
			return LDSnapshotTaker::ITBmp;
		case 3:
			return LDSnapshotTaker::ITJpg;
	}
	return LDSnapshotTaker::ITPng;
}

- (int)width:(int)refWidth
{
	if ([sizeCheck getCheck])
	{
		return [widthField intValue];
	}
	else
	{
		return refWidth;
	}
}

- (int)height:(int)refHeight
{
	if ([sizeCheck getCheck])
	{
		return [heightField intValue];
	}
	else
	{
		return refHeight;
	}
}

- (bool)zoomToFit
{
	if ([sizeCheck getCheck])
	{
		return [zoomToFitCheck getCheck];
	}
	else
	{
		return false;
	}
}

@end
