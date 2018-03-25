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
#import "OCUserDefaults.h"

#include <LDLib/LDUserDefaultsKeys.h>
#include <LDLib/LDSnapshotTaker.h>
#include <TCFoundation/TCUserDefaults.h>

@implementation SaveSnapshotViewOwner

- (id)init
{
	self = [super init];
	if (self)
	{
		int i;

		firstType = LDSnapshotTaker::ITFirst;
		lastType = LDSnapshotTaker::lastImageType();
		fileTypes = [[NSMutableArray alloc] initWithCapacity:lastType];
		for (i = firstType; i <= lastType; i++)
		{
			LDSnapshotTaker::ImageType type = (LDSnapshotTaker::ImageType)i;

			[fileTypes addObject:[NSString stringWithASCIICString:LDSnapshotTaker::extensionForType(type).c_str()]];
		}
		[NSBundle loadNibNamed:@"SaveSnapshotView" owner:self];
	}
	return self;
}

- (void) dealloc
{
	[savePanel release];
	[accessoryView release];
	[fileTypes release];
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
	[digitsField setIntValue:saveDigits];
	[digitsStepper setIntValue:saveDigits];
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
	[widthField setIntValue:(int)TCUserDefaults::longForKey(SAVE_WIDTH_KEY, 640, false)];
	[heightField setIntValue:(int)TCUserDefaults::longForKey(SAVE_HEIGHT_KEY, 480, false)];
	[zoomToFitCheck setCheck:TCUserDefaults::boolForKey(SAVE_ZOOM_TO_FIT_KEY, true, false)];
}

- (void)disableSize
{
	[self enableSizeUI:NO];
	[widthField setStringValue:@""];
	[heightField setStringValue:@""];
	[zoomToFitCheck setCheck:false];
}

- (void)enableAllStepsUI:(BOOL)enabled
{
	[self enableLabel:suffixLabel value:enabled];
	[suffixField setEnabled:enabled];
	[sameScaleCheck setEnabled:enabled && [self zoomToFit]];
}

- (void)setStepSuffix:(NSString *)value
{
	if (value != stepSuffix)
	{
		[stepSuffix release];
		stepSuffix = [value retain];
	}
}

- (void)enableAllSteps
{
	[self enableAllStepsUI:YES];
	[self setStepSuffix:[OCUserDefaults stringForKey:[NSString stringWithASCIICString:SAVE_STEPS_SUFFIX_KEY] defaultValue:@"" sessionSpecific:NO]];
	[suffixField setStringValue:stepSuffix];
	[sameScaleCheck setCheck:TCUserDefaults::boolForKey(SAVE_STEPS_SAME_SCALE_KEY, true, false)];
}

- (void)disableAllSteps
{
	[self enableAllStepsUI:NO];
	[suffixField setStringValue:@""];
	[sameScaleCheck setCheck:false];
}

- (NSArray *)allowedFileTypes
{
	return fileTypes;
}

