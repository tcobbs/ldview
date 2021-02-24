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
#import "LDViewController.h"
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDrawModelViewer.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <LDLib/LDPreferences.h>
#include <TRE/TREMainModel.h>

@implementation CommandLineSnapshot

+ (BOOL)takeSnapshot
{
	BOOL retValue = NO;
	SnapshotTaker *snapshotTaker = [[SnapshotTaker alloc] init];

	TREMainModel::loadStudTexture([[[NSBundle mainBundle] pathForResource:@"StudLogo" ofType:@"png"] UTF8String]);
	LDrawModelViewer::setAppVersion([[LDViewController appVersion] UTF8String]);
	LDrawModelViewer::setAppCopyright([[LDViewController appCopyright] UTF8String]);
	if ([snapshotTaker saveFile])
	{
		retValue = YES;
	}
	[snapshotTaker release];
	[[NSUserDefaults standardUserDefaults] synchronize];
	return retValue;
}

@end
