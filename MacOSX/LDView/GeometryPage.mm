#import "GeometryPage.h"
#import "LDViewCategories.h"
#include <LDLib/LDPreferences.h>

@implementation GeometryPage

- (void)enableSeamsUI:(BOOL)enabled
{
	[seamWidthField setEnabled:enabled];
	[seamWidthStepper setEnabled:enabled];
}

- (void)enableSeams
{
	[self enableSeamsUI:YES];
	[seamWidthField setDoubleValue:ldPreferences->getSeamWidth() / 100.0];
	[seamWidthStepper takeFloatValueFrom:seamWidthField];
}

- (void)disableSeams
{
	[self enableSeamsUI:NO];
	[seamWidthField setStringValue:@""];
}

- (void)setupModelBox
{
	[self groupCheck:seamWidthCheck name:@"Seams" value:ldPreferences->getUseSeams()];
	[boundingBoxesOnlyCheck setCheck:ldPreferences->getBoundingBoxesOnly()];
}

- (void)enableWireframeUI:(BOOL)enabled
{
	[fogCheck setEnabled:enabled];
	[removeHiddenCheck setEnabled:enabled];
	[wireframeThicknessSlider setEnabled:enabled];
	[self enableLabel:wireframeThicknessLabel value:enabled];
}

- (void)enableWireframe
{
	[self enableWireframeUI:YES];
	[self setCheck:fogCheck value:ldPreferences->getUseWireframeFog()];
	[self setCheck:removeHiddenCheck
			 value:ldPreferences->getRemoveHiddenLines()];
	[wireframeThicknessSlider setIntValue:ldPreferences->getWireframeThickness()];
}

- (void)disableWireframe
{
	[self enableWireframeUI:NO];
	[self setCheck:fogCheck value:NO];
	[self setCheck:removeHiddenCheck value:NO];
	[wireframeThicknessSlider setIntValue:1];
}

- (void)enableBfcUI:(BOOL)enabled
{
	[redBackFacesCheck setEnabled:enabled];
	[greenFrontFacesCheck setEnabled:enabled];
	[blueNeutralFacesCheck setEnabled:enabled];
}

- (void)enableBfc
{
	[self enableBfcUI:YES];
	[self setCheck:redBackFacesCheck value:ldPreferences->getRedBackFaces()];
	[self setCheck:greenFrontFacesCheck value:ldPreferences->getGreenFrontFaces()];
	[self setCheck:blueNeutralFacesCheck value:ldPreferences->getBlueNeutralFaces()];
}

- (void)disableBfc
{
	[self enableBfcUI:NO];
	[self setCheck:redBackFacesCheck value:false];
	[self setCheck:greenFrontFacesCheck value:false];
	[self setCheck:blueNeutralFacesCheck value:false];
}

- (void)enableEdgesUI:(BOOL)enabled
{
	[edgesOnlyCheck setEnabled:enabled];
	[conditionalLinesCheck setEnabled:enabled];
	[showAllConditionalsCheck setEnabled:enabled];
	[showControlPtsCheck setEnabled:enabled];
	[highQualityEdgesCheck setEnabled:enabled];
	[blackEdgesCheck setEnabled:enabled];
	[self enableLabel:edgeThicknessLabel value:enabled];
	[edgeThicknessSlider setEnabled:enabled];
}

- (void)enableCondtionalsUI:(BOOL)enabled
{
	[showAllConditionalsCheck setEnabled:enabled];
	[showControlPtsCheck setEnabled:enabled];
}

- (void)enableConditionals
{
	[self enableCondtionalsUI:YES];
	[self setCheck:showAllConditionalsCheck value:ldPreferences->getShowAllConditionalLines()];
	[self setCheck:showControlPtsCheck value:ldPreferences->getShowConditionalControlPoints()];
}

- (void)disableConditionals
{
	[self enableCondtionalsUI:NO];
	[self setCheck:showAllConditionalsCheck value:false];
	[self setCheck:showControlPtsCheck value:false];
}

