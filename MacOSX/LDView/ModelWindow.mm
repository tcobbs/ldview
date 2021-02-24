#import "ModelWindow.h"
#import "LDrawModelView.h"
#import "LDViewController.h"
#import "Preferences.h"
#import "ToolbarSegmentedControl.h"
//#import "ToolbarPopUpButton.h"
#import "ErrorsAndWarnings.h"
#import "BoundingBox.h"
#import "ErrorItem.h"
#import "OCLocalStrings.h"
#import "OCUserDefaults.h"
#import "SnapshotTaker.h"
#import "SaveSnapshotViewOwner.h"
#import "SaveExportViewOwner.h"
#import "PartsList.h"
#import "ModelTree.h"
#import "MPD.h"
#import "AlertHandler.h"
#import "LDViewCategories.h"
#import "OpenGLDriverInfo.h"
#import "GeneralPage.h"
#import "GoToStep.h"
#import "LatLon.h"

#include <LDLoader/LDLError.h>
#include <LDLoader/LDLMainModel.h>
#include <LDLib/LDPreferences.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <LDLib/LDInputHandler.h>
#include <LDLib/LDPartsList.h>
#include <LDLib/LDHtmlInventory.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCStringArray.h>

#if MAC_OS_X_VERSION_MAX_ALLOWED <= 1040

enum
{
	NSSegmentStyleAutomatic = 0,
	NSSegmentStyleRounded = 1,
	NSSegmentStyleTexturedRounded = 2,
	NSSegmentStyleRoundRect = 3,
	NSSegmentStyleTexturedSquare = 4,
	NSSegmentStyleCapsule = 5,
	NSSegmentStyleSmallSquare = 6
};

@interface NSSegmentedControl (SCLeopardOnly)

- (void)setSegmentStyle:(int)style;

@end

#endif

@implementation ModelWindow

- (void)killPolling
{
	[pollingTimer invalidate];
	[pollingTimer release];
	pollingTimer = nil;
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[statusBar release];
	[toolbarItems release];
	[defaultIdentifiers release];
	[otherIdentifiers release];
	[allIdentifiers release];
	TCObject::release(alertHandler);
	alertHandler = NULL;
	[imageFileTypes release];
	[snapshotTaker release];
	[saveSnapshotViewOwner release];
	[saveExportViewOwner release];
	[modelTree release];
	[mpd release];
	[initialTitle release];
	[stepToolbarControls release];
	[self killPolling];
	[self releaseTopLevelObjects:topLevelObjects orTopLevelObject:window];
	[super dealloc];
}

- (BOOL)showStatusLatLon:(BOOL)show
{
	BOOL changed = NO;
	
	if (show)
	{
		if ([latLonBox isHidden])
		{
			NSRect frame = [progressMessage frame];
			
			[latLonBox setHidden:NO];
			frame.size.width -= latLonDelta;
			[progressMessage setFrame:frame];
			changed = YES;
		}
	}
	else
	{
		if (![latLonBox isHidden])
		{
			NSRect messageFrame = [progressMessage frame];
			NSRect latLonFrame = [latLonBox frame];
			
			latLonDelta = latLonFrame.origin.x + latLonFrame.size.width - messageFrame.origin.x - messageFrame.size.width;
			[latLonBox setHidden:YES];
			messageFrame.size.width += latLonDelta;
			[progressMessage setFrame:messageFrame];
			changed = YES;
		}
	}
	return changed;
}

- (NSToolbarItem *) toolbar:(NSToolbar *)toolbar
      itemForItemIdentifier:(NSString *)itemIdentifier
  willBeInsertedIntoToolbar:(BOOL)flag
{
	return [toolbarItems objectForKey:itemIdentifier];
}

- (NSArray *)toolbarAllowedItemIdentifiers: (NSToolbar *)toolbar
{
	return allIdentifiers;
}

- (NSArray *)toolbarDefaultItemIdentifiers: (NSToolbar *)toolbar
{
	return defaultIdentifiers;
}

- (void)alertDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	[[alert window] orderOut:self];
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	if (modelViewer == NULL || modelViewer->getMainTREModel() == NULL)
	{
		[self enableToolbarItems:NO];
	}
}

- (void)runAlertSheetWithMessageText:(NSString *)messageTitle defaultButton:(NSString *)defaultButtonTitle alternateButton:(NSString *)alternateButtonTitle otherButton:(NSString *)otherButtonTitle informativeText:(NSString *)informativeText
{
	NSAlert *alert = [NSAlert alertWithMessageText:messageTitle defaultButton:defaultButtonTitle alternateButton:alternateButtonTitle otherButton:otherButtonTitle informativeTextWithFormat:informativeText];
	
	[alert beginSheetModalForWindow:window modalDelegate:self didEndSelector:@selector(alertDidEnd:returnCode:contextInfo:) contextInfo:NULL];
}

- (NSToolbarItem *)addToolbarItemWithIdentifier:(NSString *)identifier label:(NSString *)label control:(NSControl **)pControl menuItem:(NSMenuItem *)menuItem highPriority:(BOOL)highPriority isDefault:(BOOL)isDefault
{
	NSControl *&control = *pControl;
	NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:identifier];
	NSSize size;

	if (replaceSegments && [control isKindOfClass:[NSSegmentedControl class]] && ![control isKindOfClass:[ToolbarSegmentedControl class]])
	{
		*pControl = [[ToolbarSegmentedControl alloc] initWithTemplate:*pControl];
	}
	if ([control isKindOfClass:[NSSegmentedControl class]] && [control respondsToSelector:@selector(setSegmentStyle:)])
	{
		[(NSSegmentedControl *)control setSegmentStyle:NSSegmentStyleCapsule];
	}
	size = [control frame].size;
	size.height += 1.0f;
	if (label == nil)
	{
		label = [OCLocalStrings get:identifier];
	}
	[item setLabel:label];
	[item setPaletteLabel:label];
	[item setToolTip:label];
	[item setTarget:self];
	[item setMinSize:size];
	[item setMaxSize:size];
	[control retain];
	[control removeFromSuperview];
	[item setView:control];
	[control release];
	if (highPriority)
	{
		[item setVisibilityPriority:NSToolbarItemVisibilityPriorityHigh];
	}
	else
	{
		[item setVisibilityPriority:NSToolbarItemVisibilityPriorityStandard];
	}
	[toolbarItems setObject:item forKey:identifier];
	if (isDefault)
	{
		[defaultIdentifiers addObject:identifier];
	}
	else
	{
		[otherIdentifiers addObject:identifier];
	}
	[allIdentifiers addObject:identifier];
	if (!menuItem && [control isKindOfClass:[NSSegmentedControl class]] && [(NSSegmentedControl *)control segmentCount] == 1)
	{
		menuItem = [[[NSMenuItem alloc] initWithTitle:label action:[control action] keyEquivalent:@""] autorelease];
		[menuItem setTag:[[(NSSegmentedControl *)control cell] tagForSegment:0]];
	}
	if (menuItem)
	{
		[menuItem setKeyEquivalent:@""];
		[item setMenuFormRepresentation:menuItem];
	}
	[item release];
	return item;
}

- (NSToolbarItem *)addToolbarItemWithIdentifier:(NSString *)identifier label:(NSString *)label control:(NSControl **)pControl highPriority:(BOOL)highPriority isDefault:(BOOL)isDefault
{
	return [self addToolbarItemWithIdentifier:identifier label:label control:pControl menuItem:nil highPriority:highPriority isDefault:isDefault];
}

- (void)updateSegments:(NSSegmentedControl *)segments states:(NSArray *)states alternates:(NSArray *)alternates
{
	int i;
	int count = (int)[states count];
	NSSegmentedCell *cell = [segments cell];
	
	for (i = 0; i < count; i++)
	{
		[cell setSelected:[[states objectAtIndex:i] boolValue] forSegment:i];
		if (alternates)
		{
			[self updateSegments:[alternates objectAtIndex:i] states:[NSArray arrayWithObject:[states objectAtIndex:i]] alternates:nil];
		}
	}
}

- (void)updateSegments:(NSSegmentedControl *)segments states:(NSArray *)states
{
	[self updateSegments:segments states:states alternates:nil];
}

- (void)setupSegments:(NSSegmentedControl *)segments alternates:(NSArray *)alternates toolTips:(NSArray *)toolTips
{
	int i;
	int count = (int)[segments segmentCount];
	NSSegmentedCell *cell = [segments cell];

	for (i = 0; i < count; i++)
	{
		NSString *toolTip;

		if (toolTips != nil && [toolTips count] > i)
		{
			toolTip = [toolTips objectAtIndex:i];
		}
		else
		{
			toolTip = [segments labelForSegment:i];
		}
		[cell setToolTip:toolTip forSegment:i];
		if (alternates)
		{
			[self setupSegments:[alternates objectAtIndex:i] alternates:nil toolTips:[NSArray arrayWithObject:toolTip]];
		}
	}
}

- (void)setupSegments:(NSSegmentedControl *)segments alternates:(NSArray *)alternates
{
	[self setupSegments:segments alternates:alternates toolTips:nil];
}

