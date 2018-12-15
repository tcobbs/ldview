//
//  HelperPanel.mm
//  LDView
//
//  Created by Travis Cobbs on 5/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "HelperPanel.h"
#import "ModelWindow.h"
#import "LDrawModelView.h"
#import "LDViewCategories.h"
#import "OCUserDefaults.h"

#include <LDLib/LDModelTree.h>
#include <LDLib/LDrawModelViewer.h>
#include <LDLoader/LDLMainModel.h>


@implementation HelperPanel

- (id)initWithParent:(ModelWindow *)parent
{
	self = [super init];
	if (self != nil)
	{
		className = [NSStringFromClass([self class]) retain];
		modelWindow = parent;	// Don't retain; we're a child.
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(modelChanged:) name:@"ModelLoaded" object:modelWindow];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(modelChanged:) name:@"ModelLoadCanceled" object:modelWindow];
		[self ldvLoadNibNamed:className topLevelObjects:&topLevelObjects];
		[topLevelObjects retain];
	}
	return self;
}

- (void)dealloc
{
	[className release];
	TCObject::release(model);
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	if (![self releaseTopLevelObjects:topLevelObjects])
	{
        [panel release];
	}
	[super dealloc];
}

- (void)modelChanged:(NSNotification *)notification
{
	// Overridden by subclasses.
}

- (void)open
{
    [panel makeKeyAndOrderFront:nil];
}

- (void)close
{
    [panel orderOut:nil];
}

- (bool)isOpen
{
    return panel.visible;
}

- (void)toggle
{
	if ([self isOpen])
	{
		[self close];
	}
	else
	{
		[self open];
	}
}

@end
