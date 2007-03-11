//
//  main.m
//  LDView
//
//  Created by Travis Cobbs on 7/2/06.
//  Copyright __MyCompanyName__ 2006. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OCUserDefaults.h"

int main(int argc, char *argv[])
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	[OCUserDefaults setAppName:@"LDView"];
	[pool release];
    return NSApplicationMain(argc,  (const char **) argv);
}
