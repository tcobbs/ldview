//
//  NumberOptionUI.h
//  LDView
//
//  Created by Travis Cobbs on 6/17/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OptionUI.h"


@interface NumberOptionUI : OptionUI
{
	NSTextField *label;
	NSTextField *textField;
	CGFloat textFieldWidth;
}

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting;
- (CGFloat)updateLayoutX:(CGFloat)x y:(CGFloat)y width:(CGFloat)width update:(bool)update optimalWidth:(CGFloat &)optimalWidth;
- (void)setEnabled:(BOOL)enabled;
- (NSRect)frame;
- (NSView *)firstKeyView;

@end
