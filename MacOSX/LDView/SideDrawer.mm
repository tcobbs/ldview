//
//  SideDrawer.mm
//  LDView
//
//  Created by Travis Cobbs on 5/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "SideDrawer.h"
#import "ModelWindow.h"
#import "LDrawModelView.h"
#import "LDViewCategories.h"
#import "OCUserDefaults.h"

#include <LDLib/LDModelTree.h>
#include <LDLib/LDrawModelViewer.h>
#include <LDLoader/LDLMainModel.h>


@implementation SideDrawer

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
		[drawer release];
		[contentView release];
	}
	[super dealloc];
}

- (void)modelChanged:(NSNotification *)notification
{
	// Overridden by subclasses.
}

- (BOOL)isSideDrawer:(NSDrawer *)aDrawer
{
	return [aDrawer edge] == NSMinXEdge || [aDrawer edge] == NSMaxXEdge;
}

- (BOOL)isOpenDrawer:(NSDrawer *)aDrawer
{
	return [aDrawer state] == NSDrawerOpeningState || [aDrawer state] == NSDrawerOpenState;
}

- (void)checkDrawer:(NSDrawer *)aDrawer leftWidth:(float &)leftDrawerWidth rightWidth:(float &)rightDrawerWidth
{
	if (aDrawer == drawer || ([self isSideDrawer:aDrawer] && [self isOpenDrawer:aDrawer]))
	{
		float drawerWidth = [aDrawer contentSize].width + [aDrawer leadingOffset] + [aDrawer trailingOffset];
		
		if ([aDrawer preferredEdge] == NSMinXEdge)
		{
			if (drawerWidth > leftDrawerWidth)
			{
				leftDrawerWidth = drawerWidth;
			}
		}
		else
		{
			if (drawerWidth > rightDrawerWidth)
			{
				rightDrawerWidth = drawerWidth;
			}
		}
	}
}

- (void)fixWindowSizeIfNecessary
{
	NSWindow *window = [modelWindow window];
	NSRect windowFrame = [window frame];
	NSSize windowSize = windowFrame.size;
	NSRect visibleFrame = [[window screen] visibleFrame];
	NSSize screenSize = visibleFrame.size;
	float leftDrawerWidth = 0.0f;
	float rightDrawerWidth = 0.0f;
	float drawersWidth = 0.0f;
	NSArray *drawers = [window drawers];
	int count = (int)[drawers count];
	int i;
	
	[self checkDrawer:drawer leftWidth:leftDrawerWidth rightWidth:rightDrawerWidth];;
	for (i = 0; i < count; i++)
	{
		[self checkDrawer:[drawers objectAtIndex:i] leftWidth:leftDrawerWidth rightWidth:rightDrawerWidth];;
		
	}
	drawersWidth = leftDrawerWidth + rightDrawerWidth;
	if (windowSize.width + drawersWidth > screenSize.width)
	{
		windowFrame.size.width = screenSize.width - drawersWidth;
		windowFrame.origin.x = visibleFrame.origin.x + leftDrawerWidth;
		[window setFrame:windowFrame display:YES];
	}
}

- (void)open
{
	[self fixWindowSizeIfNecessary];
	[drawer openOnEdge:[drawer preferredEdge]];
}

- (void)close
{
	[drawer close];
}

- (bool)isOpen
{
	return [self isOpenDrawer:drawer];
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

- (NSString *)widthKey
{
	return [NSString stringWithFormat:@"%@DrawerWidth", className];
}

// NSDrawer delegate methods

- (NSSize)drawerWillResizeContents:(NSDrawer *)sender toSize:(NSSize)contentSize
{
	[OCUserDefaults setFloat:contentSize.width forKey:[self widthKey] sessionSpecific:NO];
	return contentSize;
}

@end