- (void)setupSegments:(NSSegmentedControl *)segments
{
	[self setupSegments:segments alternates:nil];
}

- (void)updateOtherFeatureStates
{
	LDPreferences *ldPreferences = [[controller preferences] ldPreferences];
	bool primSub = ldPreferences->getAllowPrimitiveSubstitution();
	NSArray *states = [NSArray arrayWithObjects:
		[NSNumber numberWithBool:ldPreferences->getTransDefaultColor()],
		[NSNumber numberWithBool:ldPreferences->getBoundingBoxesOnly()],
		[NSNumber numberWithBool:ldPreferences->getCutawayMode() != LDVCutawayNormal],
		[NSNumber numberWithBool:ldPreferences->getUseFlatShading()],
		[NSNumber numberWithBool:ldPreferences->getPerformSmoothing()],
		[NSNumber numberWithBool:primSub && ldPreferences->getTextureStuds()],
		[NSNumber numberWithBool:!ldPreferences->getQualityStuds()],
		nil];
	NSArray *alternates = [NSArray arrayWithObjects:
		transDefaultSegments,
		partBBoxesSegments,
		wireframeCutawaySegments,
		flatShadingSegments,
		smoothCurvesSegments,
		studLogosSegments,
		lowResStudsSegments,
		nil];
	BOOL enabled = primSub ? YES : NO;
	
	[self updateSegments:otherFeaturesSegments states:states alternates:alternates];
	[otherFeaturesSegments setEnabled:enabled forSegment:5];
	[studLogosSegments setEnabled:enabled forSegment:0];
	[otherFeaturesSegments setNeedsDisplay:YES];
}

- (void)updatePartsAuthorStates
{
	LDPreferences *ldPreferences = [[controller preferences] ldPreferences];
	bool conditionals = ldPreferences->getDrawConditionalHighlights() && ldPreferences->getShowHighlightLines();
	NSArray *states = [NSArray arrayWithObjects:
		[NSNumber numberWithBool:ldPreferences->getShowAxes()],
		[NSNumber numberWithBool:ldPreferences->getRandomColors()],
		[NSNumber numberWithBool:ldPreferences->getBfc()],
		[NSNumber numberWithBool:conditionals && ldPreferences->getShowAllConditionalLines()],
		[NSNumber numberWithBool:conditionals && ldPreferences->getShowConditionalControlPoints()],
		[NSNumber numberWithBool:ldPreferences->getTexmaps()],
		nil];
	NSArray *alternates = [NSArray arrayWithObjects:
		axesSegments,
		randomColorsSegments,
		bfcSegments,
		allConditionalsSegments,
		conditionalControlsSegments,
		texmapsSegments,
		nil];
	BOOL enabled = conditionals ? YES : NO;

	[self updateSegments:partsAuthorSegments states:states alternates:alternates];
	[partsAuthorSegments setEnabled:enabled forSegment:3];
	[partsAuthorSegments setEnabled:enabled forSegment:4];
	[allConditionalsSegments setEnabled:enabled forSegment:0];
	[conditionalControlsSegments setEnabled:enabled forSegment:0];
}

- (void)updateFeatureStates
{
	LDPreferences *ldPreferences = [[controller preferences] ldPreferences];
	NSArray *states = [NSArray arrayWithObjects:
		[NSNumber numberWithBool:ldPreferences->getDrawWireframe()],
		[NSNumber numberWithBool:ldPreferences->getUseSeams()],
		[NSNumber numberWithBool:ldPreferences->getShowHighlightLines()],
		[NSNumber numberWithBool:ldPreferences->getAllowPrimitiveSubstitution()],
		[NSNumber numberWithBool:ldPreferences->getUseLighting()],
		nil];
	NSArray *alternates = [NSArray arrayWithObjects:
		wireframeSegments,
		seamsSegments,
		edgesSegments,
		primitivesSegments,
		lightingSegments,
		nil];
	
	[self updateSegments:featuresSegments states:states alternates:alternates];
	[featuresSegments setNeedsDisplay:YES];
}

- (BOOL)haveLatLon
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer)
	{
		if (modelViewer->getViewMode() == LDrawModelViewer::VMExamine && modelViewer->getExamineMode() == LDrawModelViewer::EMLatLong)
		{
			return YES;
		}
	}
	return NO;
}

- (void)updateOtherStates
{
	long viewMode = [modelView viewMode];
	bool examineMode = viewMode == LDInputHandler::VMExamine;
	bool flyThroughMode = viewMode == LDInputHandler::VMFlyThrough;
//	bool walkMode = viewMode == LDInputHandler::VMWalk;

	[[boundingBoxSegments cell] setSelected:[[BoundingBox sharedInstance] isVisible] forSegment:0];
	[[latLonRotationSegments cell] setSelected:examineMode && examineLatLong forSegment:0];
	[latLonRotationSegments setEnabled:!flyThroughMode forSegment:0];
}

- (void)updateStates
{
	[self updateFeatureStates];
	[self updatePartsAuthorStates];
	[self updateOtherFeatureStates];
	[self updateOtherStates];
}

- (void)preferencesDidUpdate:(NSNotification *)notification
{
	[self updateStates];
}

- (void)setupSteps
{
	NSArray *alternates = [NSArray arrayWithObjects:
		stepFirstSegments,
		stepPrevSegments,
		stepNextSegments,
		stepLastSegments,
		nil];

	[self addToolbarItemWithIdentifier:@"StepFirst" label:nil control:&stepFirstSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"StepPrev" label:nil control:&stepPrevSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"StepPrev2" label:[OCLocalStrings get:@"StepPrev"] control:&stepPrevSegments2 highPriority:YES isDefault:NO];
	[self addToolbarItemWithIdentifier:@"StepField" label:nil control:&stepField highPriority:NO isDefault:YES];
	[self addToolbarItemWithIdentifier:@"StepNext2" label:[OCLocalStrings get:@"StepNext"] control:&stepNextSegments2 highPriority:YES isDefault:NO];
	[self addToolbarItemWithIdentifier:@"StepNext" label:nil control:&stepNextSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"StepLast" label:nil control:&stepLastSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"Step" label:nil control:&stepSegments highPriority:YES isDefault:YES];
	[self addToolbarItemWithIdentifier:@"Step2" label:[OCLocalStrings get:@"Step"] control:&stepSegments2 highPriority:YES isDefault:NO];
	[self setupSegments:stepSegments2 alternates:alternates];
	[self setupSegments:stepSegments];
	[self setupSegments:stepPrevSegments2];
	[self setupSegments:stepNextSegments2];
}

- (void)setupOtherActions
{
	NSArray *alternates = [NSArray arrayWithObjects:
		fullScreenSegments,
		zoomToFitSegments,
		errorsSegments,
		partsListSegments,
		modelTreeSegments,
		mpdSegments,
		povCameraSegments,
		helpSegments,
		nil];

	[self addToolbarItemWithIdentifier:@"OtherActions" label:nil control:&otherActionsSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"FullScreen" label:nil control:&fullScreenSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"ZoomToFit" label:nil control:&zoomToFitSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"Errors&Warnings" label:nil control:&errorsSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"PartsList" label:nil control:&partsListSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"ModelTree" label:nil control:&modelTreeSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"MPDModelSelection" label:nil control:&mpdSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"POVCameraInfo" label:nil control:&povCameraSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"Help" label:nil control:&helpSegments highPriority:NO isDefault:NO];
	[self setupSegments:otherActionsSegments alternates:alternates];
}

- (void)setupFileActions
{
	NSArray *alternates = [NSArray arrayWithObjects:
		openButton,
		snapshotButton,
		exportButton,
		reloadButton,
		nil];

	[self addToolbarItemWithIdentifier:@"Actions" label:nil control:&fileActionsSegments highPriority:YES isDefault:YES];
	[self addToolbarItemWithIdentifier:@"OpenFile" label:nil control:&openButton highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"SaveSnapshot" label:nil control:&snapshotButton highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"Export" label:nil control:&exportButton highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"Reload" label:nil control:&reloadButton highPriority:NO isDefault:NO];
	[self setupSegments:fileActionsSegments alternates:alternates];
}

- (void)setupViewingAngles
{
	NSArray *alternates = [NSArray arrayWithObjects:
		viewFrontSegments,
		viewBackSegments,
		viewLeftSegments,
		viewRightSegments,
		viewTopSegments,
		viewBottomSegments,
		viewLatLonSegments,
		viewTwoThirdsSegments,
		nil];

	[self addToolbarItemWithIdentifier:@"ViewingAngles" label:nil control:&viewSegments1 highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"ViewingAnglesAlt" label:nil control:&viewSegments2 highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"FrontView" label:nil control:&viewFrontSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"BackView" label:nil control:&viewBackSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"LeftView" label:nil control:&viewLeftSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"RightView" label:nil control:&viewRightSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"TopView" label:nil control:&viewTopSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"BottomView" label:nil control:&viewBottomSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"SpecifyLatLon" label:nil control:&viewLatLonSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"TwoThirdsView" label:nil control:&viewTwoThirdsSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"View" label:[OCLocalStrings get:@"SelectView"] control:&viewingAngleSegments highPriority:YES isDefault:NO];
	[self setupSegments:viewSegments1 alternates:alternates];
	[self setupSegments:viewSegments2];
	[self setupSegments:viewingAngleSegments alternates:nil toolTips:[NSArray arrayWithObject:[OCLocalStrings get:@"SelectView"]]];
}

