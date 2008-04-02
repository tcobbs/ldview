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
}

- (id)initWithNibName:(NSString *)nibName;
- (int)runSheetInWindow:(NSWindow *)window;
- (IBAction)cancel:(id)sender;
- (IBAction)ok:(id)sender;

@end
