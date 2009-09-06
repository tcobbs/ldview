#include <TRE/TREGLExtensions.h>
#import "PrimitivesPage.h"
#include "OCLocalStrings.h"
#include <LDLib/LDPreferences.h>

@implementation PrimitivesPage

- (void)enableTextureStudsUI:(BOOL)enabled
{	
	[filteringMatrix setEnabled:enabled];
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

- (void)enableTextureStuds
{
	long filterType = ldPreferences->getTextureFilterType();
	bool haveAniso = TREGLExtensions::haveAnisoExtension();
	float anisoLevel = ldPreferences->getAnisoLevel();

	[self enableTextureStudsUI:YES];
	if (haveAniso && filterType == GL_LINEAR_MIPMAP_LINEAR && anisoLevel > 1.0f)
	{
		[filteringMatrix selectCellWithTag:1];
	}
	else
	{
		[filteringMatrix selectCellWithTag:ldPreferences->getTextureFilterType()];
	}
	[self filterTypeChanged];
}

- (void)disableTextureStuds
{
	[self enableTextureStudsUI:NO];
	[filteringMatrix deselectAllCells];
	[self filterTypeChanged];
}

- (void)enablePrimitiveSubstitutionUI:(BOOL)enabled
{
	[textureStudsCheck setEnabled:enabled];
	[filteringMatrix setEnabled:enabled];
	[anisoSlider setEnabled:enabled];
	[anisoLevelLabel setEnabled:enabled];
	[self enableLabel:curveQualityLabel value:enabled];
	[curveQualitySlider setEnabled:enabled];
}

- (void)enablePrimitiveSubstitution
{
	[self enablePrimitiveSubstitutionUI:YES];
	[self groupCheck:textureStudsCheck name:@"TextureStuds" value:ldPreferences->getTextureStuds()];
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
	[self groupCheck:primitiveSubstitutionCheck name:@"PrimitiveSubstitution" value:ldPreferences->getAllowPrimitiveSubstitution()];
	[self setupMiscBox];
}

- (bool)updateLdPreferences
{
	if ([self getCheck:primitiveSubstitutionCheck])
	{
		int tag = [[filteringMatrix selectedCell] tag];

		ldPreferences->setAllowPrimitiveSubstitution(true);
		ldPreferences->setTextureStuds([self getCheck:textureStudsCheck]);
		if ([self getCheck:textureStudsCheck])
		{
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
		ldPreferences->setCurveQuality([curveQualitySlider intValue]);
	}
	else
	{
		ldPreferences->setAllowPrimitiveSubstitution(false);
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

- (IBAction)textureStuds:(id)sender
{
	[self valueChanged:sender];
	[self groupCheck:sender name:@"TextureStuds"];
}

- (IBAction)filterType:(id)sender
{
	[self valueChanged:sender];
	[self filterTypeChanged];
}

@end
