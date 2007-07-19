#import "EffectsPage.h"
#include <LDLib/LDPreferences.h>

@implementation EffectsPage

- (void)enableLightingUI:(BOOL)enabled
{
	[self setCheck:lightingCheck value:enabled];
	[highQualityCheck setEnabled:enabled];
	[subduedCheck setEnabled:enabled];
	[specularCheck setEnabled:enabled];
	[alternateLightingCheck setEnabled:enabled];
	[lightDirMatrix setEnabled:enabled];
	[useLightDatCheck setEnabled:enabled];
	[replaceStandardLightCheck setEnabled:enabled];
	[hideLightDatGeom setEnabled:enabled];
}

- (void)enableLighting
{
	[self enableLightingUI:YES];
	[self setCheck:highQualityCheck value:ldPreferences->getQualityLighting()];
	[self setCheck:subduedCheck value:ldPreferences->getSubduedLighting()];
	[self setCheck:specularCheck value:ldPreferences->getUseSpecular()];
	[self setCheck:alternateLightingCheck value:ldPreferences->getOneLight()];	
	// Light dir
	[self setCheck:useLightDatCheck value:ldPreferences->getDrawLightDats()];
	[self groupCheck:useLightDatCheck name:@"LightDats" init:YES];
	[self setCheck:hideLightDatGeom value:ldPreferences->getNoLightGeom()];
}

- (void) disableLighting
{
	[self enableLightingUI:NO];
	[self setCheck:highQualityCheck value:NO];
	[self setCheck:subduedCheck value:NO];
	[self setCheck:specularCheck value:NO];
	[self setCheck:alternateLightingCheck value:NO];	
	[self setCheck:useLightDatCheck value:NO];
	[self setCheck:replaceStandardLightCheck value:NO];
	[self setCheck:hideLightDatGeom value:NO];
}

- (void)enableLightDatsUI:(BOOL)enabled
{
	[self setCheck:useLightDatCheck value:enabled];
	[replaceStandardLightCheck setEnabled:enabled];
}

- (void)enableLightDats
{
	[self enableLightDatsUI:YES];
	[self setCheck:replaceStandardLightCheck value:ldPreferences->getOptionalStandardLight()];
}

- (void) disableLightDats
{
	[self enableLightDatsUI:NO];
	[self setCheck:replaceStandardLightCheck value:NO];
}

- (void)enableStereoUI:(BOOL)enabled
{
	[self setCheck:stereoCheck value:enabled];
	[stereoMatrix setEnabled:enabled];
	[stereoAmountSlider setEnabled:enabled];
}

- (void)enableStereo
{
	[self enableStereoUI:YES];
}

- (void) disableStereo
{
	[self enableStereoUI:NO];
}

- (void)enableWireframeCutawayUI:(BOOL)enabled
{
	[self setCheck:wireframeCutawayCheck value:enabled];
	[wireframeCutawayMatrix setEnabled:enabled];
	[opacitySlider setEnabled:enabled];
	[wcThicknessSlider setEnabled:enabled];
}

- (void)enableWireframeCutaway
{
	[self enableWireframeCutawayUI:YES];
}

- (void) disableWireframeCutaway
{
	[self enableWireframeCutawayUI:NO];
}

- (void)setup
{
	[super setup];
	[self setCheck:lightingCheck value:ldPreferences->getUseLighting()];
	[self setCheck:stereoCheck value:ldPreferences->getStereoMode() != LDVStereoNone];
	[self setCheck:wireframeCutawayCheck
			 value:ldPreferences->getCutawayMode() != LDVCutawayNormal];
	[self groupCheck:lightingCheck name:@"Lighting" init:YES];
	[self groupCheck:stereoCheck name:@"Stereo" init:YES];
	[self groupCheck:wireframeCutawayCheck name:@"WireframeCutaway" init:YES];
}

- (void)updateLdPreferences
{
	[super updateLdPreferences];
}

- (IBAction)lighting:(id)sender
{
	[self groupCheck:lightingCheck name:@"Lighting"];
}

- (IBAction)useLightDats:(id)sender
{
	[self groupCheck:useLightDatCheck name:@"LightDats"];
}

- (IBAction)stereo:(id)sender
{
	[self groupCheck:stereoCheck name:@"Stereo"];
}

- (IBAction)wireframeCutaway:(id)sender
{
	[self groupCheck:wireframeCutawayCheck name:@"WireframeCutaway"];
}

@end
