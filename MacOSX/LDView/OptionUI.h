//
//  OptionsUI.h
//  LDView
//
//  Created by Travis Cobbs on 6/15/08.
//  Copyright 2008 Travis Cobbs. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class Options;

class LDExporterSetting;

@interface OptionUI : NSObject
{
	Options *options;
	NSView *docView;
	LDExporterSetting *setting;
	bool shown;
	int leftGroupMargin;
	int rightGroupMargin;
	int bottomGroupMargin;
}

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting;
- (float)updateLayoutX:(float)x y:(float)y width:(float)width update:(bool)update optimalWidth:(float &)optimalWidth;
- (bool)validate:(NSString *&)error;
- (void)commit;
- (LDExporterSetting *)setting;
- (void)setEnabled:(BOOL)enabled;
- (BOOL)groupEnabled;
- (NSRect)frame;
- (int)leftGroupMargin;
- (int)rightGroupMargin;
- (int)bottomGroupMargin;
- (NSString *)wrappedStringForString:(NSString *)string width:(float &)width height:(float &)height font:(NSFont *)font;
- (NSRect)calcCheckLayout:(NSButton *)check inRect:(NSRect)bounds optimalWidth:(float &)optimalWidth;
- (NSRect)calcLabelLayout:(NSTextField *)textField inRect:(NSRect)bounds optimalWidth:(float &)optimalWidth;
- (NSButton *)createCheck;
- (NSTextField *)createLabel;
- (NSView *)firstKeyView;
- (NSView *)lastKeyView;

@end
