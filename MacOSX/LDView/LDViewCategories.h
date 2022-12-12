//
//  LDViewCategories.h
//  LDView
//
//  Created by Travis Cobbs on 3/22/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include <TCFoundation/mystring.h>


@interface NSButton(LDView)

- (void)setCheck:(bool)value;
- (bool)getCheck;

@end

@interface NSColorWell(LDView)

- (void)setR:(int)r g:(int)g b:(int)b;
- (void)getR:(int *_Nonnull)r g:(int *_Nonnull)g b:(int *_Nonnull)b;
- (void)getR:(int *_Nonnull)r g:(int *_Nonnull)g b:(int *_Nonnull)b a:(int *_Nullable)a;

@end

@interface NSString(LDView)

+ (id _Nonnull)stringWithUCString:(const ucstring &)ucstring;
- (ucstring)ucString;
- (id _Nonnull)initWithUCString:(const ucstring &)ucstring;

@end

@interface NSOpenPanel(LDView)

- (NSString *_Nullable)ldvFilename;

@end

@interface NSSavePanel(LDView)

- (NSString *_Nullable)ldvFilename;

@end

@interface NSURL(LDView)

@property (readonly) const char * _Nonnull ldvFileSystemRepresentation NS_RETURNS_INNER_POINTER;

@end

@interface NSBundle(LDView)

- (BOOL)ldvLoadNibNamed:(NSNibName _Nonnull)nibName owner:(id _Nonnull)owner topLevelObjects:(NSArray * _Nullable* _Nullable)topLevelObjects;

@end

@interface NSObject(LDView)

- (BOOL)haveTopLevelObjectsArray;
- (BOOL)releaseTopLevelObjects:(NSArray * _Nullable)topLevelObjects;
- (void)releaseTopLevelObjects:(NSArray * _Nullable)topLevelObjects orTopLevelObject:(id _Nullable)topLevelObject;
- (BOOL)ldvLoadNibNamed:(NSNibName _Nonnull)nibName topLevelObjects:(NSArray * _Nullable* _Nullable)topLevelObjects;

@end

@interface NSWindow(LDView)

- (void)showAlertSheetWithTitle:(NSString * _Nonnull)title message:(NSString * _Nullable)message defaultButton:(NSString * _Nullable)defaultButton alternateButton:(NSString * _Nullable)alternateButton otherButton:(NSString * _Nullable)otherButton isCritical:(BOOL)isCritical completionHandler:(void (^ _Nullable)(NSModalResponse returnCode))handler;
- (void)showAlertSheetWithTitle:(NSString * _Nonnull)title message:(NSString * _Nullable)message defaultButton:(NSString * _Nullable)defaultButton alternateButton:(NSString * _Nullable)alternateButton otherButton:(NSString * _Nullable)otherButton completionHandler:(void (^ _Nullable)(NSModalResponse returnCode))handler;
- (void)showCriticalAlertSheetWithTitle:(NSString * _Nonnull)title message:(NSString * _Nullable)message defaultButton:(NSString * _Nullable)defaultButton alternateButton:(NSString * _Nullable)alternateButton otherButton:(NSString * _Nullable)otherButton completionHandler:(void (^ _Nullable)(NSModalResponse returnCode))handler;

@end

@interface NSAlert(LDView)

+ (instancetype _Nonnull)alertWithTitle:(NSString * _Nonnull)title message:(NSString * _Nullable)message defaultButton:(NSString * _Nullable)defaultButton alternateButton:(NSString * _Nullable)alternateButton otherButton:(NSString * _Nullable)otherButton isCritical:(BOOL)isCritical;
+ (NSModalResponse)runModalWithTitle:(NSString * _Nonnull)title message:(NSString * _Nullable)message defaultButton:(NSString * _Nullable)defaultButton alternateButton:(NSString * _Nullable)alternateButton otherButton:(NSString * _Nullable)otherButton isCritical:(BOOL)isCritical;
+ (NSModalResponse)runModalWithTitle:(NSString * _Nonnull)title message:(NSString * _Nullable)message defaultButton:(NSString * _Nullable)defaultButton alternateButton:(NSString * _Nullable)alternateButton otherButton:(NSString * _Nullable)otherButton;

@end
