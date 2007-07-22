#import "GeneralPage.h"
#import "Preferences.h"
#include <LDLib/LDPreferences.h>

@implementation GeneralPage

- (void)setup
{
	int r, g, b;
	
	[super setup];
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
}

- (void)updateLdPreferences
{
	int r, g, b;
	
	[super updateLdPreferences];
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
}

@end
