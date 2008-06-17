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
}

- (id)initWithOptions:(Options *)theOptions setting:(LDExporterSetting &)theSetting;
- (float)updateLayoutX:(float)x y:(float)y width:(float)width update:(bool)update optimalWidth:(float &)optimalWidth;
- (void)commit;
- (void)setEnabled:(BOOL)enabled;
- (NSRect)frame;
- (void)closeGroupAtY:(float)y;

- (BOOL)groupEnabled;

@end