- (void)enableEdges
{
	[self enableEdgesUI:YES];
	[self setCheck:edgesOnlyCheck value:ldPreferences->getEdgesOnly()];
	[self groupCheck:conditionalLinesCheck name:@"Conditionals" value:ldPreferences->getDrawConditionalHighlights()];
	[self setCheck:highQualityEdgesCheck value:ldPreferences->getUsePolygonOffset()];
	[self setCheck:blackEdgesCheck value:ldPreferences->getBlackHighlights()];
	[edgeThicknessSlider setIntValue:ldPreferences->getEdgeThickness()];
}

- (void)disableEdges
{
	[self enableEdgesUI:NO];
	[self setCheck:edgesOnlyCheck value:NO];
	[self setCheck:conditionalLinesCheck value:NO];
	[self setCheck:highQualityEdgesCheck value:NO];
	[self setCheck:blackEdgesCheck value:NO];
	[edgeThicknessSlider setIntValue:1];
}

- (void)setup
{
	[super setup];
	[self setupModelBox];
	[self groupCheck:wireframeCheck name:@"Wireframe" value:ldPreferences->getDrawWireframe()];
	[self groupCheck:bfcCheck name:@"Bfc" value:ldPreferences->getBfc()];
	[self groupCheck:edgeLinesCheck name:@"Edges" value:ldPreferences->getShowHighlightLines()];
}

- (bool)updateLdPreferences
{
	if ([self getCheck:seamWidthCheck])
	{
		ldPreferences->setUseSeams(true);
		ldPreferences->setSeamWidth((int)([seamWidthField floatValue] * 100.0f));
	}
	else
	{
		ldPreferences->setUseSeams(false);
	}
	ldPreferences->setBoundingBoxesOnly([boundingBoxesOnlyCheck getCheck]);
	if ([self getCheck:wireframeCheck])
	{
		ldPreferences->setDrawWireframe(true);
		ldPreferences->setUseWireframeFog([self getCheck:fogCheck]);
		ldPreferences->setRemoveHiddenLines([self getCheck:removeHiddenCheck]);
		ldPreferences->setWireframeThickness([wireframeThicknessSlider intValue]);
	}
	else
	{
		ldPreferences->setDrawWireframe(false);
	}
	if ([self getCheck:bfcCheck])
	{
		ldPreferences->setBfc(true);
		ldPreferences->setRedBackFaces([self getCheck:redBackFacesCheck]);
		ldPreferences->setGreenFrontFaces([self getCheck:greenFrontFacesCheck]);
		ldPreferences->setBlueNeutralFaces([self getCheck:blueNeutralFacesCheck]);
	}
	else
	{
		ldPreferences->setBfc(false);
	}
	if ([self getCheck:edgeLinesCheck])
	{
		ldPreferences->setShowHighlightLines(true);
		ldPreferences->setEdgesOnly([self getCheck:edgesOnlyCheck]);
		ldPreferences->setDrawConditionalHighlights([self getCheck:conditionalLinesCheck]);
		ldPreferences->setShowAllConditionalLines([self getCheck:showAllConditionalsCheck]);
		ldPreferences->setShowConditionalControlPoints([self getCheck:showControlPtsCheck]);
		ldPreferences->setUsePolygonOffset([self getCheck:highQualityEdgesCheck]);
		ldPreferences->setBlackHighlights([self getCheck:blackEdgesCheck]);
		ldPreferences->setEdgeThickness([edgeThicknessSlider intValue]);
	}
	else
	{
		ldPreferences->setShowHighlightLines(false);
	}
	return [super updateLdPreferences];
}

- (IBAction)resetPage:(id)sender
{
	ldPreferences->loadDefaultGeometrySettings(false);
	[super resetPage:sender];
}

- (IBAction)seamWidth:(id)sender
{
	[self groupCheck:sender name:@"Seams"];
}

- (IBAction)wireframe:(id)sender
{
	[self groupCheck:sender name:@"Wireframe"];
}

- (IBAction)bfc:(id)sender
{
	[self groupCheck:sender name:@"Bfc"];
}

- (IBAction)edgeLines:(id)sender
{
	[self groupCheck:sender name:@"Edges"];
}

- (IBAction)conditionalLines:(id)sender
{
	[self groupCheck:sender name:@"Conditionals"];
}

@end
