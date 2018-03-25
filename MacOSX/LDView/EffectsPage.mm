#import "EffectsPage.h"
#include <LDLib/LDPreferences.h>
#include <TCFoundation/TCAlert.h>

@implementation EffectsPage

- (void)enableLightingUI:(BOOL)enabled
{
	[highQualityCheck setEnabled:enabled];
	[subduedCheck setEnabled:enabled];
	[specularCheck setEnabled:enabled];
	[alternateLightingCheck setEnabled:enabled];
	[self enableBoxTitle:lightDirBox value:enabled];
	[lightDirMatrix setEnabled:enabled];
	[useLightDatCheck setEnabled:enabled];
	[replaceStandardLightCheck setEnabled:enabled];
	[hideLightDatGeom setEnabled:enabled];
}

- (void)updateLightDir
{
	if (ldPreferences->getLightDirection() == LDPreferences::CustomDirection)
	{
		[lightDirMatrix deselectAllCells];
	}
	else
	{
		[lightDirMatrix selectCellWithTag:ldPreferences->getLightDirection()];
	}
}

- (void)enableLighting
{
	[self enableLightingUI:YES];
	[self setCheck:highQualityCheck value:ldPreferences->getQualityLighting()];
	[self setCheck:subduedCheck value:ldPreferences->getSubduedLighting()];
	[self setCheck:specularCheck value:ldPreferences->getUseSpecular()];
	[self setCheck:alternateLightingCheck value:ldPreferences->getOneLight()];
	[self updateLightDir];
	[self groupCheck:useLightDatCheck name:@"LightDats" value:ldPreferences->getDrawLightDats()];
	[self setCheck:hideLightDatGeom value:ldPreferences->getNoLightGeom()];
}

- (void) disableLighting
{
	[self enableLightingUI:NO];
	[self setCheck:highQualityCheck value:NO];
	[self setCheck:subduedCheck value:NO];
	[self setCheck:specularCheck value:NO];
	[self setCheck:alternateLightingCheck value:NO];
	[lightDirMatrix deselectAllCells];
	[self setCheck:useLightDatCheck value:NO];
	[self setCheck:replaceStandardLightCheck value:NO];
	[self setCheck:hideLightDatGeom value:NO];
}

- (void)enableLightDatsUI:(BOOL)enabled
{
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
	[crossEyedRadio setEnabled:enabled];
	[parallelRadio setEnabled:enabled];
	[self enableLabel:stereoAmountLabel value:enabled];
	[stereoAmountSlider setEnabled:enabled];
}

- (void)enableStereo
{
	[self enableStereoUI:YES];
	if (ldPreferences->getStereoMode() == LDVStereoNone)
	{
		[self setCheck:crossEyedRadio value:true];
		[self setCheck:parallelRadio value:false];
	}
	else
	{
		[self setCheck:crossEyedRadio value:false];
		[self setCheck:parallelRadio value:true];
	}
	[stereoAmountSlider setIntValue:ldPreferences->getStereoEyeSpacing()];
}

- (void) disableStereo
{
	[self enableStereoUI:NO];
	[self setCheck:crossEyedRadio value:false];
	[self setCheck:parallelRadio value:false];
	[stereoAmountSlider setIntValue:0];
}

- (void)enableWireframeCutawayUI:(BOOL)enabled
{
	[self setCheck:wireframeCutawayCheck value:enabled];
	[cutawayColorRadio setEnabled:enabled];
	[cutawayMonochromeRadio setEnabled:enabled];
	[self enableLabel:opacityLabel value:enabled];
	[opacitySlider setEnabled:enabled];
	[self enableLabel:wcThicknessLabel value:enabled];
	[wcThicknessSlider setEnabled:enabled];
}

- (void)enableWireframeCutaway
{
	[self enableWireframeCutawayUI:YES];
	if (ldPreferences->getCutawayMode() == LDVCutawayWireframe)
	{
		[self setCheck:cutawayColorRadio value:true];
		[self setCheck:cutawayMonochromeRadio value:false];
	}
	else
	{
		[self setCheck:cutawayColorRadio value:false];
		[self setCheck:cutawayMonochromeRadio value:true];
	}
	[opacitySlider setIntValue:ldPreferences->getCutawayAlpha()];
	[wcThicknessSlider setIntValue:ldPreferences->getCutawayThickness()];
}

- (void) disableWireframeCutaway
{
	[self enableWireframeCutawayUI:NO];
	[self setCheck:cutawayColorRadio value:false];
	[self setCheck:cutawayMonochromeRadio value:false];
	[opacitySlider setIntValue:1];
	[wcThicknessSlider setIntValue:0];
}

