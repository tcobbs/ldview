#import "GeneralPage.h"
#import "Preferences.h"
#include <LDLib/LDPreferences.h>

@implementation GeneralPage

- (void)setup
{
	[super setup];
	ldPreferences = [preferences ldPreferences];
	[self setCheck:antialiasedLinesCheck
		value:ldPreferences->getLineSmoothing()];
	[self setCheck:showFrameRateCheck value:ldPreferences->getShowFps()];
	[self setCheck:showErrorsCheck value:ldPreferences->getShowErrors()];
	[self setCheck:processLDConfigCheck
		value:ldPreferences->getProcessLdConfig()];
	// BG color
	// Default color
	[self setCheck:transparentDefaultCheck
		value:ldPreferences->getTransDefaultColor()];
	// FOV
	// Memory Usage
}

- (void)updateLdPreferences
{
	[super updateLdPreferences];
	ldPreferences->setLineSmoothing([self getCheck:antialiasedLinesCheck]);
	ldPreferences->setProcessLdConfig([self getCheck:processLDConfigCheck]);
	ldPreferences->setShowErrors([self getCheck:showErrorsCheck]);
	ldPreferences->setShowFps([self getCheck:showFrameRateCheck]);
	ldPreferences->setTransDefaultColor(
		[self getCheck:transparentDefaultCheck]);
}

@end