- (void)setExamineLatLong:(bool)value
{
	LDrawModelViewer::ExamineMode examineMode = LDrawModelViewer::EMFree;
	
	if (value)
	{
		examineMode = LDrawModelViewer::EMLatLong;
	}
	[modelView modelViewer]->setExamineMode(examineMode);
	TCUserDefaults::setLongForKey(examineMode, EXAMINE_MODE_KEY, false);
	[self showStatusLatLon:[self haveLatLon]];
}

- (void)setupViewMode
{
	[self addToolbarItemWithIdentifier:@"ViewMode" label:nil control:&viewModeSegments highPriority:YES isDefault:YES];
	[self setupSegments:viewModeSegments];
	[self setViewMode:TCUserDefaults::longForKey(VIEW_MODE_KEY, LDInputHandler::VMExamine, false)];
	[self setKeepRightSideUp:TCUserDefaults::boolForKey(KEEP_RIGHT_SIDE_UP_KEY, false, false)];
	examineLatLong = TCUserDefaults::longForKey(EXAMINE_MODE_KEY, LDrawModelViewer::EMFree, false) == LDrawModelViewer::EMLatLong;
	[self setExamineLatLong:examineLatLong];
}

- (void)setupOtherFeatures
{
	NSArray *alternates = [NSArray arrayWithObjects:
		transDefaultSegments,
		partBBoxesSegments,
		wireframeCutawaySegments,
		flatShadingSegments,
		smoothCurvesSegments,
		studLogosSegments,
		lowResStudsSegments,
		nil];

	[self addToolbarItemWithIdentifier:@"OtherFeatures" label:nil control:&otherFeaturesSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"TransDefaultColor" label:nil control:&transDefaultSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"PartBBoxes" label:nil control:&partBBoxesSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"WireframeCutaway" label:nil control:&wireframeCutawaySegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"FlatShading" label:nil control:&flatShadingSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"SmoothCurves" label:nil control:&smoothCurvesSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"StudLogos" label:nil control:&studLogosSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"LowResStuds" label:nil control:&lowResStudsSegments highPriority:NO isDefault:NO];
	[self setupSegments:otherFeaturesSegments alternates:alternates];
}

- (void)setupPartsAuthor
{
	NSArray *alternates = [NSArray arrayWithObjects:
		axesSegments,
		randomColorsSegments,
		bfcSegments,
		allConditionalsSegments,
		conditionalControlsSegments,
		texmapsSegments,
		nil];

	[self addToolbarItemWithIdentifier:@"PartsAuthor" label:nil control:&partsAuthorSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"Axes" label:nil control:&axesSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"RandomColors" label:nil control:&randomColorsSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"BFC" label:nil control:&bfcSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"AllConditionals" label:nil control:&allConditionalsSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"ConditionalControls" label:nil control:&conditionalControlsSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"Texmaps" label:nil control:&texmapsSegments highPriority:NO isDefault:NO];
	[self setupSegments:partsAuthorSegments alternates:alternates];
}

- (void)setupFeatures
{
	NSArray *alternates = [NSArray arrayWithObjects:
		wireframeSegments,
		seamsSegments,
		edgesSegments,
		primitivesSegments,
		lightingSegments,
		nil];

	[self addToolbarItemWithIdentifier:@"Features" label:nil control:&featuresSegments highPriority:YES isDefault:YES];
	[self addToolbarItemWithIdentifier:@"Wireframe" label:nil control:&wireframeSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"Seams" label:nil control:&seamsSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"EdgeLines" label:nil control:&edgesSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"Primitives" label:nil control:&primitivesSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"Lighting" label:nil control:&lightingSegments highPriority:NO isDefault:NO];
	[self setupSegments:featuresSegments alternates:alternates];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(preferencesDidUpdate:) name:LDPreferencesDidUpdateNotification object:nil];
}

- (void)updateStatusLatLon
{
	if ([self haveLatLon])
	{
		LDrawModelViewer *modelViewer = [modelView modelViewer];

		if (modelViewer != NULL)
		{
			int lon = (int)modelViewer->getExamineLongitude();

			[latField setIntValue:(int)modelViewer->getExamineLatitude()];
			if (lon < -179)
			{
				lon += 360;
			}
			if (lon > 180)
			{
				lon -= 360;
			}
			[lonField setIntValue:lon];
			[latLonBox display];
		}
	}
}

- (void)setKeepRightSideUp:(bool)value
{
	[modelView setKeepRightSideUp:value];
}

- (void)setViewMode:(long)newViewMode;
{
	[modelView setViewMode:newViewMode];
	switch (newViewMode)
	{
		case LDInputHandler::VMExamine:
			[viewModeSegments selectSegmentWithTag:0];
			break;
		case LDInputHandler::VMFlyThrough:
			[viewModeSegments selectSegmentWithTag:1];
			break;
		case LDInputHandler::VMWalk:
			[viewModeSegments selectSegmentWithTag:2];
			break;
	}
	[self showStatusLatLon:[self haveLatLon]];
}

- (void)setupToolbarItems
{
	toolbarItems = [[NSMutableDictionary alloc] init];
	defaultIdentifiers = [[NSMutableArray alloc] init];
	otherIdentifiers = [[NSMutableArray alloc] init];
	allIdentifiers = [[NSMutableArray alloc] init];

	[self setupFileActions];
	[self setupOtherActions];
	[self setupFeatures];
	[self setupPartsAuthor];
	[self setupOtherFeatures];
	[self setupViewingAngles];
	[self setupViewMode];
	[self setupSteps];
	[defaultIdentifiers addObject:NSToolbarFlexibleSpaceItemIdentifier];	
	[self addToolbarItemWithIdentifier:@"Prefs" label:nil control:&prefsSegments menuItem:[[[controller prefsMenuItem] copy] autorelease] highPriority:NO isDefault:YES];
	[printSegments setTarget:controller];
	[self addToolbarItemWithIdentifier:@"Print" label:nil control:&printSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"LatLonRotation" label:nil control:&latLonRotationSegments highPriority:NO isDefault:NO];
	[self addToolbarItemWithIdentifier:@"BoundingBox" label:nil control:&boundingBoxSegments highPriority:NO isDefault:NO];
	stepToolbarControls = [[NSArray alloc] initWithObjects:stepSegments, stepSegments2, stepPrevSegments, stepPrevSegments2, stepNextSegments, stepNextSegments2, stepFirstSegments, stepLastSegments, nil];
	[allIdentifiers addObjectsFromArray:[NSArray arrayWithObjects:
		NSToolbarFlexibleSpaceItemIdentifier,
		NSToolbarSpaceItemIdentifier,
		NSToolbarSeparatorItemIdentifier,
		nil]];
	[viewingAngleSegments setMenu:[[[controller viewingAngleMenu] copy] autorelease] forSegment:0];
	[self updateStates];
}

- (void)setupToolbar
{
	[self setupToolbarItems];
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"LDViewToolbar"];
	[toolbar setDelegate:self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconOnly];
	[toolbar setSizeMode:NSToolbarSizeModeSmall];
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setAutosavesConfiguration:YES];
}

- (BOOL)showStatusBar:(BOOL)show
{
	BOOL changed = NO;
	
	if (show)
	{
		if ([statusBar isHidden])
		{
			NSRect modelViewFrame1 = [modelView frame];
			float height = [statusBar frame].size.height;
			
			modelViewFrame1.size.height -= height;
			modelViewFrame1.origin.y += height;
			[modelView setFrame:modelViewFrame1];
			[statusBar setHidden:NO];
			changed = YES;
		}
	}
	else
	{
		if (![statusBar isHidden])
		{
			NSRect modelViewFrame2 = [modelView frame];
			float height = [statusBar frame].size.height;
			
			modelViewFrame2.size.height += height;
			modelViewFrame2.origin.y -= height;
			[statusBar setHidden:YES];
			[modelView setFrame:modelViewFrame2];
			changed = YES;
		}
	}
	return changed;
}

- (void)awakeFromNib
{
	progressWidth = [progress frame].size.width;
	progressMargin = [progress frame].origin.x;
	replaceSegments = true;
	[stepsMenu release];
	initialTitle = [[window title] retain];
	showStatusBar = [OCUserDefaults longForKey:@"StatusBar" defaultValue:1 sessionSpecific:NO];
	[self showStatusBar:showStatusBar];
	[self showStatusLatLon:[self haveLatLon]];
	[self setupToolbar];
	[window setToolbar:toolbar];
	[statusBar retain];
	progressAdjust = [progressMessage frame].origin.x - [progress frame].origin.x;
	[window setNextResponder:controller];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(errorFilterChange:) name:LDErrorFilterChange object:nil];
	imageFileTypes = [[NSArray alloc] initWithObjects:@"png", @"bmp", nil];
	[self enableToolbarItems:NO];
}

