//
//  SaveSnapshotViewOwner.h
//  LDView
//
//  Created by Travis Cobbs on 10/27/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "ViewOwnerBase.h"
#include <LDLib/LDSnapshotTaker.h>

@interface SaveSnapshotViewOwner : ViewOwnerBase
{
	IBOutlet NSView *accessoryView;
	IBOutlet NSPopUpButton *fileTypePopUp;
	IBOutlet NSButton *saveSeriesCheck;
	IBOutlet NSTextField *digitsLabel;
	IBOutlet NSTextField *digitsField;
	IBOutlet NSStepper *digitsStepper;
	IBOutlet NSButton *transparentCheck;
	IBOutlet NSButton *autocropCheck;
	IBOutlet NSButton *sizeCheck;
	IBOutlet NSForm *sizeForm;
	IBOutlet NSButton *zoomToFitCheck;
	
	NSSavePanel *savePanel;
}

- (id)init;
- (void)setSavePanel:(NSSavePanel *)aSavePanel;
- (void)saveSettings;

- (IBAction)saveSeries:(id)sender;
- (IBAction)size:(id)sender;
- (IBAction)fileType:(id)sender;

- (bool)transparentBackground;
- (bool)autocrop;
- (int)width:(int)refWidth;
- (int)height:(int)refHeight;
- (bool)zoomToFit;
- (LDSnapshotTaker::ImageType)imageType;
- (NSString *)requiredFileType;

@end
