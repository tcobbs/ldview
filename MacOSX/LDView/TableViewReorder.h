//
//  TableViewReorder.h
//  LDView
//
//  Created by Travis Cobbs on 3/23/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class TableViewReorder;

@protocol TableViewReorder

- (NSMutableArray *)tableRows:(TableViewReorder *)sender;

@end

@protocol TableViewReorderMessages

- (void)tableViewReorderDidOccur:(id)sender;

@end

#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1060
@interface TableViewReorder : NSObject < NSTableViewDataSource >
#else
@interface TableViewReorder : NSObject
#endif
{
	NSTableView *tableView;
	id<TableViewReorder> owner;
	NSString *dragType;
	id tableViewDataSource;
}

- (id)initWithTableView:(NSTableView *)theTableView owner:(id<TableViewReorder>)theOwner dragType:(NSString *)theDragType;

@end
