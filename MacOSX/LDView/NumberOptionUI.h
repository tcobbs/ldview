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
}

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting;
- (float)updateLayoutX:(float)x y:(float)y width:(float)width update:(bool)update optimalWidth:(float &)optimalWidth;
- (void)commit;
- (void)setEnabled:(BOOL)enabled;
- (NSRect)frame;
- (NSView *)firstKeyView;

@end