- (void)setupTransparencyBox
{
	[self setCheck:sortTransCheck value:ldPreferences->getSortTransparent()];
	[self setCheck:stippleTransCheck value:ldPreferences->getUseStipple()];
}

- (void)setupMiscBox
{
	[self setCheck:flatShadingCheck value:ldPreferences->getUseFlatShading()];
	[self setCheck:smoothCurvesCheck value:ldPreferences->getPerformSmoothing()];
}

- (void)setup
{
	[super setup];
	[self groupCheck:lightingCheck name:@"Lighting" value:ldPreferences->getUseLighting()];
	[self groupCheck:stereoCheck name:@"Stereo" value:ldPreferences->getStereoMode() != LDVStereoNone];
	[self groupCheck:wireframeCutawayCheck name:@"WireframeCutaway" value:ldPreferences->getCutawayMode() != LDVCutawayNormal];
	[self setupTransparencyBox];
	[self setupMiscBox];
}

- (bool)updateLdPreferences
{
	if ([self getCheck:lightingCheck])
	{
		ldPreferences->setUseLighting(true);
		ldPreferences->setQualityLighting([self getCheck:highQualityCheck]);
		ldPreferences->setSubduedLighting([self getCheck:subduedCheck]);
		ldPreferences->setUseSpecular([self getCheck:specularCheck]);
		ldPreferences->setOneLight([self getCheck:alternateLightingCheck]);
		if ([lightDirMatrix selectedCell])
		{
			ldPreferences->setLightDirection((LDPreferences::LightDirection)[[lightDirMatrix selectedCell] tag]);
		}
		if ([self getCheck:useLightDatCheck])
		{
			ldPreferences->setDrawLightDats(true);
			ldPreferences->setOptionalStandardLight([self getCheck:replaceStandardLightCheck]);
		}
		else
		{
			ldPreferences->setDrawLightDats(false);
		}
		ldPreferences->setNoLightGeom([self getCheck:hideLightDatGeom]);
	}
	else
	{
		ldPreferences->setUseLighting(false);
	}
	if ([self getCheck:stereoCheck])
	{
		LDVStereoMode stereoMode = LDVStereoCrossEyed;
		if ([self getCheck:parallelRadio])
		{
			stereoMode = LDVStereoParallel;
		}
		ldPreferences->setStereoMode(stereoMode);
		ldPreferences->setStereoEyeSpacing([stereoAmountSlider intValue]);
	}
	else
	{
		ldPreferences->setStereoMode(LDVStereoNone);
	}
	if ([self getCheck:wireframeCutawayCheck])
	{
		LDVCutawayMode cutawayMode = LDVCutawayWireframe;
		if ([self getCheck:cutawayMonochromeRadio])
		{
			cutawayMode = LDVCutawayStencil;
		}
		ldPreferences->setCutawayMode(cutawayMode);
		ldPreferences->setCutawayAlpha([opacitySlider intValue]);
		ldPreferences->setCutawayThickness([wcThicknessSlider intValue]);
	}
	else
	{
		ldPreferences->setCutawayMode(LDVCutawayNormal);
	}
	ldPreferences->setSortTransparent([self getCheck:sortTransCheck]);
	ldPreferences->setUseStipple([self getCheck:stippleTransCheck]);
	ldPreferences->setUseFlatShading([self getCheck:flatShadingCheck]);
	ldPreferences->setPerformSmoothing([self getCheck:smoothCurvesCheck]);
	return [super updateLdPreferences];
}

- (IBAction)resetPage:(id)sender
{
	ldPreferences->loadDefaultEffectsSettings(false);
	[super resetPage:sender];
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

- (IBAction)stereoModeChanged:(id)sender
{
	[self valueChanged:sender];
}

- (IBAction)cutawayModeChanged:(id)sender
{
	[self valueChanged:sender];	
}

- (IBAction)sort:(id)sender
{
	[self valueChanged:sender];
	[self setCheck:stippleTransCheck value:false];
}

- (IBAction)stipple:(id)sender
{
	[self valueChanged:sender];
	[self setCheck:sortTransCheck value:false];
}

- (IBAction)flatShading:(id)sender
{
	[self valueChanged:sender];
	[self setCheck:smoothCurvesCheck value:false];
}

- (IBAction)smoothCurves:(id)sender
{
	[self valueChanged:sender];
	[self setCheck:flatShadingCheck value:false];
}

- (void)lightVectorChanged:(TCAlert *)alert
{
	// Note: sender is ignored, because all windows get the same light
	// vector.
	[self updateLightDir];
}

@end
