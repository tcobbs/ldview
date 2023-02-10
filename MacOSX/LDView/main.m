//
//  main.m
//  LDView
//
//  Created by Travis Cobbs on 7/2/06.
//  Copyright __MyCompanyName__ 2006. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OCUserDefaults.h"
#import "OCLocalStrings.h"
#import "OCWebClientPlugin.h"
#import "CommandLineSnapshot.h"

int main(int argc, char *argv[])
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	BOOL done = NO;
//	int i;
//
//	for (i = 0; argv[i]; i++)
//	{
//		printf("arg%d: <<%s>>\n", i, argv[i]);
//	}
	registerWebClientPlugin();
	[OCUserDefaults setCommandLine:argv];
	[OCUserDefaults setAppName:@"LDView"];
	[OCUserDefaults initSession];
	[OCLocalStrings loadStringTable:[[NSBundle mainBundle] pathForResource:@"LDViewMessages" ofType:@"ini"] replace:true];
	[OCLocalStrings loadStringTable:[[NSBundle mainBundle] pathForResource:@"LDExportMessages" ofType:@"ini"] replace:false];
	done = [CommandLineSnapshot takeSnapshot];
	[pool release];
	if (done)
	{
		return 0;
	}
	return NSApplicationMain(argc,  (const char **) argv);
}
