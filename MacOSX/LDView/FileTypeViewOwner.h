//
//  FileTypeViewOwner.h
//  LDView
//
//  Created by Travis Cobbs on 10/27/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "ViewOwnerBase.h"
#include <LDLib/LDSnapshotTaker.h>

@interface FileTypeViewOwner : ViewOwnerBase
{
	IBOutlet NSView *accessoryView;
	IBOutlet NSButton *fileTypeOptionsButton;
	IBOutlet NSPopUpButton *fileTypePopUp;

	NSTextField *nameFieldLabel;
	NSTextField *filenameField;

	NSSavePanel *savePanel;

	NSMutableArray *fileTypes;
	NSMutableArray *extensions;
	NSString *udTypeKey;
}

- (id)init;
- (void)setSavePanel:(NSSavePanel *)aSavePanel;
- (void)saveSettings;

- (IBAction)fileType:(id)sender;
- (IBAction)fileTypeOptions:(id)sender;

- (NSString *)requiredFileType;

- (BOOL)haveOptions;

@end
