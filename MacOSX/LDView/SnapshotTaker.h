//
//  SnapshotTaker.h
//  LDView
//
//  Created by Travis Cobbs on 10/14/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <TRE/TREGL.h>

#include <LDLib/LDSnapshotTaker.h>

class LDSnapshotTaker;
class LDrawModelViewer;
class ModelViewer;
class TCAlert;
class SnapshotAlertHandler;

@interface SnapshotTaker : NSObject {
	LDSnapshotTaker *ldSnapshotTaker;
START_IGNORE_DEPRECATION
	CGLPBufferObj pbuffer;
END_IGNORE_DEPRECATION
	CGLContextObj context;
	NSOpenGLContext *sharedContext;
	LDrawModelViewer *modelViewer;
	SnapshotAlertHandler *snapshotAlertHandler;
}

- (id)init;
- (id)initWithModelViewer:(LDrawModelViewer *)modelViewer sharedContext:(NSOpenGLContext *)sharedContext;

- (LDSnapshotTaker *)ldSnapshotTaker;
- (void)snapshotCallback:(TCAlert *)alert;
- (void)setImageType:(LDSnapshotTaker::ImageType)value;
- (void)setTrySaveAlpha:(bool)value;
- (void)setAutoCrop:(bool)value;
- (bool)saveFile;
- (bool)saveFile:(NSString *)filename width:(int)width height:(int)height zoomToFit:(bool)zoomToFit;
- (NSImage *)imageWithWidth:(int)width height:(int)height zoomToFit:(bool)zoomToFit;

- (BOOL)useFBO;

@end
