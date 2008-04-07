#import "GeneralPage.h"
#import "Preferences.h"
#import "LDViewCategories.h"

#include <LDLib/LDPreferences.h>
#include <TCFoundation/TCUserDefaults.h>

@implementation GeneralPage

- (void)setSnapshotsDir:(NSString *)value
{
	if (value != snapshotsDir)
	{
		[snapshotsDir release];
		snapshotsDir = [value retain];
	}
}

- (void)setPartsListsDir:(NSString *)value
{
	if (value != partsListsDir)
	{
		[partsListsDir release];
		partsListsDir = [value retain];
	}
}

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

- (void)setup
{
	int r, g, b;
	
	[super setup];
	[self setCheck:promptAtStartupCheck value:[self promptAtStartup]];
	[self setCheck:newModelWindowsCheck value:[self newModelWindows]];
	[self setCheck:antialiasedLinesCheck
		value:ldPreferences->getLineSmoothing()];
	[self setCheck:showFrameRateCheck value:ldPreferences->getShowFps()];
	[self setCheck:showErrorsCheck value:ldPreferences->getShowErrors()];
	[self setCheck:processLDConfigCheck
		value:ldPreferences->getProcessLdConfig()];
	ldPreferences->getBackgroundColor(r, g, b);
	[self setColorWell:backgroundColorWell r:r g:g b:b];
	ldPreferences->getDefaultColor(r, g, b);
	[self setColorWell:defaultColorWell r:r g:g b:b];
	[self setCheck:transparentDefaultCheck
		value:ldPreferences->getTransDefaultColor()];
	[fovField setFloatValue:ldPreferences->getFov()];
	[memoryUsagePopUp selectItemWithTag:ldPreferences->getMemoryUsage()];
	[snapshotsDirPopUp selectItemWithTag:ldPreferences->getSnapshotsDirMode()];
	[self setSnapshotsDir:[self snapshotsDir]];
	[self updateDirField:snapshotsDirField browseButton:snapshotsBrowseButton mode:ldPreferences->getSnapshotsDirMode() value:snapshotsDir];
	[self setPartsListsDir:[self partsListsDir]];
	[partsListsDirPopUp selectItemWithTag:ldPreferences->getPartsListsDirMode()];
	[self updateDirField:partsListsDirField browseButton:partsListsBrowseButton mode:ldPreferences->getPartsListsDirMode() value:partsListsDir];
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
	int r, g, b;

	TCUserDefaults::setBoolForKey([self getCheck:promptAtStartupCheck], "PromptForModelAtStartup", false);
	TCUserDefaults::setBoolForKey([self getCheck:newModelWindowsCheck], "OpenModelsInNewWindows", false);	
	ldPreferences->setLineSmoothing([self getCheck:antialiasedLinesCheck]);
	ldPreferences->setProcessLdConfig([self getCheck:processLDConfigCheck]);
	ldPreferences->setShowErrors([self getCheck:showErrorsCheck]);
	ldPreferences->setShowFps([self getCheck:showFrameRateCheck]);
	[self getColorWell:backgroundColorWell r:&r g:&g b:&b];
	ldPreferences->setBackgroundColor(r,g,b);
	[self getColorWell:defaultColorWell r:&r g:&g b:&b];
	ldPreferences->setDefaultColor(r,g,b);
	ldPreferences->setTransDefaultColor(
		[self getCheck:transparentDefaultCheck]);
	ldPreferences->setFov([fovField floatValue]);
	ldPreferences->setMemoryUsage([[memoryUsagePopUp selectedItem] tag]);
	ldPreferences->setSnapshotsDirMode((LDPreferences::DefaultDirMode)[snapshotsDirPopUp selectedTag]);
	ldPreferences->setSnapshotsDir([snapshotsDir asciiCString]);
	ldPreferences->setPartsListsDirMode((LDPreferences::DefaultDirMode)[partsListsDirPopUp selectedTag]);
	ldPreferences->setPartsListsDir([partsListsDir asciiCString]);
	return [super updateLdPreferences];
}

- (IBAction)resetPage:(id)sender
{
	ldPreferences->loadDefaultGeneralSettings(false);
	[super resetPage:sender];
}

- (IBAction)snapshot:(id)sender
{
	[self updateDirField:snapshotsDirField browseButton:snapshotsBrowseButton mode:[sender selectedTag] value:snapshotsDir];
}

- (IBAction)partsList:(id)sender
{
	[self updateDirField:partsListsDirField browseButton:partsListsBrowseButton mode:[sender selectedTag] value:partsListsDir];
}

- (void)textDidChange:(NSNotification *)aNotification
{
	if ([aNotification object] == snapshotsDirField)
	{
		[self setSnapshotsDir:[snapshotsDirField stringValue]];
	}
	else if ([aNotification object] == partsListsDirField)
	{
		[self setPartsListsDir:[partsListsDirField stringValue]];
	}
	[super textDidChange:aNotification];
}

- (void)openPanelDidEnd:(NSOpenPanel *)openPanel returnCode:(int)returnCode contextInfo:(void  *)contextInfo
{
	if (returnCode = NSOKButton)
	{
		NSTextField *textField = (NSTextField *)contextInfo;
		[textField setStringValue:[openPanel filename]];
		[[NSNotificationCenter defaultCenter] postNotificationName:NSControlTextDidChangeNotification object:textField];
	}
}

- (IBAction)snapshotBrowse:(id)sender
{
	[self browseForFolder:snapshotsDirField];
}

- (IBAction)partsListBrowse:(id)sender
{
	[self browseForFolder:partsListsDirField];
}

- (int)snapshotsDirMode
{
	return ldPreferences->getSnapshotsDirMode();
}

- (NSString *)snapshotsDir
{
	return [NSString stringWithASCIICString:ldPreferences->getSnapshotsDir().c_str()];
}

- (int)partsListsDirMode
{
	return ldPreferences->getPartsListsDirMode();
}

- (NSString *)partsListsDir
{
	return [NSString stringWithASCIICString:ldPreferences->getPartsListsDir().c_str()];
}

@end
