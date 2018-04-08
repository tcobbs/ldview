//
//  ViewOwnerBase.h
//  LDView
//
//  Created by Travis Cobbs on 10/27/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MacSetup.h"

@interface ViewOwnerBase : NSObject
{
	NSMutableDictionary *origColors;
	NSMutableDictionary *origBoxTitles;
	NSMutableDictionary *disabledBoxTitles;
	NSArray *topLevelObjects;
}

- (void)setCheck:(NSButton *)check value:(bool)value;
- (bool)getCheck:(NSButton *)check;
- (void)setColorWell:(NSColorWell *)colorWell r:(int)r g:(int)g b:(int)b;
- (void)getColorWell:(NSColorWell *)colorWell r:(int *)r g:(int *)g b:(int *)b a:(int *)a;
- (void)getColorWell:(NSColorWell *)colorWell r:(int *)r g:(int *)g b:(int *)b;
- (void)enableLabel:(NSTextField *)label value:(BOOL)enabled;
- (void)enableBoxTitle:(NSBox *)box value:(BOOL)enabled;
- (void)groupCheck:(id)sender name:(NSString *)groupName;
- (void)groupCheck:(id)sender name:(NSString *)groupName value:(bool)value;
- (void)groupCheck:(id)sender name:(NSString *)groupName init:(BOOL)init;
- (void)setUISection:(NSString *)sectionName enabled:(BOOL)enabled;
- (IBAction)valueChanged:(id)sender;

@end
