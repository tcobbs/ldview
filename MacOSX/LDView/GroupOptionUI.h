//
//  GroupOptionUI.h
//  LDView
//
//  Created by Travis Cobbs on 6/15/08.
//  Copyright 2008 Travis Cobbs. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OptionUI.h"


@interface GroupOptionUI : OptionUI
{
	NSBox *box;
	NSButton *check;
	NSTextField *label;
	NSButton *resetButton;
	CGFloat spacing;
}

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting spacing:(CGFloat)theSpacing;
- (CGFloat)updateLayoutX:(CGFloat)x y:(CGFloat)y width:(CGFloat)width update:(bool)update optimalWidth:(CGFloat &)optimalWidth;
- (void)commit;
- (void)setEnabled:(BOOL)enabled;
- (NSRect)frame;
- (void)closeGroupAtY:(CGFloat)y;
- (CGFloat)bottomGroupMargin;

- (BOOL)groupEnabled;
- (NSView *)firstKeyView;

@end
