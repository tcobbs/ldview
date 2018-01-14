#include <TRE/TREGLExtensions.h>
#import "PrimitivesPage.h"
#include "OCLocalStrings.h"
#include <LDLib/LDPreferences.h>

@implementation PrimitivesPage

- (void)enableTextureFilterTypeUI:(BOOL)enabled
{	
	[filteringMatrix setEnabled:enabled];
	[anisoSlider setEnabled:enabled];
	[anisoLevelLabel setEnabled:enabled];
}

- (int)sliderValueFromAniso:(TCFloat32)aniso
{
	return (int)(log(aniso) / log(2.0) + 0.5);
}

- (int)anisoLevelFromSlider
{
	return (int)((TCFloat32)(1 << [anisoSlider intValue]) + 0.5f);
}

- (void)anisoLevelChanged
{
	NSCell *selectedCell = [filteringMatrix selectedCell];

	if (selectedCell && [selectedCell tag] == 1)
	{
		[anisoLevelLabel setStringValue:[NSString stringWithFormat:[OCLocalStrings get:@"AnisoNx"], [self anisoLevelFromSlider]]];
	}
	else
	{
		[anisoLevelLabel setStringValue:@""];
		[anisoSlider setIntValue:1];
	}
}

- (void)filterTypeChanged
{
	BOOL anisoEnabled = NO;
	NSCell *selectedCell = [filteringMatrix selectedCell];

	if (selectedCell && [selectedCell tag] == 1)
	{
		int numAnisoLevels = [self sliderValueFromAniso:TREGLExtensions::getMaxAnisoLevel()];

		[anisoSlider setMaxValue:numAnisoLevels];
		[anisoSlider setNumberOfTickMarks:numAnisoLevels];
		anisoEnabled = YES;
	}
	[anisoSlider setEnabled:anisoEnabled];
	[anisoLevelLabel setEnabled:anisoEnabled];
	if (anisoEnabled)
	{
		[anisoSlider setIntValue:[self sliderValueFromAniso:ldPreferences->getAnisoLevel()]];
	}
	[self anisoLevelChanged];
}

- (void)enableTexmapsUI:(BOOL)enabled
{
	[transparentTexturesLastCheck setEnabled:enabled];
	[textureOffsetSlider setEnabled:enabled];
	if (enabled)
	{
		[self setCheck:transparentTexturesLastCheck value:ldPreferences->getTexturesAfterTransparent()];
		[textureOffsetSlider setFloatValue:ldPreferences->getTextureOffsetFactor()];
	}
	else
	{
		[self setCheck:transparentTexturesLastCheck value:false];
	}
}

- (void)enableTextureFilterType
{
	long filterType = ldPreferences->getTextureFilterType();
	bool haveAniso = TREGLExtensions::haveAnisoExtension();
	float anisoLevel = ldPreferences->getAnisoLevel();

	[self enableTextureFilterTypeUI:YES];
	if (haveAniso && filterType == GL_LINEAR_MIPMAP_LINEAR && anisoLevel > 1.0f)
	{
		[filteringMatrix selectCellWithTag:1];
	}
	else
	{
		[filteringMatrix selectCellWithTag:ldPreferences->getTextureFilterType()];
	}
	[self enableTexmapsUI:[self getCheck:texmapsCheck]];
	[self filterTypeChanged];
}

- (void)disableTextureFilterType
{
	[self enableTextureFilterTypeUI:NO];
	[filteringMatrix deselectAllCells];
	[self enableTexmapsUI:[self getCheck:texmapsCheck]];
	[self filterTypeChanged];
}

- (void)enablePrimitiveSubstitutionUI:(BOOL)enabled
{
	[textureStudsCheck setEnabled:enabled];
	[self enableLabel:curveQualityLabel value:enabled];
	[curveQualitySlider setEnabled:enabled];
}

- (void)enablePrimitiveSubstitution
{
	[self enablePrimitiveSubstitutionUI:YES];
	[self groupCheck:textureStudsCheck name:@"TextureFilterType" value:ldPreferences->getTextureStuds()];
	[curveQualitySlider setIntValue:ldPreferences->getCurveQuality()];
}

