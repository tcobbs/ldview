#import "GeometryPage.h"
#include <LDLib/LDPreferences.h>

@implementation GeometryPage

- (void)enableSeamsUI:(BOOL)enabled
{
	[self setCheck:seamWidthCheck value:enabled];
	[seamWidthField setEnabled:enabled];
	[seamWidthStepper setEnabled:enabled];
}

- (void)enableSeams
{
	[self enableSeamsUI:YES];
	[seamWidthField setFloatValue:ldPreferences->getSeamWidth() / 100.0];
	[seamWidthStepper takeFloatValueFrom:seamWidthField];
}

- (void)disableSeams
{
	[self enableSeamsUI:NO];
	[seamWidthField setStringValue:@""];
}

- (void)setupModelBox
{
	if (ldPreferences->getUseSeams())
	{
		[self enableSeams];
	}
	else
	{
		[self disableSeams];
	}
}

- (void)enableWireframeUI:(BOOL)enabled
{
	[self setCheck:wireframeCheck value:enabled];
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
	// Thickness
}

- (void)disableWireframe
{
	[self enableWireframeUI:NO];
	[self setCheck:fogCheck value:NO];
	[self setCheck:removeHiddenCheck value:NO];
}

- (void)setupWireframeBox
{
	if (ldPreferences->getDrawWireframe())
	{
		[self enableWireframe];
	}
	else
	{
		[self disableWireframe];
	}
}

- (void)enableBfcUI:(BOOL)enabled
{
	[self setCheck:bfcCheck value:enabled];
	[redBackFacesCheck setEnabled:enabled];
	[greenFrontFacesCheck setEnabled:enabled];
}

- (void)enableBfc
{
	[self enableBfcUI:YES];
	[self setCheck:redBackFacesCheck value:ldPreferences->getRedBackFaces()];
	[self setCheck:greenFrontFacesCheck value:ldPreferences->getGreenFrontFaces()];
}

- (void)disableBfc
{
	[self enableBfcUI:NO];
	[self setCheck:redBackFacesCheck value:false];
	[self setCheck:greenFrontFacesCheck value:false];
}

- (void)setupBfcBox
{
	if (ldPreferences->getBfc())
	{
		[self enableBfc];
	}
	else
	{
		[self disableBfc];
	}
}

- (void)enableEdgesUI:(BOOL)enabled
{
	[self setCheck:edgeLinesCheck value:enabled];
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
	[self setCheck:conditionalLinesCheck value:enabled];
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
	if (ldPreferences->getDrawConditionalHighlights())
	{
		[self enableConditionals];
	}
	else
	{
		[self disableConditionals];
	}
	[self setCheck:highQualityEdgesCheck value:ldPreferences->getUsePolygonOffset()];
	[self setCheck:blackEdgesCheck value:ldPreferences->getBlackHighlights()];
	// @ToDo: Edge thickness.
}

- (void)disableEdges
{
	[self enableEdgesUI:NO];
}

- (void)setupEdgesBox
{
	if (ldPreferences->getShowHighlightLines())
	{
		[self enableEdges];
	}
	else
	{
		[self disableEdges];
	}
}

- (void)setup
{
	[super setup];
	[self setupModelBox];
	[self setupWireframeBox];
	[self setupBfcBox];
	[self setupEdgesBox];
}

- (void)updateLdPreferences
{
	[super updateLdPreferences];
	if ([self getCheck:seamWidthCheck])
	{
		ldPreferences->setUseSeams(true);
		ldPreferences->setSeamWidth((int)([seamWidthField floatValue] * 100.0f));
	}
	else
	{
		ldPreferences->setUseSeams(false);
	}
	if ([self getCheck:wireframeCheck])
	{
		ldPreferences->setDrawWireframe(true);
		ldPreferences->setUseWireframeFog([self getCheck:fogCheck]);
		ldPreferences->setRemoveHiddenLines([self getCheck:removeHiddenCheck]);
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
		// Edge thickness.
	}
	else
	{
		ldPreferences->setShowHighlightLines(false);
	}
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