- (void)stepChanged
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	int newStep = 0;
	int i, j;
	
	if (modelViewer && modelViewer->getFilename() && (newStep = modelViewer->getStep()) > 0)
	{
		[stepField setStringValue:[NSString stringWithFormat:@"%d", newStep]];
	}
	else
	{
		[stepField setStringValue:[OCLocalStrings get:@"DashDash"]];
	}
	for (i = 0; i < [stepToolbarControls count]; i++)
	{
		NSSegmentedControl *control = [stepToolbarControls objectAtIndex:i];
		NSSegmentedCell *cell = [control cell];
		
		for (j = 0; j < [cell segmentCount]; j++)
		{
			BOOL enabled = NO;
		
			switch ([cell tagForSegment:j])
			{
				case -1:
				case 0:
					enabled = newStep > 1;
					break;
				case 1:
				case 2:
					enabled = modelViewer && newStep < modelViewer->getNumSteps() && newStep > 0;
					break;
			}
			[control setEnabled:enabled forSegment:j];
		}
	}
}

- (void)changeStep:(int)action
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer)
	{
		int newStep;
		
		if (action == 0)
		{
			newStep = 1;
		}
		else if (action == 2)
		{
			newStep = modelViewer->getNumSteps();
		}
		else
		{
			newStep = modelViewer->getStep() + action;
		}
		if (newStep <  1)
		{
			newStep = 1;
		}
		else if (newStep > modelViewer->getNumSteps())
		{
			newStep = modelViewer->getNumSteps();
		}
		modelViewer->setStep(newStep);
		[self stepChanged];
		[modelView rotationUpdate];
	}
}

- (IBAction)takeStepFrom:(id)sender
{
	if ([sender isKindOfClass:[NSSegmentedControl class]])
	{
		[self changeStep:(int)[[sender cell] tagForSegment:[sender selectedSegment]]];
	}
	else
	{
		[self changeStep:(int)[sender tag]];
	}
}

- (void)setStep:(int)step
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];

	if (step > 0 && step <= modelViewer->getNumSteps())
	{
		modelViewer->setStep(step);
	}
	else
	{
		NSBeep();
	}
	[self stepChanged];
	[modelView rotationUpdate];
}

- (void)controlTextDidEndEditing:(NSNotification *)notification
{
	if ([notification object] == stepField)
	{
		[self setStep:[stepField intValue]];
	}
}

- (IBAction)goToStep:(id)sender
{
	GoToStep *sheet = [[GoToStep alloc] init];
	
	if ([sheet runSheetInWindow:window] == NSModalResponseOK)
	{
		[self setStep:[sheet step]];
	}
	[sheet release];
}

- (id)initWithController:(LDViewController *)value
{
	if ((self = [super init]) != nil)
	{
		controller = value;
		[self ldvLoadNibNamed:@"ModelWindow" topLevelObjects:&topLevelObjects];
		[topLevelObjects retain];
		alertHandler = new AlertHandler(self);
	}
	return self;
}

- (void)enableToolbarItems:(BOOL)enabled
{
	NSEnumerator *enumerator = [allIdentifiers objectEnumerator];
	
	while (NSString *identifier = [enumerator nextObject])
	{
		NSToolbarItem *item = [toolbarItems objectForKey:identifier];
		NSControl *control = (NSControl *)[item view];

		if (control == fileActionsSegments)
		{
			[fileActionsSegments setEnabled:enabled forSegment:1];
			[fileActionsSegments setEnabled:enabled forSegment:2];
			[fileActionsSegments setEnabled:enabled forSegment:3];
		}
		else if (control == openButton)
		{
			[openButton setEnabled:YES forSegment:0];
		}
		else if ([control isKindOfClass:[NSControl class]])
		{
			[control setEnabled:enabled];
		}
	}
	[self updateStates];
}

- (ErrorItem *)filteredRootErrorItem
{
//	return [[ErrorsAndWarnings sharedInstance] filteredRootErrorItem:unfilteredRootErrorItem];
	if (!filteredRootErrorItem)
	{
		filteredRootErrorItem = [[[ErrorsAndWarnings sharedInstance] filteredRootErrorItem:unfilteredRootErrorItem] retain];
	}
	return filteredRootErrorItem;
}

- (BOOL)openModel:(NSString *)filename
{
	NSRange colonRange = [filename rangeOfString:@":" options:NSBackwardsSearch];
	[unfilteredRootErrorItem release];
	unfilteredRootErrorItem = nil;
	[filteredRootErrorItem release];
	filteredRootErrorItem = nil;
	if (colonRange.location != NSNotFound)
	{
		[window setTitleWithRepresentedFilename:[filename substringToIndex:colonRange.location]];
	}
	else
	{
		[window setTitleWithRepresentedFilename:filename];
	}
	[window makeKeyAndOrderFront:self];
	if ([modelView openModel:filename])
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

- (bool)loading
{
	return loading;
}

- (bool)parsing
{
	return parsing;
}

- (bool)loadCanceled
{
	return loadCanceled;
}

- (LDrawModelView *)modelView
{
	return modelView;
}

- (bool)isMyAlert:(TCAlert *)alert
{
	TCAlertSender *sender = alert->getSender();

	return sender != NULL && (sender->getAlertSender() == [modelView modelViewer] || sender->getAlertSender() == [snapshotTaker ldSnapshotTaker]);
}

- (void)addErrorItem:(ErrorItem *)parent string:(NSString *)string error:(LDLError *)error
{
	[parent addChild:[[[ErrorItem alloc] initWithString:string error:error includeIcon:NO] autorelease]];
}

- (void)addErrorItem:(ErrorItem *)parent attributedString:(NSAttributedString *)attributedString error:(LDLError *)error
{
	[parent addChild:[[[ErrorItem alloc] initWithAttributedString:attributedString error:error] autorelease]];
}

- (void)ldlErrorCallback:(LDLError *)error
{
	if (![self isMyAlert:error] || !loading)
	{
		return;
	}
	TCStringArray *extraInfo = error->getExtraInfo();
	NSString *lineString;

	if (!unfilteredRootErrorItem)
	{
		unfilteredRootErrorItem = [[ErrorItem alloc] init];
	}
	ErrorItem *errorItem = [unfilteredRootErrorItem addChild:[[[ErrorItem alloc] initWithString:[NSString stringWithUTF8String:error->getMessage()] error:error includeIcon:YES] autorelease]];
	if (error->getFilename())
	{
		lineString = [NSString stringWithFormat:@"%@%s", [OCLocalStrings get:@"ErrorTreeFilePrefix"], error->getFilename()];
	}
	else
	{
		lineString = [OCLocalStrings get:@"ErrorTreeUnknownFile"];
	}
	[self addErrorItem:errorItem string:lineString error:error];
	const char* formattedLine = error->getFormattedFileLine();
	if (formattedLine != NULL)
	{
		int lineNumber = error->getLineNumber();
		
		if (lineNumber > 0)
		{
			lineString = [NSString stringWithFormat:[OCLocalStrings get:@"ErrorTreeLine#"], lineNumber];
			[self addErrorItem:errorItem string:lineString error:error];
			NSString *format = [OCLocalStrings get:@"ErrorTreeLine"];
			lineString = [NSString stringWithFormat:format, formattedLine];
			NSUInteger lineStart = [format rangeOfString:@"%s"].location;
			if (lineStart == NSNotFound)
			{
				[self addErrorItem:errorItem string:lineString error:error];
			}
			else
			{
				NSMutableAttributedString *attributedString = [[NSMutableAttributedString alloc] initWithString:lineString];
				CGFloat fontSize = [NSFont systemFontSize];
				[attributedString addAttribute:NSFontAttributeName value:[NSFont systemFontOfSize:fontSize] range:NSMakeRange(0, lineStart)];
				NSFont *fixedFont = [NSFont fontWithName:@"Andale Mono" size:fontSize];
				if (fixedFont == nil)
				{
					fixedFont = [NSFont userFixedPitchFontOfSize:fontSize];
				}
				[attributedString addAttribute:NSFontAttributeName value:fixedFont range:NSMakeRange(lineStart, [attributedString length] - lineStart)];
				[self addErrorItem:errorItem attributedString:attributedString error:error];
				[attributedString release];
			}
		}
		else
		{
			lineString = [OCLocalStrings get:@"ErrorTreeUnknownLine#"];
			[self addErrorItem:errorItem string:lineString error:error];
		}
	}
	else
	{
		lineString = [OCLocalStrings get:@"ErrorTreeUnknownLine"];
		[self addErrorItem:errorItem string:lineString error:error];
	}
	if (extraInfo)
	{
		for (int i = 0; i < extraInfo->getCount(); i++)
		{
			[self addErrorItem:errorItem string:[NSString stringWithUTF8String:extraInfo->stringAtIndex(i)] error:error];
		}
	}
}

- (void)adjustProgressMessageSize:(float)amount
{
	NSRect frame = [progressMessage frame];
	frame.origin.x -= amount;
	frame.size.width += amount;
	[progressMessage setFrame:frame];
}

- (void)cancelLoad:(id)sender
{
	loadCanceled = true;
}

- (void)setProgressHidden:(BOOL)hidden
{
	CGFloat layoutConstant;
	if (hidden)
	{
		layoutConstant = progressMargin;
	}
	else
	{
		layoutConstant = progressWidth + progressMargin * 2;
	}
	[progressMessageLeft setConstant:layoutConstant];
	[progress setHidden:hidden];
}

- (void)progressAlertCallback:(TCProgressAlert *)alert
{
	if (![self isMyAlert:alert])
	{
		return;
	}
	static NSDate *lastProgressUpdate = NULL;
	float alertProgress = alert->getProgress();
	NSString *alertMessage = [NSString stringWithUTF8String:alert->getMessage()];
	BOOL forceUpdate = NO;
	BOOL updated = NO;

	if (![alertMessage isEqualToString:[progressMessage stringValue]] && (!forceProgress || [alertMessage length] > 0))
	{
		[progressMessage setStringValue:alertMessage];
		forceUpdate = YES;
	}
	if (alertProgress >= 0.0f && alertProgress <= 1.0f)
	{
		// Don't update the progress more than 5 times per second, or loading
		// the model can take a REALLY long time.
		if (alertProgress == 1.0f || lastProgressUpdate == NULL ||
			[lastProgressUpdate timeIntervalSinceNow] < -0.2)
		{
			NSEvent *event;
			unsigned int mouseEventsMask = NSMouseMovedMask | NSLeftMouseDraggedMask | NSRightMouseDraggedMask | NSMouseEnteredMask | NSMouseExitedMask | NSOtherMouseDraggedMask;
			//unsigned int mouseEventsMask = NSLeftMouseDown | NSLeftMouseUp | NSRightMouseDown | NSRightMouseUp | NSMouseMoved | NSLeftMouseDragged | NSRightMouseDragged | NSMouseEntered | NSMouseExited | NSOtherMouseDown | NSOtherMouseUp | NSOtherMouseDragged;
			//unsigned int mask = NSAnyEventMask & ~mouseEventsMask;

			if ([progress isHidden])
			{
				[self setProgressHidden:NO];
				[self adjustProgressMessageSize: -progressAdjust];
			}
			[window makeFirstResponder:progress];
			[progress setDoubleValue:alertProgress];
			[statusBar display];
			updated = YES;
			[lastProgressUpdate release];
			lastProgressUpdate = [[NSDate alloc] init];
			// Flush the main event loop.
			while ((event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES]) != NULL)
			{
				bool skip = false;
				
				if ([event type] == NSKeyDown || [event type] == NSKeyUp)
				{
					skip = true;
					if ([[event characters] characterAtIndex:0] == 27)
					{
						[[controller currentModelWindow] cancelLoad:self];
					}
				}
				else if ([event window] == window)
				{
					if (((1 << [event type]) & mouseEventsMask) == 0)
					{
						skip = true;
					}
				}
				if (!skip)
				{
					[NSApp sendEvent:event];
				}
			}
		}
	}
	else if (alertProgress == 2.0f)
	{
		[progress setDoubleValue:1.0];
		if (![progress isHidden] && !forceProgress)
		{
			[self setProgressHidden:YES];
			[self adjustProgressMessageSize: progressAdjust]; 
		}
		//[self showStatusBar:showStatusBar];
	}
	if (forceUpdate && !updated)
	{
		[statusBar display];
	}
	if (loadCanceled)
	{
		alert->abort();
	}
}

