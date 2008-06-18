//
//  StringOptionUI.h
//  LDView
//
//  Created by Travis Cobbs on 6/16/08.
//  Copyright 2008 Travis Cobbs. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OptionUI.h"


@interface StringOptionUI : OptionUI
{
	NSTextField *label;
	NSTextField *textField;
}

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting;
- (float)updateLayoutX:(float)x y:(float)y width:(float)width update:(bool)update optimalWidth:(float &)optimalWidth;
- (void)commit;
- (void)setEnabled:(BOOL)enabled;
- (NSRect)frame;
- (float)textFieldWidthForWidth:(float)width;
- (float)row2Height;
- (NSView *)firstKeyView;

@end