- (void)disablePrimitiveSubstitution
{
	[self enablePrimitiveSubstitutionUI:NO];
	[curveQualitySlider setIntValue:0];
}

- (void)setupMiscBox
{
	[self setCheck:lowQualityStudsCheck value:!ldPreferences->getQualityStuds()];
	[self setCheck:hiResCheck value:ldPreferences->getHiResPrimitives()];
}

- (void)setup
{
	[super setup];
	[self setCheck:transparentTexturesLastCheck value:!ldPreferences->getTexturesAfterTransparent()];
	[textureOffsetSlider setFloatValue:ldPreferences->getTextureOffsetFactor()];
	[self groupCheck:primitiveSubstitutionCheck name:@"PrimitiveSubstitution" value:ldPreferences->getAllowPrimitiveSubstitution()];
	[self groupCheck:texmapsCheck name:@"TextureFilterType" value:ldPreferences->getTexmaps()];
	[self setupMiscBox];
}

- (bool)updateLdPreferences
{
	if ([self getCheck:primitiveSubstitutionCheck])
	{
		ldPreferences->setAllowPrimitiveSubstitution(true);
		ldPreferences->setTextureStuds([self getCheck:textureStudsCheck]);
		ldPreferences->setCurveQuality([curveQualitySlider intValue]);
	}
	else
	{
		ldPreferences->setAllowPrimitiveSubstitution(false);
	}
	ldPreferences->setTexmaps([self getCheck:texmapsCheck]);
	if ([self getCheck:texmapsCheck])
	{
		ldPreferences->setTexturesAfterTransparent([self getCheck:transparentTexturesLastCheck]);
		ldPreferences->setTextureOffsetFactor([textureOffsetSlider floatValue]);
	}
	if (([self getCheck:textureStudsCheck] && [self getCheck:primitiveSubstitutionCheck]) || [self getCheck:texmapsCheck])
	{
		int tag = (int)[[filteringMatrix selectedCell] tag];
		int filterType = tag;
		TCFloat32 anisoLevel = 1.0f;
		
		if (tag == 1)
		{
			filterType = GL_LINEAR_MIPMAP_LINEAR;
			anisoLevel = (TCFloat32)[self anisoLevelFromSlider];
		}
		ldPreferences->setTextureFilterType(filterType);
		ldPreferences->setAnisoLevel(anisoLevel);
	}
	ldPreferences->setQualityStuds(![self getCheck:lowQualityStudsCheck]);
	ldPreferences->setHiResPrimitives([self getCheck:hiResCheck]);
	return [super updateLdPreferences];
}

- (IBAction)resetPage:(id)sender
{
	ldPreferences->loadDefaultPrimitivesSettings(false);
	[super resetPage:sender];
}

- (IBAction)anisoLevel:(id)sender
{
	[self valueChanged:sender];
	[self anisoLevelChanged];
}

- (IBAction)primitiveSubstitution:(id)sender
{
	[self valueChanged:sender];
	[self groupCheck:sender name:@"PrimitiveSubstitution"];
}

- (IBAction)texmap:(id)sender
{
	[self groupCheck:sender name:@"TextureFilterType"];
}

- (IBAction)textureStuds:(id)sender
{
	[self groupCheck:sender name:@"TextureFilterType"];
}

- (IBAction)filterType:(id)sender
{
	[self valueChanged:sender];
	[self filterTypeChanged];
}

- (void)groupCheck:(id)sender name:(NSString *)groupName init:(BOOL)init
{
	if (sender == textureStudsCheck || sender == texmapsCheck)
	{
		if (!init)
		{
			[self valueChanged:sender];
		}
		[self setUISection:groupName enabled:([self getCheck:textureStudsCheck] && [self getCheck:primitiveSubstitutionCheck]) || [self getCheck:texmapsCheck]];
	}
	else
	{
		[super groupCheck:sender name:groupName init:init];
	}

}

@end