- (void)modelViewerAlertCallback:(TCAlert *)alert
{
	[modelView modelViewerAlertCallback:alert];
}

- (void)setLastWriteTime:(NSDate *)value
{
	if (value != lastWriteTime)
	{
		[lastWriteTime release];
		lastWriteTime = [value retain];
	}
}

- (void)pollingUpdate
{
	[self reload:self];
}

- (void)pollingAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSAlertDefaultReturn)
	{
		[[alert window] orderOut:self];
		[self pollingUpdate];
	}
}

- (void)askForPollingUpdate
{
	NSAlert *alert = [NSAlert alertWithMessageText:[OCLocalStrings get:@"PollFileUpdate"] defaultButton:[OCLocalStrings get:@"Yes"] alternateButton:[OCLocalStrings get:@"No"] otherButton:nil informativeTextWithFormat:[OCLocalStrings get:@"PollReloadCheck"]];
	[alert beginSheetModalForWindow:window modalDelegate:self didEndSelector:@selector(pollingAlertDidEnd:returnCode:contextInfo:) contextInfo:NULL];
}

- (NSDictionary *)fileAttributes:(NSString *)filename
{
	NSFileManager *fileManager = [NSFileManager defaultManager];

	NSDictionary *retValue = [fileManager attributesOfItemAtPath:filename error:NULL];
	if ([[retValue objectForKey:NSFileType] isEqualToString:NSFileTypeSymbolicLink])
	{
		return [self fileAttributes:[fileManager destinationOfSymbolicLinkAtPath:filename error:NULL]];
	}
	return retValue;
}

- (NSDate *)lastModifiedTime:(NSString *)filename
{
	return [[self fileAttributes:filename] objectForKey:NSFileModificationDate];
}

- (void)pollingTimerFired:(NSTimer*)theTimer
{
	NSString *filename = [self filename];

	if (filename)
	{
		NSDictionary *attributes = [self fileAttributes:[self filename]];
		NSDate *thisWriteTime = [attributes objectForKey:NSFileModificationDate];
		if (![lastWriteTime isEqualToDate:thisWriteTime])
		{
			NSInteger fileSize = [[attributes objectForKey:NSFileSize] integerValue];
			if (fileSize != mainFileSize)
			{
				// A full half second must pass without any changes to the file
				// size before we consider the file update to be complete.
				mainFileSize = fileSize;
				return;
			}
			mainFileSize = 0;
			[self setLastWriteTime:thisWriteTime];
			switch ([controller pollingMode])
			{
				case 1:
					pollingUpdateNeeded = true;
					break;
				case 2:
					if ([controller currentModelWindow] == self)
					{
						[self pollingUpdate];
					}
					else
					{
						pollingUpdateNeeded = true;
					}
					break;
				case 3:
					[self pollingUpdate];
					break;
				default:
					break;
			}
		}
	}
}

- (NSString *)filename
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer)
	{
		return [NSString stringWithUTF8String:modelViewer->getFilename()];
	}
	else
	{
		return nil;
	}
}

- (void)updatePolling
{
	NSString *filename = [self filename];
	long pollingMode = [controller pollingMode];

	if (pollingMode && filename != nil && [modelView modelViewer]->getMainModel() != NULL)
	{
		if (!pollingTimer)
		{
			pollingTimer = [[NSTimer scheduledTimerWithTimeInterval:0.5f target:self selector:@selector(pollingTimerFired:) userInfo:nil repeats:YES] retain];
		}
	}
	else if (pollingTimer)
	{
		[self killPolling];
	}
}

- (void)updateUtilityWindows:(id)sender andShowErrorsIfNeeded:(BOOL)andShowErrorsIfNeeded
{
	ModelWindow *modelWindow = closing ? nil : self;
	if ([ErrorsAndWarnings sharedInstanceIsVisible] || andShowErrorsIfNeeded)
	{
		[[ErrorsAndWarnings sharedInstance] update:modelWindow];
	}
	if (andShowErrorsIfNeeded)
	{
		[[ErrorsAndWarnings sharedInstance] showIfNeeded];
	}
	if ([BoundingBox sharedInstanceIsVisible])
	{
		[[BoundingBox sharedInstance] update:modelWindow];
	}
}

- (void)updateUtilityWindows:(id)sender
{
	[self updateUtilityWindows:self andShowErrorsIfNeeded:NO];
}

- (void)postLoad
{
	if ([self showStatusBar:showStatusBar] || [self showStatusLatLon:[self haveLatLon]])
	{
		[window display];
	}
	[modelView rotationUpdate];
	[self stepChanged];
	[self updateFps];
	[self updatePolling];
}

