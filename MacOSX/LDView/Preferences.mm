#import "Preferences.h"
#import "LDViewCategories.h"
#include "ModelWindow.h"
#include "LDrawModelView.h"
#include "PreferencePage.h"
#include "LDViewController.h"
#include <LDLib/LDPreferences.h>

@implementation Preferences

NSString *LDPreferencesDidUpdateNotification = @"LDPreferencesDidUpdate";

- (id)initWithController:(LDViewController *)value
{
	if ((self = [super init]) != nil)
	{
		controller = value;	// Don't retain: it's our parent.
		pages = [[NSMutableArray alloc] initWithCapacity:[[tabView tabViewItems] count]];
		ldPreferences = new LDPreferences;

		[self ldvLoadNibNamed:@"Preferences" topLevelObjects:&topLevelObjects];
		[topLevelObjects retain];
	}
	return self;
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	TCObject::release(ldPreferences);
	[pages release];
	[self releaseTopLevelObjects:topLevelObjects orTopLevelObject:window];
	[super dealloc];
}

- (void)loadSettings
{
	ldPreferences->loadSettings();
	[pages makeObjectsPerformSelector:@selector(setup)];
}

- (void)openGLInitialized
{
	if (!openGLInitialized)
	{
		[primitivesPage setup];
		openGLInitialized = YES;
	}
}

- (void)show
{
	[window makeKeyAndOrderFront:self];
}

- (void)addPage:(id)page
{
	[pages addObject:page];
	if (![self haveTopLevelObjectsArray])
	{
		[page release];
	}
}

- (void)modelLoading:(NSNotification *)notification
{
	loading = YES;
	[applyButton setEnabled:NO];
	[okButton setEnabled:NO];
}

- (void)modelLoaded:(NSNotification *)notification
{
	loading = NO;
	[applyButton setEnabled:applyEnabled];
	[okButton setEnabled:YES];
}

- (void)awakeFromNib
{
	if ([[controller currentModelWindow] loading])
	{
		[self modelLoading:nil];
	}
	generalIndex = 0;
	ldrawIndex = 1;
	geometryIndex = 2;
	effectsIndex = 3;
	primitivesIndex = 4;
	updatesIndex = 5;
	prefSetsIndex = 6;
	[self addPage:generalPage];
	[self addPage:ldrawPage];
	[self addPage:geometryPage];
	[self addPage:effectsPage];
	[self addPage:primitivesPage];
	[self addPage:updatesPage];
	[self addPage:prefSetsPage];
	[pages makeObjectsPerformSelector:@selector(setPreferences:) withObject:self];
	[self loadSettings];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(modelLoading:) name:@"ModelLoading" object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(modelLoaded:) name:@"ModelLoaded" object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(modelLoaded:) name:@"ModelLoadCanceled" object:nil];
}

- (void)initModelWindow:(ModelWindow *)value
{
	ldPreferences->setModelViewer([[value modelView] modelViewer]);
	ldPreferences->applySettings();
}

- (void)modelWindowWillClose:(ModelWindow *)modelWindow
{
	if (ldPreferences->getModelViewer() == [[modelWindow modelView] modelViewer])
	{
		NSArray *modelWindows = [controller modelWindows];
		if ([modelWindows count] > 0)
		{
			LDrawModelView *modelView = [[modelWindows lastObject] modelView];
			ldPreferences->setModelViewer([modelView modelViewer]);
		}
		else
		{
			ldPreferences->setModelViewer(NULL);
		}
	}
}

- (LDPreferences *)ldPreferences
{
	return ldPreferences;
}

- (bool)applyFailed
{
	return applyFailedPage != nil;
}

- (void)setApplyFailed:(PreferencePage *)page
{
	applyFailedPage = page;
	if (page)
	{
		[tabView selectTabViewItemAtIndex:[pages indexOfObject:applyFailedPage]];
		[window makeKeyAndOrderFront:self];
	}
}

- (IBAction)apply:(id)sender
{
	applyFailedPage = nil;
	[pages makeObjectsPerformSelector:@selector(apply)];
	if (applyFailedPage == nil)
	{
		NSArray *modelWindows = [controller modelWindows];
		for (NSUInteger i = 0; i < [modelWindows count]; i++)
		{
			LDrawModelView *modelView = [[modelWindows objectAtIndex:i] modelView];
			ldPreferences->setModelViewer([modelView modelViewer]);
			ldPreferences->applySettings();
			if (i == 0)
			{
				ldPreferences->commitSettings();
			}
			[modelView setNeedsDisplay:YES];
		}
		[[NSNotificationCenter defaultCenter] postNotificationName:LDPreferencesDidUpdateNotification object:self];
	}
}

- (void)hotKeyPressed:(int)index
{
	[prefSetsPage hotKeyPressed:index];
}

- (IBAction)cancel:(id)sender
{
	[window orderOut:sender];
	[pages makeObjectsPerformSelector:@selector(setup)];
}

- (IBAction)ok:(id)sender
{
	[self apply:sender];
	if (applyFailedPage == nil)
	{
		[window orderOut:sender];
	}
}

- (void)enableApply:(BOOL)enabled
{
	applyEnabled = enabled;
	if (!loading || !enabled)
	{
		[applyButton setEnabled:enabled];
	}
	[window setDocumentEdited:enabled];
}

