#import "UpdatesPage.h"
#import "LDViewCategories.h"

#include <LDLib/LDPreferences.h>

@implementation UpdatesPage

- (void)enableProxyUI:(BOOL)enabled
{
	[self enableLabel:proxyLabel value:enabled];
	[proxyField setEnabled:enabled];
	[self enableLabel:portLabel value:enabled];
	[portField setEnabled:enabled];
}

- (void)enableProxy
{
	[self enableProxyUI:YES];
	[proxyField setStringValue:[NSString stringWithUTF8String:ldPreferences->getProxyServer()]];
	[portField setIntValue:ldPreferences->getProxyPort()];
}

- (void)disableProxy
{
	[self enableProxyUI:NO];
	[proxyField setStringValue:@""];
	[portField setStringValue:@""];
}

- (void)proxyTypeChanged
{
	[self setUISection:@"Proxy" enabled:[[proxyMatrix selectedCell] tag] == 2];
}

- (void)enableAutoUpdateUI:(BOOL)enabled
{
	[self enableLabel:missingDaysLabel value:enabled];
	[missingDaysField setEnabled:enabled];
	[self enableLabel:updatedDaysLabel value:enabled];
	[updatedDaysField setEnabled:enabled];
}

- (void)enableAutoUpdate
{
	[self enableAutoUpdateUI:YES];
	[missingDaysField setIntValue:ldPreferences->getMissingPartWait()];
	[updatedDaysField setIntValue:ldPreferences->getUpdatedPartWait()];
}

- (void)disableAutoUpdate
{
	[self enableAutoUpdateUI:NO];
	[missingDaysField setStringValue:@""];
	[updatedDaysField setStringValue:@""];
}

- (void)setup
{
	[super setup];
	if (ldPreferences->getProxyType() == 2)
	{
		[proxyMatrix selectCellWithTag:2];
		[self enableProxy];
	}
	else
	{
		[proxyMatrix selectCellWithTag:0];
		[self disableProxy];
	}
	[self groupCheck:automaticCheck name:@"AutoUpdate" value:ldPreferences->getCheckPartTracker()];
}

- (bool)updateLdPreferences
{
	if ([[proxyMatrix selectedCell] tag] == 2)
	{
		ldPreferences->setProxyType(2);
		ldPreferences->setProxyServer([[proxyField stringValue] UTF8String]);
		ldPreferences->setProxyPort([portField intValue]);
	}
	else
	{
		ldPreferences->setProxyType(0);
	}
	if ([self getCheck:automaticCheck])
	{
		ldPreferences->setCheckPartTracker(true);
		ldPreferences->setMissingPartWait([missingDaysField intValue]);
		ldPreferences->setUpdatedPartWait([updatedDaysField intValue]);
	}
	else
	{
		ldPreferences->setCheckPartTracker(false);
	}
	return [super updateLdPreferences];
}

- (IBAction)resetPage:(id)sender
{
	ldPreferences->loadDefaultUpdatesSettings(false);
	[super resetPage:sender];
}

- (IBAction)proxyType:(id)sender
{
	[self valueChanged:sender];
	[self proxyTypeChanged];
}

- (IBAction)resetTimes:(id)sender
{
	LDrawModelViewer::resetUnofficialDownloadTimes();
}

@end
