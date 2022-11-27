//
//  MPD.h
//  LDView
//
//  Created by Travis Cobbs on 5/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "HelperPanel.h"


@interface MPD : HelperPanel
{
	IBOutlet NSTableView *tableView;

	NSMutableArray *modelNames;
}

- (void)modelChanged:(NSNotification *)notification;
- (BOOL)isKeyWindow;

@end
