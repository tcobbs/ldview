//
//  CommandLineSnapshot.mm
//  LDView
//
//  Created by Travis Cobbs on 10/21/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "CommandLineSnapshot.h"
#import "OCUserDefaults.h"
#import "SnapshotTaker.h"
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDrawModelViewer.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <LDLib/LDPreferences.h>
#include <TRE/TREMainModel.h>

@implementation CommandLineSnapshot

+ (BOOL)takeSnapshot
{
	TCStringArray *unhandledArgs = TCUserDefaults::getUnhandledCommandLineArgs();
	
	if (unhandledArgs)
	{
		NSString *snapshotFilename = [OCUserDefaults stringForKey:@"SaveSnapshot" defaultValue:nil sessionSpecific:NO];
		BOOL retValue = NO;
		
		if (snapshotFilename)
		{
			int width = TCUserDefaults::longForKey(SAVE_WIDTH_KEY, 640, false);
			int height = TCUserDefaults::longForKey(SAVE_HEIGHT_KEY, 480, false);
			bool zoomToFit = TCUserDefaults::boolForKey(SAVE_ZOOM_TO_FIT_KEY, true, false);
			bool saveAlpha = TCUserDefaults::boolForKey(SAVE_ALPHA_KEY, false, false);
			LDrawModelViewer *modelViewer = new LDrawModelViewer(width, height);
			SnapshotTaker *snapshotTaker = [[SnapshotTaker alloc] initWithModelViewer:modelViewer];
			LDPreferences *prefs = new LDPreferences(modelViewer);

			TREMainModel::loadStudTexture([[[NSBundle mainBundle] pathForResource:@"StudLogo" ofType:@"png"] cStringUsingEncoding:NSASCIIStringEncoding]);
			prefs->loadSettings();
			prefs->applySettings();
			modelViewer->setFilename(unhandledArgs->stringAtIndex(0));
			[snapshotTaker setTrySaveAlpha:saveAlpha];
			if ([snapshotTaker saveFile:snapshotFilename width:width height:height zoomToFit:zoomToFit])
			{
				retValue = YES;
			}
			[snapshotTaker release];
		}
		unhandledArgs->release();
		return retValue;
	}
	return NO;
}

@end
