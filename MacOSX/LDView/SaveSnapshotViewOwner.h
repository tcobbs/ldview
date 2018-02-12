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
	IBOutlet NSButton *fileTypeOptionsButton;
	IBOutlet NSPopUpButton *fileTypePopUp;
	IBOutlet NSButton *saveSeriesCheck;
	IBOutlet NSBox *saveSeriesBox;
	IBOutlet NSTextField *digitsLabel;
	IBOutlet NSTextField *digitsField;
	IBOutlet NSStepper *digitsStepper;
	IBOutlet NSButton *transparentCheck;
	IBOutlet NSButton *autocropCheck;
	IBOutlet NSButton *matchWindowScaleCheck;
	IBOutlet NSButton *sizeCheck;
	IBOutlet NSButton *zoomToFitCheck;
	IBOutlet NSTextField *widthLabel;
	IBOutlet NSTextField *widthField;
	IBOutlet NSTextField *heightLabel;
	IBOutlet NSTextField *heightField;
	IBOutlet NSButton *allStepsCheck;
	IBOutlet NSTextField *suffixLabel;
	IBOutlet NSTextField *suffixField;
	IBOutlet NSButton *sameScaleCheck;

	NSTextField *nameFieldLabel;
	NSTextField *filenameField;

	NSSavePanel *savePanel;

	int numSteps;
	int saveDigits;
	NSString *stepSuffix;
	NSMutableArray *fileTypes;
	int firstType;
	int lastType;
}

- (id)init;
- (void)setSavePanel:(NSSavePanel *)aSavePanel;
- (void)saveSettings;
- (void)setNumSteps:(int)value;

- (IBAction)matchWindowScale:(id)sender;
- (IBAction)saveSeries:(id)sender;
- (IBAction)size:(id)sender;
- (IBAction)fileType:(id)sender;
- (IBAction)fileTypeOptions:(id)sender;

- (bool)transparentBackground;
- (bool)autocrop;
- (int)width:(int)refWidth;
- (int)height:(int)refHeight;
- (bool)zoomToFit;
- (bool)matchWindowScale;
- (LDSnapshotTaker::ImageType)imageType;
- (NSString *)requiredFileType;

@end