- (NSString *)requiredFileType
{
	LDSnapshotTaker::ImageType type = (LDSnapshotTaker::ImageType)[[fileTypePopUp selectedCell] tag];
	BOOL hasOptions = type == LDSnapshotTaker::ITJpg;
	NSString *retValue = [NSString stringWithASCIICString:LDSnapshotTaker::extensionForType(type).c_str()];

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

- (id)isFilenameField:(NSTextField *)textField
{
	NSTextFieldCell *textFieldCell = [textField cell];
	
	if ([textFieldCell bezelStyle] == NSTextFieldSquareBezel && [textField isEditable] && [textField isEnabled])
	{
		NSRect textFieldFrame = [textField frame];
		NSRect labelFrame = [nameFieldLabel frame];
		float fy1 = textFieldFrame.origin.y;
		float fy2 = fy1 + textFieldFrame.size.height;
		float fx1 = textFieldFrame.origin.x;
		float ly1 = labelFrame.origin.y;
		float ly2 = ly1 + labelFrame.size.height;
		float lx1 = labelFrame.origin.x;
		float lx2 = lx1 + labelFrame.size.width;

		if (fy1 <= ly1 && fy2 >= ly2 && fx1 >= lx2)
		{
			return textField;
		}
	}
	return nil;
}

- (id)isNameFieldLabel:(NSTextField *)textField
{
	if ([textField isEnabled] && ![textField isEditable] && [[textField stringValue] isEqualToString:[savePanel nameFieldLabel]])
	{
		return textField;
	}
	else
	{
		return nil;
	}
}

- (NSTextField *)findTextFieldInView:(NSView *)view selector:(SEL)selector
{
	NSArray *subViews = [view subviews];
	int i;
	int count = (int)[subViews count];

	if ([view isHidden])
	{
		return nil;
	}
	for (i = 0; i < count; i++)
	{
		NSView *subView = [subViews objectAtIndex:i];

		if ([subView isKindOfClass:[NSTextField class]] && ![subView isHidden])
		{
			NSTextField *textField = [self performSelector:selector withObject:subView];
			
			if (textField)
			{
				return textField;
			}
		}
	}
	for (i = 0; i < count; i++)
	{
		NSTextField *textField = [self findTextFieldInView:[subViews objectAtIndex:i] selector:selector];
		
		if (textField)
		{
			return textField;
		}
	}
	return nil;
}

- (void)findFilenameField
{
	NSView *saveContentView = [savePanel contentView];

	nameFieldLabel = [self findTextFieldInView:saveContentView selector:@selector(isNameFieldLabel:)];
	if (nameFieldLabel)
	{
		filenameField = [self findTextFieldInView:saveContentView selector:@selector(isFilenameField:)];
	}
}

- (void)setSavePanel:(NSSavePanel *)aSavePanel
{
	if (aSavePanel != nil)
	{
		int fileType = (int)TCUserDefaults::longForKey(SAVE_IMAGE_TYPE_KEY, 1, false);
		int i;

		saveDigits = (int)TCUserDefaults::longForKey(SAVE_DIGITS_KEY, 1, false);
		[fileTypePopUp selectItemWithTag:fileType];
		[self groupCheck:saveSeriesCheck name:@"SaveSeries" value:TCUserDefaults::boolForKey(SAVE_SERIES_KEY, false, false)];
		[self groupCheck:sizeCheck name:@"Size" value:!TCUserDefaults::boolForKey(SAVE_ACTUAL_SIZE_KEY, true, false)];
		[autocropCheck setCheck:TCUserDefaults::boolForKey(AUTO_CROP_KEY, false, false)];
		[matchWindowScaleCheck setCheck:TCUserDefaults::boolForKey(SAVE_MATCH_SCALE_FACTOR_KEY, true, false)];
		[self groupCheck:allStepsCheck name:@"AllSteps" value:TCUserDefaults::boolForKey(SAVE_STEPS_KEY, false, false)];
		for (i = (int)[fileTypePopUp numberOfItems] - 1; i >= 0; i--)
		{
			if ([[fileTypePopUp itemAtIndex:i] tag] > lastType)
			{
				[fileTypePopUp removeItemAtIndex:i];
			}
		}
	}
	if (aSavePanel != savePanel)
	{
		[savePanel release];
		savePanel = [aSavePanel retain];
	}
	if (savePanel)
	{
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(windowDidUpdate:) name:NSWindowDidUpdateNotification object:nil];
		[self findFilenameField];
		if (filenameField == nil)
		{
			[saveSeriesCheck setHidden:YES];
			[saveSeriesBox setHidden:YES];
		}
		[savePanel setAccessoryView:accessoryView];
	}
	[self updateRequiredFileType];
}

- (NSString *)baseFilename
{
	NSString *filename = [[[savePanel filename] lastPathComponent] stringByDeletingPathExtension];

	if ([[suffixField stringValue] length])
	{
		filename = [NSString stringWithASCIICString:LDSnapshotTaker::removeStepSuffix([filename asciiCString], [stepSuffix asciiCString]).c_str()];
	}
	if (saveDigits > 0)
	{
		int i;
		int len = (int)[filename length];

		for (i = 0; i < saveDigits && i < len && isdigit([filename characterAtIndex:len - i - 1]); i++)
		{
			// Don't do anything;
		}
		if (i == saveDigits && i < len && [filename characterAtIndex:len - i - 1] == '-')
		{
			filename = [filename substringToIndex:len - i - 1];
		}
	}
	return filename;
}

- (int)digitsFieldValue
{
	return std::min(10, std::max(1, [digitsField intValue]));
}

- (void)updateFilename
{
	if (filenameField != nil)
	{
		NSString *baseFilename = [self baseFilename];
		NSString *extension = [[savePanel filename] pathExtension];
		NSFileManager *fileManager = [NSFileManager defaultManager];
		int i;

		for (i = 1; i <= 100; i++)
		{
			NSString *filename = baseFilename;

			if ([saveSeriesCheck getCheck])
			{
				NSString *format = [NSString stringWithFormat:@"%%@-%%0%dd", [self digitsFieldValue]];

				filename = [NSString stringWithFormat:format, filename, i];
				saveDigits = [self digitsFieldValue];
			}
			else
			{
				saveDigits = 0;
			}
			if ([allStepsCheck getCheck])
			{
				[self setStepSuffix:[suffixField stringValue]];
				filename = [NSString stringWithASCIICString:LDSnapshotTaker::addStepSuffix([filename asciiCString], [stepSuffix asciiCString], 1, numSteps).c_str()];
			}
			if (![savePanel isExtensionHidden])
			{
				filename = [filename stringByAppendingPathExtension:extension];
			}
			[filenameField setStringValue:filename];
			if (![saveSeriesCheck getCheck] || ![fileManager fileExistsAtPath:[savePanel filename]])
			{
				break;
			}
		}
	}
}

