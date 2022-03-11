#import "GeneralPage.h"
#import "Preferences.h"
#import "LDViewCategories.h"

#include <LDLib/LDPreferences.h>
#include <TCFoundation/TCUserDefaults.h>

@implementation GeneralPage

- (void)dealloc
{
	[saveDirPopUps release];
	[saveDirFields release];
	[saveDirButtons release];
	[saveDirs release];
	[super dealloc];
}

- (NSString *)defaultSaveDirForOp:(LDPreferences::SaveOp)op modelFilename:(NSString *)filename
{
	return [NSString stringWithUTF8String:ldPreferences->getDefaultSaveDir(op, [filename UTF8String]).c_str()];
}

//- (void)setSnapshotsDir:(NSString *)value
//{
//	if (value != snapshotsDir)
//	{
//		[snapshotsDir release];
//		snapshotsDir = [value retain];
//	}
//}
//
//- (void)setPartsListsDir:(NSString *)value
//{
//	if (value != partsListsDir)
//	{
//		[partsListsDir release];
//		partsListsDir = [value retain];
//	}
//}

- (void)updateDirField:(NSTextField *)field browseButton:(NSButton *)browseButton mode:(int)mode value:(NSString *)value
{
	if (mode == LDPreferences::DDMSpecificDir)
	{
		[field setStringValue:value];
		[field setEnabled:YES];
		[browseButton setEnabled:YES];
	}
	else
	{
		[field setStringValue:@""];
		[field setEnabled:NO];
		[browseButton setEnabled:NO];
	}
}

- (void)updateSaveDirForOp:(int)op
{
	NSString *value = [saveDirs objectAtIndex:op];
	int mode = (int)[[saveDirPopUps objectAtIndex:op] selectedTag];
	NSTextField *field = [saveDirFields objectAtIndex:op];
	NSButton *browseButton = [saveDirButtons objectAtIndex:op];
	
	if (mode == LDPreferences::DDMSpecificDir)
	{
		[field setStringValue:value];
		[field setEnabled:YES];
		[browseButton setEnabled:YES];
	}
	else
	{
		[field setStringValue:@""];
		[field setEnabled:NO];
		[browseButton setEnabled:NO];
	}
}

- (void)setup
{
	int r, g, b;
	int i;
	
	[super setup];
	[self setCheck:promptAtStartupCheck value:[self promptAtStartup]];
	[self setCheck:newModelWindowsCheck value:[self newModelWindows]];
	[self setCheck:antialiasedLinesCheck
		value:ldPreferences->getLineSmoothing()];
	[self setCheck:showFrameRateCheck value:ldPreferences->getShowFps()];
	[self setCheck:showAxesCheck value:ldPreferences->getShowAxes()];
	[self setCheck:showErrorsCheck value:ldPreferences->getShowErrors()];
	[self setCheck:disableSmpCheck value:ldPreferences->getDisableSmp()];
	[self setCheck:processLDConfigCheck
		value:ldPreferences->getProcessLdConfig()];
	[customConfigPathField setStringValue:[NSString stringWithUTF8String:ldPreferences->getCustomConfigPath().c_str()]];
	[self setCheck:randomColorsCheck value:ldPreferences->getRandomColors()];
	ldPreferences->getBackgroundColor(r, g, b);
	[self setColorWell:backgroundColorWell r:r g:g b:b];
	ldPreferences->getDefaultColor(r, g, b);
	[self setColorWell:defaultColorWell r:r g:g b:b];
	[self setCheck:transparentDefaultCheck
		value:ldPreferences->getTransDefaultColor()];
	[fovField setFloatValue:ldPreferences->getFov()];
	[memoryUsagePopUp selectItemWithTag:ldPreferences->getMemoryUsage()];
	saveDirPopUps = [[NSArray alloc] initWithObjects:snapshotsDirPopUp, partsListsDirPopUp, exportsDirPopUp, nil];
	saveDirFields = [[NSArray alloc] initWithObjects:snapshotsDirField, partsListsDirField, exportsDirField, nil];
	saveDirButtons = [[NSArray alloc] initWithObjects:snapshotsBrowseButton, partsListsBrowseButton, exportsBrowseButton, nil];
	saveDirs = [[NSMutableArray alloc] init];
	for (i = LDPreferences::SOFirst; i <= LDPreferences::SOLast && i < [saveDirPopUps count]; i++)
	{
		LDPreferences::SaveOp op = (LDPreferences::SaveOp)i;

		[saveDirs addObject:[NSString stringWithUTF8String:ldPreferences->getSaveDir(op).c_str()]];
		[[saveDirPopUps objectAtIndex:i] selectItemWithTag:ldPreferences->getSaveDirMode(op)];
		[self updateSaveDirForOp:i];
	}
}

- (bool)promptAtStartup
{
	return TCUserDefaults::boolForKey("PromptForModelAtStartup", true, false);
}