- (void)loadAlertCallback:(TCAlert *)alert
{
	LDrawModelViewer *modelViewer = (LDrawModelViewer *)alert->getSender();
	
	if (modelViewer == [modelView modelViewer])
	{
		NSString *message = [NSString stringWithUTF8String:alert->getMessage()];

		pollingUpdateNeeded = false;
		if ([message isEqualToString:@"ModelLoading"])
		{
			[self killPolling];
			loading = true;
			loadCanceled = false;
			if ([self showStatusBar:YES] || [self showStatusLatLon:NO])
			{
				[window display];
			}
			[unfilteredRootErrorItem release];
			unfilteredRootErrorItem = nil;
		}
		else if ([message isEqualToString:@"ModelParsing"])
		{
			[self killPolling];
			parsing = true;
			if (!loading)
			{
				loadCanceled = false;
				if ([self showStatusBar:YES] || [self showStatusLatLon:NO])
				{
					[window display];
				}
			}
		}
		else if ([message isEqualToString:@"ModelParsed"] || [message isEqualToString:@"ModelParseCanceled"])
		{
			if (!loading)
			{
				[self postLoad];
			}
			parsing = false;
		}
		else if ([message isEqualToString:@"ModelLoaded"])
		{
			bool showErrorsIfNeeded = [[[controller preferences] generalPage] showErrorsIfNeeded];
			NSString *filename = nil;

			[self enableToolbarItems:YES];
			loading = false;
			filename = [self filename];
			[OCUserDefaults setString:[filename stringByDeletingLastPathComponent] forKey:[NSString stringWithUTF8String:LAST_OPEN_PATH_KEY] sessionSpecific:NO];
			[self setLastWriteTime:[self lastModifiedTime:filename]];
			[[self controller] recordRecentFile:filename];
			[self postLoad];
			[self updateUtilityWindows:self andShowErrorsIfNeeded:showErrorsIfNeeded];
		}
		else if ([message isEqualToString:@"ModelLoadCanceled"])
		{
			if ([self showStatusBar:showStatusBar] || [self showStatusLatLon:[self haveLatLon]])
			{
				[window display];
			}
			if (!loadCanceled)
			{
				[self runAlertSheetWithMessageText:[OCLocalStrings get:@"Error"] defaultButton:[OCLocalStrings get:@"OK"] alternateButton:nil otherButton:nil informativeText:[NSString stringWithFormat: [OCLocalStrings get:@"ErrorLoadingModel"], modelViewer->getFilename()]];
			}
			loading = false;
			[[self window] setTitleWithRepresentedFilename:@""];
			[[self window] setTitle:initialTitle];
			[self postLoad];
			[self updateUtilityWindows:self];
		}
		[[NSNotificationCenter defaultCenter] postNotificationName:message object:self];
	}
}

- (void)redrawAlertCallback:(TCAlert *)alert
{
	[modelView redrawAlertCallback:alert];
}

- (void)captureAlertCallback:(TCAlert *)alert
{
	[modelView captureAlertCallback:alert];
}

- (void)releaseAlertCallback:(TCAlert *)alert
{
	[modelView releaseAlertCallback:alert];
}

//- (void)peekMouseUpAlertCallback:(TCAlert *)alert
//{
//	[modelView peekMouseUpAlertCallback:alert];
//}

- (void)reloadNeeded
{
    if (![mpd isKeyWindow])
    {
        [window makeKeyWindow];
    }
	[modelView reloadNeeded];
}

- (void)modelWillReload
{
	[self performSelectorOnMainThread:@selector(reloadNeeded) withObject:nil waitUntilDone:NO];
}

- (void)windowWillClose:(NSNotification *)aNotification
{
	if ([aNotification object] == window)
	{
		closing = YES;
		[self updateUtilityWindows:nil];
		[window setDelegate:nil];
		[self killPolling];
		[controller modelWindowWillClose:self];
	}
}

- (float)fps
{
	return fps;
}

- (void)showFps
{
	[modelView setFps:0.0f];
	if ([[controller preferences] ldPreferences]->getShowFps() && [modelView modelViewer]->getMainTREModel())
	{
		if (showStatusBar)
		{
			if (fps > 0.0f)
			{
				[progressMessage setStringValue:[NSString stringWithFormat:[OCLocalStrings get:@"FPSFormat"], fps]];
			}
			else
			{
				[progressMessage setStringValue:[OCLocalStrings get:@"FPSSpinPrompt"]];
			}
		}
		else
		{
			[modelView setFps:fps];
		}
	}
	else
	{
		[progressMessage setStringValue:@""];
	}
}

- (void)updateFps
{
	if (fpsReferenceDate)
	{
		NSTimeInterval interval = -[fpsReferenceDate timeIntervalSinceNow];
		
		if (interval >= 0.25)
		{
			[fpsReferenceDate release];
			fpsReferenceDate = [[NSDate alloc] init];
			fps = 1.0 / interval * fpsFrameCount;
			fpsFrameCount = 1;
		}
		else
		{
			fpsFrameCount++;
		}
	}
	else
	{
		fpsReferenceDate = [[NSDate alloc] init];
	}
	[self showFps];
}

- (void)clearFps
{
	[fpsReferenceDate release];
	fpsReferenceDate = nil;
	fps = 0.0f;
	fpsFrameCount = 1;
	[self showFps];
	if (!showStatusBar)
	{
		[modelView rotationUpdate];
	}
}

- (void)setShowStatusBar:(BOOL)value
{
	showStatusBar = value;
	[self showStatusBar:showStatusBar];
}

- (BOOL)showStatusBar
{
	return showStatusBar && ![self isFullScreen];
}

- (void)show
{
	[window makeKeyAndOrderFront:self];
}

- (NSWindow *)window
{
	return window;
}

- (NSToolbar *)toolbar
{
	return toolbar;
}

- (IBAction)open:(id)sender
{
	[controller openModel:sender];
}

- (const char *)savePanelPath:(NSSavePanel *)savePanel
{
	return savePanel.URL.ldvFileSystemRepresentation;
}

- (void)exportSavePanelDidEnd:(NSSavePanel *)savePanel returnCode:(NSModalResponse)returnCode
{
	if (returnCode == NSModalResponseOK)
	{
		LDrawModelViewer *modelViewer = [modelView modelViewer];
		[saveExportViewOwner saveSettings];
		modelViewer->exportCurModel([self savePanelPath:savePanel], [[LDViewController appVersion] UTF8String], [[LDViewController appCopyright] UTF8String]);
	}
	[saveExportViewOwner setSavePanel:nil];
	sheetBusy = false;
}

- (void)htmlSavePanelDidEnd:(NSSavePanel *)savePanel returnCode:(NSModalResponse)returnCode
{
	if (returnCode == NSModalResponseOK)
	{
		LDrawModelViewer *modelViewer = [modelView modelViewer];
		if (htmlInventory->generateHtml([self savePanelPath:savePanel], partsList, modelViewer->getCurFilename().c_str()))
		{
			if (htmlInventory->isSnapshotNeeded())
			{
				LDrawModelViewer *modelViewer = [modelView modelViewer];
				bool origSteps = TCUserDefaults::boolForKey(SAVE_STEPS_KEY, false, false);
				int origStep = modelViewer->getStep();

				modelViewer->setStep(modelViewer->getNumSteps());
				TCUserDefaults::setBoolForKey(false, SAVE_STEPS_KEY, false);
				if (!snapshotTaker)
				{
					snapshotTaker = [[SnapshotTaker alloc] initWithModelViewer:modelViewer sharedContext:[modelView openGLContext]];
				}
				htmlInventory->prepForSnapshot(modelViewer);
				[snapshotTaker saveFile:[NSString stringWithUTF8String:htmlInventory->getSnapshotPath()] width:400 height:300 zoomToFit:YES];
				htmlInventory->restoreAfterSnapshot(modelViewer);
				modelViewer->setStep(origStep);
				TCUserDefaults::setBoolForKey(origSteps, SAVE_STEPS_KEY, false);
				[modelView rotationUpdate];
			}
			if (htmlInventory->getShowFileFlag())
			{
				[[NSWorkspace sharedWorkspace] openFile:[savePanel URL].path];
			}
		}
	}
	htmlInventory->release();
	partsList->release();
	sheetBusy = false;
}

- (NSString *)defaultSaveDirForOp:(LDPreferences::SaveOp)op
{
	return [[[controller preferences] generalPage] defaultSaveDirForOp:op modelFilename:[self filename]];
}

- (void)snapshotSavePanelDidEnd:(NSSavePanel *)savePanel returnCode:(NSModalResponse)returnCode
{
	if (returnCode == NSModalResponseOK)
	{
		NSSize viewSize = [modelView frame].size;
		int width = (int)viewSize.width;
		int height = (int)viewSize.height;		

		[saveSnapshotViewOwner saveSettings];
		[OCUserDefaults setString:[savePanel URL].path forKey:[NSString stringWithUTF8String:LAST_SNAPSHOT_DIR_KEY] sessionSpecific:NO];
		if (!snapshotTaker)
		{
			snapshotTaker = [[SnapshotTaker alloc] initWithModelViewer:[modelView modelViewer] sharedContext:[modelView openGLContext]];
		}
		if ([saveSnapshotViewOwner matchWindowScale])
		{
			[snapshotTaker ldSnapshotTaker]->setScaleFactor([modelView modelViewer]->getScaleFactor());
		}
		else
		{
			[snapshotTaker ldSnapshotTaker]->setScaleFactor(1.0f);
		}
		[snapshotTaker setImageType:[saveSnapshotViewOwner imageType]];
		[snapshotTaker setTrySaveAlpha:[saveSnapshotViewOwner transparentBackground]];
		[snapshotTaker setAutoCrop:[saveSnapshotViewOwner autocrop]];
		[savePanel orderOut:self];
		if ([self showStatusBar:YES])
		{
			[window display];
		}
		forceProgress = true;
		[snapshotTaker saveFile:[savePanel URL].path width:[saveSnapshotViewOwner width:width] height:[saveSnapshotViewOwner height:height] zoomToFit:[saveSnapshotViewOwner zoomToFit]];
		[saveSnapshotViewOwner saveSettings];
		forceProgress = false;
		if (![progress isHidden])
		{
			[self setProgressHidden:YES];
			[self adjustProgressMessageSize: progressAdjust];
		}
		[self updateFps];
		if ([self showStatusBar:showStatusBar])
		{
			[window display];
		}
	}
	[saveSnapshotViewOwner setSavePanel:nil];
	sheetBusy = false;
}

