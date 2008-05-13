//
//  MPD.h
//  LDView
//
//  Created by Travis Cobbs on 5/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "SideDrawer.h"


@interface MPD : SideDrawer
{
    IBOutlet NSTableView *tableView;

	NSMutableArray *modelNames;
}

- (void)modelChanged:(NSNotification *)notification;

@end
