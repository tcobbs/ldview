//
//  GenericSheet.h
//  LDView
//
//  Created by Travis Cobbs on 7/29/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface GenericSheet : NSObject
{
    IBOutlet NSPanel *panel;
	NSArray *topLevelObjects;
	NSWindow *sheetWindow;
}

- (id)initWithNibName:(NSString *)nibName;
- (void)beginSheetInWindow:(NSWindow *)window completionHandler:(void (^)(NSModalResponse returnCode))handler;
- (IBAction)cancel:(id)sender;
- (IBAction)ok:(id)sender;
- (void)finishInitWithNibName:(NSString *)nibName;
- (void)stopModalWithCode:(NSModalResponse)returnCode;
- (void)setField:(NSTextField *)textField toFloat:(double)value;

@end