- (bool)sheetBusy
{
	return sheetBusy;
}

- (IBAction)saveSnapshot:(id)sender
{
	NSSavePanel *savePanel = [NSSavePanel savePanel];
	NSString *modelFilename = [self filename];
	NSString *defaultFilename = [[modelFilename lastPathComponent] stringByDeletingPathExtension];

	if (!saveSnapshotViewOwner)
	{
		saveSnapshotViewOwner = [[SaveSnapshotViewOwner alloc] init];
	}
	[saveSnapshotViewOwner setNumSteps:[modelView modelViewer]->getNumSteps()];
	[saveSnapshotViewOwner setSavePanel:savePanel];
	savePanel.canSelectHiddenExtension = YES;
	NSString *dir = [self defaultSaveDirForOp:LDPreferences::SOSnapshot];
	if (dir != nil && dir.length > 0)
	{
		savePanel.directoryURL = [NSURL fileURLWithPath:dir];
	}
	savePanel.nameFieldStringValue = defaultFilename;
	sheetBusy = true;
	[savePanel beginSheetModalForWindow:window completionHandler:^(NSModalResponse response){
		[self snapshotSavePanelDidEnd:savePanel returnCode:response];
	}];
}

- (IBAction)reload:(id)sender
{
	[modelView reload];
}

- (IBAction)exportModel:(id)sender
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer)
	{
		NSSavePanel *savePanel = [NSSavePanel savePanel];
		NSString *defaultFilename;
		std::string curFilename;
		
		if (!saveExportViewOwner)
		{
			saveExportViewOwner = [[SaveExportViewOwner alloc] initWithModelViewer:modelViewer];
		}
		[saveExportViewOwner setSavePanel:savePanel];
		curFilename = modelViewer->getCurFilename();
		defaultFilename = [[[[NSString stringWithUTF8String:curFilename.c_str()] lastPathComponent] stringByDeletingPathExtension] stringByAppendingPathExtension:[saveExportViewOwner requiredFileType]];
		savePanel.canSelectHiddenExtension = YES;
		NSString *dir = [self defaultSaveDirForOp:LDPreferences::SOExport];
		if (dir != nil && dir.length > 0)
		{
			savePanel.directoryURL = [NSURL fileURLWithPath:dir];
		}
		savePanel.nameFieldStringValue = defaultFilename;
		sheetBusy = true;
		[savePanel beginSheetModalForWindow:window completionHandler:^(NSModalResponse response){
			[self exportSavePanelDidEnd:savePanel returnCode:response];
		}];
	}
}

- (IBAction)fileActions:(id)sender
{
	switch ([[sender cell] tagForSegment:[sender selectedSegment]])
	{
		case 0:
			[self open:sender];
			break;
		case 1:
			[self saveSnapshot:sender];
			break;
		case 2:
			[self exportModel:sender];
			break;
		case 3:
			[self reload:sender];
			break;
		default:
			NSLog(@"Unknown action.\n");
			break;
	}
}

- (IBAction)zoomToFit:(id)sender
{
	[modelView zoomToFit:sender];
}

- (IBAction)errorsAndWarnings:(id)sender
{
	[[ErrorsAndWarnings sharedInstance] update:self];
	[[ErrorsAndWarnings sharedInstance] show:self];
}

- (IBAction)modelTree:(id)sender
{
	if (!modelTree)
	{
		modelTree = [[ModelTree alloc] initWithParent:self];
	}
	[modelTree toggle];
}

- (IBAction)mpd:(id)sender
{
	if (!mpd)
	{
		mpd = [[MPD alloc] initWithParent:self];
	}
	[mpd toggle];
}

- (IBAction)showPovCameraInfo:(id)sender
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer != NULL)
	{
		UCSTR message;
		char *povCamera;
		
		modelViewer->getPovCameraInfo(message, povCamera);
		if (message && povCamera)
		{
			if (NSRunAlertPanel([OCLocalStrings get:@"PovCameraTitle"], [NSString stringWithUCString:message], [OCLocalStrings get:@"OK"], [OCLocalStrings get:@"Cancel"], nil) == NSModalResponseOK)
			{
				[self copyStringToPasteboard:[NSString stringWithUTF8String:povCamera]];
			}
		}
		delete[] message;
		delete[] povCamera;
	}
}

- (IBAction)otherActions:(id)sender
{
	switch ([[sender cell] tagForSegment:[sender selectedSegment]])
	{
		case 0:
			[[NSApplication sharedApplication] sendAction:@selector(toggleFullScreen:) to:nil from:sender];
			break;
		case 1:
			[self zoomToFit:sender];
			break;
		case 2:
			[self errorsAndWarnings:sender];
			break;
		case 3:
			[self partsList:sender];
			break;
		case 4:
			[self modelTree:sender];
			break;
		case 5:
			[self mpd:sender];
			break;
		case 6:
			[self showPovCameraInfo:sender];
			break;
		case 7:
			[controller showHelp:sender];
			break;
		default:
			NSLog(@"Unknown action.\n");
			break;
	}
}

- (void)toggleFeature:(SEL)selector sender:(id)sender
{
	[[controller preferences] performSelector:selector withObject:sender];
	[self updateStates];
}

- (IBAction)otherFeatures:(id)sender
{
	switch ([[sender cell] tagForSegment:[sender selectedSegment]])
	{
		case 10:
			[self toggleFeature:@selector(takeTransDefaultFrom:) sender:sender];
			break;
		case 11:
			[self toggleFeature:@selector(takePartBBoxesFrom:) sender:sender];
			break;
		case 12:
			[self toggleFeature:@selector(takeWireframeCutawayFrom:) sender:sender];
			break;
		case 13:
			[self toggleFeature:@selector(takeFlatShadingFrom:) sender:sender];
			break;
		case 14:
			[self toggleFeature:@selector(takeSmoothCurvesFrom:) sender:sender];
			break;
		case 15:
			[self toggleFeature:@selector(takeStudLogosFrom:) sender:sender];
			break;
		case 16:
			[self toggleFeature:@selector(takeLowResStudsFrom:) sender:sender];
			break;
		default:
			NSLog(@"Unknown author feature.\n");
			break;
	}
}

- (IBAction)partsAuthor:(id)sender
{
	switch ([[sender cell] tagForSegment:[sender selectedSegment]])
	{
		case 5:
			[self toggleFeature:@selector(takeShowAxesFrom:) sender:sender];
			break;
		case 6:
			[self toggleFeature:@selector(takeRandomColorsFrom:) sender:sender];
			break;
		case 7:
			[self toggleFeature:@selector(takeBfcFrom:) sender:sender];
			break;
		case 8:
			[self toggleFeature:@selector(takeAllConditionalsFrom:) sender:sender];
			break;
		case 9:
			[self toggleFeature:@selector(takeConditionalControlsFrom:) sender:sender];
			break;
		case 10:
			[self toggleFeature:@selector(takeTexmapsFrom:) sender:sender];
			break;
		default:
			NSLog(@"Unknown author feature.\n");
			break;
	}
}

- (IBAction)features:(id)sender
{
	switch ([[sender cell] tagForSegment:[sender selectedSegment]])
	{
		case 0:
			[self toggleFeature:@selector(takeWireframeFrom:) sender:sender];
			break;
		case 1:
			[self toggleFeature:@selector(takeSeamsFrom:) sender:sender];
			break;
		case 2:
			[self toggleFeature:@selector(takeEdgesFrom:) sender:sender];
			break;
		case 3:
			[self toggleFeature:@selector(takePrimSubFrom:) sender:sender];
			break;
		case 4:
			[self toggleFeature:@selector(takeLightingFrom:) sender:sender];
			break;
		default:
			NSLog(@"Unknown feature.\n");
			break;
	}
}

- (IBAction)resetView:(id)sender
{
	[modelView resetView:sender];
}

- (IBAction)specifyLatLon:(id)sender
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer != NULL)
	{
		LatLon *sheet = [[LatLon alloc] init];
		
		[sheet setDefaultDist:modelViewer->getDefaultDistance()];
		[sheet setCurrentDist:modelViewer->getDistance()];
		if ([sheet runSheetInWindow:window] == NSModalResponseOK)
		{
			modelViewer->setLatLon([sheet lat], [sheet lon], [sheet dist]);
		}
		[sheet release];
	}
}

- (IBAction)viewingAngle:(id)sender
{
	int tag = -1;

	if ([sender isKindOfClass:[NSSegmentedControl class]])
	{
		tag = (int)[[sender cell] tagForSegment:[sender selectedSegment]];
	}
	else
	{
		tag = (int)[sender tag];
	}
	if (tag == 8)
	{
		[self specifyLatLon:sender];
	}
	else if (tag >= 0)
	{
		[modelView setViewingAngle:tag];
	}
}

- (IBAction)saveViewingAngle:(id)sender
{
	[[controller preferences] saveViewingAngle:self];
}

- (IBAction)preferences:(id)sender
{
	[controller preferences:sender];
}