- (bool)newModelWindows
{
	return TCUserDefaults::boolForKey("OpenModelsInNewWindows", false, false);
}

- (bool)showErrorsIfNeeded
{
	return ldPreferences->getShowErrors();
}

- (bool)updateLdPreferences
{
	int r, g, b, i;

	TCUserDefaults::setBoolForKey([self getCheck:promptAtStartupCheck], "PromptForModelAtStartup", false);
	TCUserDefaults::setBoolForKey([self getCheck:newModelWindowsCheck], "OpenModelsInNewWindows", false);	
	ldPreferences->setLineSmoothing([self getCheck:antialiasedLinesCheck]);
	ldPreferences->setProcessLdConfig([self getCheck:processLDConfigCheck]);
	ldPreferences->setCustomConfigPath([[customConfigPathField stringValue] UTF8String]);
	ldPreferences->setRandomColors([self getCheck:randomColorsCheck]);
	ldPreferences->setShowErrors([self getCheck:showErrorsCheck]);
	ldPreferences->setDisableSmp([disableSmpCheck getCheck]);
	ldPreferences->setShowFps([self getCheck:showFrameRateCheck]);
	ldPreferences->setShowAxes([self getCheck:showAxesCheck]);
	[self getColorWell:backgroundColorWell r:&r g:&g b:&b];
	ldPreferences->setBackgroundColor(r,g,b);
	[self getColorWell:defaultColorWell r:&r g:&g b:&b];
	ldPreferences->setDefaultColor(r,g,b);
	ldPreferences->setTransDefaultColor(
		[self getCheck:transparentDefaultCheck]);
	ldPreferences->setFov([fovField floatValue]);
	ldPreferences->setMemoryUsage((int)[[memoryUsagePopUp selectedItem] tag]);
	for (i = LDPreferences::SOFirst; i <= LDPreferences::SOLast; i++)
	{
		if (i < [saveDirs count])
		{
			LDPreferences::SaveOp op = (LDPreferences::SaveOp)i;

			ldPreferences->setSaveDirMode(op, (LDPreferences::DefaultDirMode)[[saveDirPopUps objectAtIndex:i] selectedTag]);
			ldPreferences->setSaveDir(op, [[saveDirs objectAtIndex:i] UTF8String]);
		}
	}
	return [super updateLdPreferences];
}

- (IBAction)resetPage:(id)sender
{
	ldPreferences->loadDefaultGeneralSettings(false);
	[super resetPage:sender];
}

- (IBAction)saveDirMode:(id)sender
{
	int tag = (int)[sender tag];

	[self updateSaveDirForOp:tag];
}

- (void)textDidChange:(NSNotification *)aNotification
{
	id sender = [aNotification object];
	int tag = (int)[sender tag];

	if (tag < [saveDirFields count] && [saveDirFields objectAtIndex:tag] == sender)
	{
		[saveDirs replaceObjectAtIndex:tag withObject:[sender stringValue]];
	}
	[super textDidChange:aNotification];
}

- (void)openPanelDidEnd:(NSOpenPanel *)openPanel returnCode:(NSModalResponse)returnCode contextInfo:(void  *)contextInfo
{
	if (returnCode == NSModalResponseOK)
	{
		int tag = (int)[(id)contextInfo tag];
		NSTextField *textField = nil;

		if (tag < [saveDirFields count])
		{
			textField = [saveDirFields objectAtIndex:tag];
		}
		else if (tag == 3) // Custom Config Path
		{
			textField = customConfigPathField;
		}
		[textField setStringValue:[openPanel ldvFilename]];
		[[NSNotificationCenter defaultCenter] postNotificationName:NSControlTextDidChangeNotification object:textField];
	}
}

- (IBAction)configPathBrowse:(id)sender
{
	NSString *dir = [[customConfigPathField stringValue] stringByDeletingLastPathComponent];
	NSArray *allowedFileTypes = [NSArray arrayWithObjects: @"ldr", @"dat", @"mpd", nil];
	if ([dir length] == 0)
	{
		dir = nil;
	}
	[self browse:customConfigPathField forFolder:NO initialDir:dir allowedFileTypes:allowedFileTypes];
}

- (IBAction)saveDirBrowse:(id)sender
{
	int tag = (int)[sender tag];
	
	if (tag < [saveDirs count])
	{
		NSString *dir = [saveDirs objectAtIndex:tag];
		
		if ([dir length] > 0)
		{
			[self browseForFolder:sender initialDir:dir];
			return;
		}
	}
	[self browseForFolder:sender];
}

- (int)saveDirModeForOp:(LDPreferences::SaveOp)op
{
	return ldPreferences->getSaveDirMode(op);
}

- (NSString *)saveDirForOp:(LDPreferences::SaveOp)op
{
	return [NSString stringWithUTF8String:ldPreferences->getSaveDir(op).c_str()];
}

@end
