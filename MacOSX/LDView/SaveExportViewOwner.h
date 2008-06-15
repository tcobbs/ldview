//
//  SaveExportViewOwner.h
//  LDView
//
//  Created by Travis Cobbs on 06/14/08.
//  Copyright 2007 Travis Cobbs. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "FileTypeViewOwner.h"

#include <LDLib/LDrawModelViewer.h>

@interface SaveExportViewOwner : FileTypeViewOwner
{
	LDrawModelViewer *modelViewer;
	NSMutableDictionary *typeDict;
}

- (id)initWithModelViewer:(LDrawModelViewer *)aModelViewer;
- (LDrawModelViewer::ExportType)exportType;

@end