- (BOOL)isApplyEnabled
{
	return [applyButton isEnabled];
}

- (NSWindow *)window
{
	return window;
}

- (BOOL)tabView:(NSTabView *)aTabView shouldSelectTabViewItem:(NSTabViewItem *)tabViewItem
{
	return [[pages objectAtIndex:[aTabView indexOfTabViewItem:[tabView selectedTabViewItem]]] canSwitchPages];
}

- (bool)getBoolFrom:(id)sender
{
	if ([sender isKindOfClass:[NSSegmentedControl class]])
	{
		return [[sender cell] isSelectedForSegment:[sender selectedSegment]] ? true : false;
	}
	return false;
}

- (void)takeWireframeFrom:(id)sender
{
	ldPreferences->setDrawWireframe([self getBoolFrom:sender]);
	[[pages objectAtIndex:geometryIndex] setup];
	[self apply:sender];
}

- (void)takeSeamsFrom:(id)sender
{
	ldPreferences->setUseSeams([self getBoolFrom:sender]);
	[[pages objectAtIndex:geometryIndex] setup];
	[self apply:sender];
}

- (void)takeEdgesFrom:(id)sender
{
	ldPreferences->setShowHighlightLines([self getBoolFrom:sender]);
	[[pages objectAtIndex:geometryIndex] setup];
	[self apply:sender];
}

- (void)takePrimSubFrom:(id)sender
{
	ldPreferences->setAllowPrimitiveSubstitution([self getBoolFrom:sender]);
	[[pages objectAtIndex:primitivesIndex] setup];
	[self apply:sender];
}

- (void)takeLightingFrom:(id)sender
{
	ldPreferences->setUseLighting([self getBoolFrom:sender]);
	[[pages objectAtIndex:effectsIndex] setup];
	[self apply:sender];
}

- (void)takeBfcFrom:(id)sender
{
	ldPreferences->setBfc([self getBoolFrom:sender]);
	[[pages objectAtIndex:geometryIndex] setup];
	[self apply:sender];
}

- (void)takeAllConditionalsFrom:(id)sender
{
	ldPreferences->setShowAllConditionalLines([self getBoolFrom:sender]);
	[[pages objectAtIndex:geometryIndex] setup];
	[self apply:sender];
}

- (void)takeConditionalControlsFrom:(id)sender
{
	ldPreferences->setShowConditionalControlPoints([self getBoolFrom:sender]);
	[[pages objectAtIndex:geometryIndex] setup];
	[self apply:sender];
}

- (void)takeTexmapsFrom:(id)sender
{
	ldPreferences->setTexmaps([self getBoolFrom:sender]);
	[[pages objectAtIndex:primitivesIndex] setup];
	[self apply:sender];
}

- (void)takeTransDefaultFrom:(id)sender
{
	ldPreferences->setTransDefaultColor([self getBoolFrom:sender]);
	[[pages objectAtIndex:generalIndex] setup];
	[self apply:sender];
}

- (void)takePartBBoxesFrom:(id)sender
{
	ldPreferences->setBoundingBoxesOnly([self getBoolFrom:sender]);
	[[pages objectAtIndex:geometryIndex] setup];
	[self apply:sender];
}

- (void)takeWireframeCutawayFrom:(id)sender
{
	ldPreferences->setCutawayMode([self getBoolFrom:sender] ? LDVCutawayWireframe : LDVCutawayNormal);
	[[pages objectAtIndex:effectsIndex] setup];
	[self apply:sender];
}

- (void)takeFlatShadingFrom:(id)sender
{
	ldPreferences->setUseFlatShading([self getBoolFrom:sender]);
	[[pages objectAtIndex:effectsIndex] setup];
	[self apply:sender];
}

- (void)takeSmoothCurvesFrom:(id)sender
{
	ldPreferences->setPerformSmoothing([self getBoolFrom:sender]);
	[[pages objectAtIndex:effectsIndex] setup];
	[self apply:sender];
}

- (void)takeStudLogosFrom:(id)sender
{
	ldPreferences->setTextureStuds([self getBoolFrom:sender]);
	[[pages objectAtIndex:primitivesIndex] setup];
	[self apply:sender];
}

- (void)takeLowResStudsFrom:(id)sender
{
	ldPreferences->setQualityStuds(![self getBoolFrom:sender]);
	[[pages objectAtIndex:primitivesIndex] setup];
	[self apply:sender];
}

- (void)takeShowAxesFrom:(id)sender
{
	ldPreferences->setShowAxes([self getBoolFrom:sender]);
	[[pages objectAtIndex:generalIndex] setup];
	[self apply:sender];
}

- (void)takeRandomColorsFrom:(id)sender
{
	ldPreferences->setRandomColors([self getBoolFrom:sender]);
	[[pages objectAtIndex:generalIndex] setup];
	[self apply:sender];
}

- (void)saveViewingAngle:(ModelWindow*)modelWindow
{
	ldPreferences->setModelViewer([[modelWindow modelView] modelViewer]);
	ldPreferences->saveDefaultView();
}

- (void)lightVectorChanged:(TCAlert *)alert
{
	[effectsPage lightVectorChanged:alert];
}

- (LDrawPage *)ldrawPage
{
	return ldrawPage;
}

- (GeneralPage *)generalPage
{
	return generalPage;
}

@end
