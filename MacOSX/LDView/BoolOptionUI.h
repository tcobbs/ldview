//
//  BoolOptionUI.h
//  LDView
//
//  Created by Travis Cobbs on 6/15/08.
//  Copyright 2008 Travis Cobbs. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OptionUI.h"


@interface BoolOptionUI : OptionUI
{
	NSButton *check;
}

- (void)checked:(id)sender;
- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting;
- (CGFloat)updateLayoutX:(CGFloat)x y:(CGFloat)y width:(CGFloat)width update:(bool)update optimalWidth:(CGFloat &)optimalWidth;
- (void)commit;
- (void)setEnabled:(BOOL)enabled;
- (NSRect)frame;
- (NSView *)firstKeyView;
- (BOOL)groupEnabled;
- (void)valueChanged;

@end