- (IBAction)alwaysOnTop:(id)sender
{
	if ([window level] == NSNormalWindowLevel)
	{
		[window setLevel:NSPopUpMenuWindowLevel];
	}
	else
	{
		[window setLevel:NSNormalWindowLevel];
	}
}

- (IBAction)toggleStatusBar:(id)sender
{
	[self setShowStatusBar:!showStatusBar];
	[OCUserDefaults setLong:showStatusBar forKey:@"StatusBar" sessionSpecific:NO];
}

- (IBAction)customizeToolbar:(id)sender
{
	[toolbar runCustomizationPalette:sender];
}

- (IBAction)viewMode:(id)sender
{
	[modelView viewMode:sender];
	[self updateOtherStates];
}

- (IBAction)boundingBox:(id)sender
{
	[[BoundingBox sharedInstance] update:self];
	[[BoundingBox sharedInstance] show:self];
	[self updateOtherStates];
}

- (void)windowDidBecomeMain:(NSNotification *)aNotification
{
	if (pollingUpdateNeeded)
	{
		pollingUpdateNeeded = false;
		switch ([controller pollingMode])
		{
			case 1:
				[self askForPollingUpdate];
				break;
			case 2:
				[self pollingUpdate];
				break;
		}
	}
	[self updateUtilityWindows:self];
}

- (void)errorFilterChange:(NSNotification *)aNotification
{
	[filteredRootErrorItem release];
	filteredRootErrorItem = nil;
	if ([controller currentModelWindow] == self)
	{
		[[ErrorsAndWarnings sharedInstance] update:self];
	}
}

- (LDViewController *)controller
{
	return controller;
}

- (void)lightVectorChanged:(TCAlert *)alert
{
	[modelView modelViewer]->setLightVector([[controller preferences] ldPreferences]->getLightVector());
	[modelView rotationUpdate];
	[[controller preferences] lightVectorChanged:alert];
}

- (IBAction)povCameraAspect:(id)sender
{
	LDrawModelViewer *modelViewer = [[self modelView] modelViewer];
	modelViewer->setPovCameraAspect(!modelViewer->getPovCameraAspect(), true);
}

- (IBAction)latLongRotation:(id)sender
{
	examineLatLong = !examineLatLong;
	[self setExamineLatLong:examineLatLong];
	[self updateOtherStates];
	[modelView setNeedsDisplay:YES];
}

- (IBAction)examineMode:(id)sender
{
	[self setViewMode:LDInputHandler::VMExamine];
	[self updateOtherStates];
}

- (IBAction)flyThroughMode:(id)sender
{
	[self setViewMode:LDInputHandler::VMFlyThrough];
	[self updateOtherStates];
}

- (IBAction)walkMode:(id)sender
{
	[self setViewMode:LDInputHandler::VMWalk];
	[self updateOtherStates];
}

- (IBAction)keepRightSideUp:(id)sender
{
	[self setKeepRightSideUp:![self keepRightSideUp]];
	[modelView setNeedsDisplay:YES];
}

- (bool)examineLatLong
{
	return examineLatLong;
}

- (bool)examineMode
{
	return [modelView examineMode];
}

- (long)viewMode
{
	return [modelView viewMode];
}

- (bool)keepRightSideUp
{
	return [modelView keepRightSideUp];
}

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];

	if (modelViewer != NULL && modelViewer->getFilename() != NULL)
	{
		return [controller validateMenuItem:menuItem];
	}
	else
	{
		return menuItem == [controller statusBarMenuItem];
	}
}

- (IBAction)partsList:(id)sender
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer)
	{
		partsList = modelViewer->getPartsList();
		
		if (partsList)
		{
			htmlInventory = new LDHtmlInventory;
			PartsList *partsListSheet = [[PartsList alloc] initWithModelWindow:self htmlInventory:htmlInventory];

			if ([partsListSheet runSheetInWindow:window] == NSModalResponseOK)
			{
				NSString *htmlFilename = [NSString stringWithUTF8String:htmlInventory->defaultFilename([modelView modelViewer]->getCurFilename().c_str()).c_str()];
				NSSavePanel *savePanel = [NSSavePanel savePanel];
				NSString *defaultFilename = [[htmlFilename lastPathComponent] stringByDeletingPathExtension];

				savePanel.allowedFileTypes = [NSArray arrayWithObject:@"html"];
				savePanel.canSelectHiddenExtension = YES;
				NSString *dir = [self defaultSaveDirForOp:LDPreferences::SOPartsList];
				if (dir != nil && dir.length > 0)
				{
					savePanel.directoryURL = [NSURL fileURLWithPath:dir];
				}
				savePanel.nameFieldStringValue = defaultFilename;
				sheetBusy = true;
				[savePanel beginSheetModalForWindow:window completionHandler:^(NSModalResponse response){
					[self htmlSavePanelDidEnd:savePanel returnCode:response];
				}];
			}
			[partsListSheet release];
		}
	}

}

- (bool)isModelTreeOpen
{
	return [modelTree isOpen];
}

- (bool)isMPDOpen
{
	return [mpd isOpen];
}

- (void)copyStringToPasteboard:(NSString *)string
{
	NSPasteboard *pb = [NSPasteboard generalPasteboard];

	[pb declareTypes:[NSArray arrayWithObject:NSStringPboardType] owner:self];
	[pb setString:string forType:NSStringPboardType];
}

- (IBAction)showViewInfo:(id)sender
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer != NULL)
	{
		ucstring message;
		ucstring commandLine;
		
		if (modelViewer->getViewInfo(message, commandLine))
		{
			if (NSRunAlertPanel([OCLocalStrings get:@"ViewInfoTitle"], [NSString stringWithUCString:message], [OCLocalStrings get:@"OK"], [OCLocalStrings get:@"Cancel"], nil) == NSModalResponseOK)
			{
				[self copyStringToPasteboard:[NSString stringWithUCString:commandLine]];
			}
		}
	}
}

- (IBAction)openGLDriverInfo:(id)sender
{
	LDrawModelViewer *modelViewer = [modelView modelViewer];
	
	if (modelViewer)
	{
		int numExtensions;
		UCSTR openGlMessage = modelViewer->getOpenGLDriverInfo(numExtensions);
		
		if (openGlMessage)
		{
			OpenGLDriverInfo *driverInfo = [[OpenGLDriverInfo alloc] init];
			
			[driverInfo showWithInfo:[NSString stringWithUCString:openGlMessage] numExtensions:numExtensions];
			delete[] openGlMessage;
			[driverInfo release];
		}
	}
}

//- (IBAction)print:(id)sender
//{
//	NSRunAlertPanel(@"Error", @"Print not yet implemented.", @"OK", nil, nil);
//}

- (IBAction)pollingMode:(id)sender
{
	[self updatePolling];
}

- (BOOL)canCopy
{
	if ([window firstResponder] == [modelTree outlineView])
	{
		return [modelTree canCopy];
	}
	else
	{
		return NO;
	}
}

- (IBAction)copy:(id)sender
{
	if ([window firstResponder] == [modelTree outlineView])
	{
		return [modelTree copy:sender];
	}
}

- (NSSize)mainMarginSize
{
	NSSize contentSize = [modelView frame].size;
	NSSize windowSize = [window frame].size;

	return NSMakeSize(windowSize.width - contentSize.width, windowSize.height - contentSize.height);
}

- (void)setMainViewWidth:(int)width height:(int)height
{
	NSRect screenRect = [[window screen] visibleFrame];
	NSSize contentSize = [modelView frame].size;
	NSRect windowFrame = [window frame];
	float deltaHeight = (float)height - contentSize.height;

	windowFrame.size.width += (float)width - contentSize.width;
	windowFrame.size.height += deltaHeight;
	windowFrame.origin.y -= deltaHeight;
	if (windowFrame.origin.x + windowFrame.size.width > screenRect.origin.x + screenRect.size.width)
	{
		windowFrame.origin.x = screenRect.origin.x + screenRect.size.width - windowFrame.size.width;
	}
	if (windowFrame.origin.y < screenRect.origin.y)
	{
		windowFrame.origin.y = screenRect.origin.y;
	}
	[window setFrame:windowFrame display:YES];
}

- (NSApplicationPresentationOptions)window:(NSWindow *)window
	  willUseFullScreenPresentationOptions:(NSApplicationPresentationOptions)proposedOptions
{
	return proposedOptions | NSApplicationPresentationAutoHideToolbar;
}

- (void)windowWillEnterFullScreen:(NSNotification *)notification
{
	[self showStatusBar:NO];
}

- (void)windowDidEnterFullScreen:(NSNotification *)notification
{
	[controller setStatusBarMenuItemDisabled:YES];
}

- (void)windowWillExitFullScreen:(NSNotification *)notification
{
	[controller setStatusBarMenuItemDisabled:NO];
	if (showStatusBar)
	{
		[self showStatusBar:YES];
	}
}

- (BOOL)isFullScreen
{
	return ([[NSApplication sharedApplication] currentSystemPresentationOptions] & NSApplicationPresentationFullScreen) != 0;
}

- (void)escapePressed
{
	if ([self isFullScreen])
	{
		[[NSApplication sharedApplication] sendAction:@selector(toggleFullScreen:) to:nil from:self];
	}
}

@end
