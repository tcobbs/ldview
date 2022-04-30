//
//  OptionsUI.h
//  LDView
//
//  Created by Travis Cobbs on 6/15/08.
//  Copyright 2008 Travis Cobbs. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MacSetup.h"

@class Options;

class LDExporterSetting;

@interface OptionUI : NSObject
{
	Options *options;
	NSView *docView;
	LDExporterSetting *setting;
	bool shown;
	CGFloat leftGroupMargin;
    CGFloat rightGroupMargin;
    CGFloat bottomGroupMargin;
}

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting;
- (CGFloat)updateLayoutX:(CGFloat)x y:(CGFloat)y width:(CGFloat)width update:(bool)update optimalWidth:(CGFloat &)optimalWidth;
- (bool)validate:(NSString *&)error;
- (void)commit;
- (LDExporterSetting *)setting;
- (void)setEnabled:(BOOL)enabled;
- (BOOL)groupEnabled;
- (NSRect)frame;
- (CGFloat)leftGroupMargin;
- (CGFloat)rightGroupMargin;
- (CGFloat)bottomGroupMargin;
- (NSString *)wrappedStringForString:(NSString *)string width:(CGFloat &)width height:(CGFloat &)height font:(NSFont *)font;
- (NSRect)calcCheckLayout:(NSButton *)check inRect:(NSRect)bounds optimalWidth:(CGFloat &)optimalWidth;
- (NSRect)calcLabelLayout:(NSTextField *)textField inRect:(NSRect)bounds optimalWidth:(CGFloat &)optimalWidth;
- (NSRect)calcLabelLayout:(NSTextField *)textField inRect:(NSRect)bounds optimalWidth:(CGFloat &)optimalWidth delta:(CGFloat)delta;
- (NSButton *)newCheck;
- (NSButton *)newButton:(NSString *)title;
- (NSTextField *)newLabel;
- (NSView *)firstKeyView;
- (NSView *)lastKeyView;
- (void)addTooltip:(NSView *)view;
- (void)valueChanged;
- (void)reset;

@end