- (void)saveSettings
{
	bool checked;

	TCUserDefaults::setLongForKey([[fileTypePopUp selectedItem] tag], SAVE_IMAGE_TYPE_KEY, false);
	checked = [saveSeriesCheck getCheck];
	TCUserDefaults::setBoolForKey(checked, SAVE_SERIES_KEY, false);
	if (checked)
	{
		TCUserDefaults::setLongForKey([self digitsFieldValue], SAVE_DIGITS_KEY, false);
	}
	TCUserDefaults::setBoolForKey([transparentCheck getCheck], SAVE_ALPHA_KEY, false);
	TCUserDefaults::setBoolForKey([autocropCheck getCheck], AUTO_CROP_KEY, false);
	TCUserDefaults::setBoolForKey([matchWindowScaleCheck getCheck], SAVE_MATCH_SCALE_FACTOR_KEY, false);
	checked = [sizeCheck getCheck];
	TCUserDefaults::setBoolForKey(!checked, SAVE_ACTUAL_SIZE_KEY, false);
	if (checked)
	{
		TCUserDefaults::setLongForKey([widthField intValue], SAVE_WIDTH_KEY, false);
		TCUserDefaults::setLongForKey([heightField intValue], SAVE_HEIGHT_KEY, false);
		TCUserDefaults::setBoolForKey([zoomToFitCheck getCheck], SAVE_ZOOM_TO_FIT_KEY, false);
	}
	checked = [allStepsCheck getCheck];
	TCUserDefaults::setBoolForKey(checked, SAVE_STEPS_KEY, false);
	if (checked)
	{
		[OCUserDefaults setString:[suffixField stringValue] forKey:[NSString stringWithASCIICString:SAVE_STEPS_SUFFIX_KEY] sessionSpecific:NO];
		TCUserDefaults::setBoolForKey([sameScaleCheck getCheck], SAVE_STEPS_SAME_SCALE_KEY, false);
	}
}

- (IBAction)saveSeries:(id)sender
{
	if ([sender getCheck])
	{
		saveDigits = (int)TCUserDefaults::longForKey(SAVE_DIGITS_KEY, 1, false);
	}
	else
	{
		[self updateFilename];
	}
	[self groupCheck:sender name:@"SaveSeries"];
	if ([sender getCheck])
	{
		[self updateFilename];
	}
	else
	{
		saveDigits = 0;
	}
}

- (bool)matchWindowScale
{
	return [matchWindowScaleCheck getCheck];
}

- (IBAction)matchWindowScale:(id)sender
{
	if ([matchWindowScaleCheck getCheck])
	{
		[sizeCheck setCheck:false];
		[self size:sizeCheck];
	}
}

- (IBAction)size:(id)sender
{
	if ([sender getCheck])
	{
		[matchWindowScaleCheck setCheck:false];
		[self matchWindowScale:matchWindowScaleCheck];
	}
	[self groupCheck:sender name:@"Size"];
	[self groupCheck:allStepsCheck name:@"AllSteps"];
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
	LDSnapshotTaker::ImageType type = (LDSnapshotTaker::ImageType)[[fileTypePopUp selectedCell] tag];

	if (type >= firstType && type <= lastType)
	{
		return type;
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

- (IBAction)zoomToFit:(id)sender
{
	[self groupCheck:allStepsCheck name:@"AllSteps"];
}

- (IBAction)allSteps:(id)sender
{
	if (![sender getCheck])
	{
		[self updateFilename];
	}
	[self groupCheck:sender name:@"AllSteps"];
	if ([sender getCheck])
	{
		[self updateFilename];
	}
}

- (void)controlTextDidChange:(NSNotification *)notification
{
	[self updateFilename];
	[self setStepSuffix:[suffixField stringValue]];
}

- (void)windowDidUpdate:(NSNotification *)notification
{
	if ([notification object] == savePanel)
	{
		[self updateFilename];
		[[NSNotificationCenter defaultCenter] removeObserver:self name:NSWindowDidUpdateNotification object:nil];
	}
}

- (void)setNumSteps:(int)value
{
	numSteps = value;
}

- (void)saveDigits:(id)sender
{
	[digitsField takeIntValueFrom:sender];
	[self updateFilename];
}

@end
