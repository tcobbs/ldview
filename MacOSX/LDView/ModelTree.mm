//
//  ModelTree.mm
//  LDView
//
//  Created by Travis Cobbs on 3/17/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "ModelTree.h"
#import "ModelWindow.h"
#import "LDrawModelView.h"
#import "ModelTreeItem.h"
#import "OCUserDefaults.h"
#import "OCLocalStrings.h"

#include <LDLib/LDModelTree.h>
#include <LDLib/LDrawModelViewer.h>
#include <LDLoader/LDLMainModel.h>
#include <LDLoader/LDLFileLine.h>
#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDUserDefaultsKeys.h>

@implementation ModelTree

- (id) initWithParent:(ModelWindow *)parent
{
	self = [super init];
	if (self != nil)
	{
		modelWindow = parent;	// Don't retain; we're a child.
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(modelChanged:) name:@"ModelLoaded" object:modelWindow];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(modelChanged:) name:@"ModelLoadCanceled" object:modelWindow];
		[NSBundle loadNibNamed:@"ModelTree.nib" owner:self];
	}
	return self;
}

- (void)dealloc
{
	[drawer release];
	[contentView release];
	[rootModelTreeItem release];
	TCObject::release(modelTree);
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[super dealloc];
}

- (void)setModel:(LDLMainModel *)value
{
	if (model != value)
	{
		TCObject::release(model);
		TCObject::release(modelTree);
		[rootModelTreeItem release];
		model = value;
		if (model)
		{
			model->retain();
			modelTree = new LDModelTree(model);
			rootModelTreeItem = [[ModelTreeItem alloc] initWithModelTree:modelTree];
		}
		else
		{
			modelTree = NULL;
			rootModelTreeItem = nil;
		}
	}
}

- (void)resizeIfNeeded:(ModelTreeItem *)item
{
	int count = [item numberOfChildren];
	NSTableColumn *column = [outlineView outlineTableColumn];
	NSFont *font = [[column dataCell] font];
	float width = [column width];
	bool widthChanged = false;
	float indent = [outlineView indentationPerLevel] * [outlineView levelForItem:item] + 24.0f;
	
	for (int i = 0; i < count; i++)
	{
		ModelTreeItem *child = [item childAtIndex:i];
		float rowWidth = [font widthOfString:[child stringValue]] + indent;
		if (rowWidth > width)
		{
			width = rowWidth;
			widthChanged = true;
		}
	}
	if (widthChanged)
	{
		[column setWidth:width];
	}
}

- (void)reloadOutlineView
{
	[outlineView reloadData];
	[[outlineView outlineTableColumn] setWidth:100];
	[self resizeIfNeeded:rootModelTreeItem];
}

- (void)modelChanged
{
	[self setModel:[[modelWindow modelView] modelViewer]->getMainModel()];
	for (int i = LDLLineTypeComment; i <= LDLLineTypeUnknown; i++)
	{
		int row = i % 4;
		int col = i / 4;
		
		if (modelTree->getShowLineType((LDLLineType)i))
		{
			[optionsMatrix setState:NSOnState atRow:row column:col];
		}
		else
		{
			[optionsMatrix setState:NSOffState atRow:row column:col];
		}
	}
	[self reloadOutlineView];
}

- (void)awakeFromNib
{
	float width = [OCUserDefaults floatForKey:@"ModelTreeDrawerWidth" defaultValue:-1.0f sessionSpecific:NO];

	showHideStartY = [showHideOptionsButton frame].origin.y;
	[drawer setParentWindow:[modelWindow window]];
	if (width != -1.0f)
	{
		[drawer setContentSize:NSMakeSize(width, [drawer contentSize].height)];
	}
	[self modelChanged];
	if (!TCUserDefaults::boolForKey(MODEL_TREE_OPTIONS_SHOWN_KEY, true, false))
	{
		optionsShown = YES;
		[self hideOptionsInstantly:YES];
	}
	else
	{
		optionsShown = NO;
		[showHideOptionsButton setState:NSOnState];
		//[showHideOptionsButton setToolTip:[OCLocalStrings get:@"HideOptions"]];
	}
}

- (void)fixWindowSizeIfNecessary
{
	NSWindow *window = [modelWindow window];
	NSRect windowFrame = [window frame];
	NSSize windowSize = windowFrame.size;
	NSRect visibleFrame = [[window screen] visibleFrame];
	NSSize screenSize = visibleFrame.size;
	float drawerWidth = [drawer contentSize].width + [drawer leadingOffset] + [drawer trailingOffset];
	
	if (windowSize.width + drawerWidth > screenSize.width)
	{
		windowFrame.size.width = screenSize.width - drawerWidth;
		windowFrame.origin.x = visibleFrame.origin.x;
		[window setFrame:windowFrame display:YES];
	}
}

- (void)open
{
	[self fixWindowSizeIfNecessary];
	[drawer open];
}

- (void)close
{
	[drawer close];
}

- (bool)isOpen
{
	switch ([drawer state])
	{
		case NSDrawerOpenState:
		case NSDrawerOpeningState:
			return true;
		default:
			return false;
	}
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

- (ModelTreeItem *)modelTreeItem:(id)item
{
	if (item == nil)
	{
		return rootModelTreeItem;
	}
	else
	{
		return item;
	}
}

// NSOutlineView Data Source methods

- (int)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	return [[self modelTreeItem:item] numberOfChildren];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
	return [[self modelTreeItem:item] numberOfChildren] > 0;
}

- (id)outlineView:(NSOutlineView *)outlineView child:(int)index ofItem:(id)item
{
	return [[self modelTreeItem:item] childAtIndex:index];
}

- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
	return [[self modelTreeItem:item] objectValue];
}


- (void)modelChanged:(NSNotification *)notification
{
	if ([[notification name] isEqualToString:@"ModelLoaded"])
	{
		[self modelChanged];
	}
	else if ([[notification name] isEqualToString:@"ModelLoadCanceled"])
	{
		[self modelChanged];
	}
}
- (void)outlineViewItemDidExpand:(NSNotification *)notification
{
	[self resizeIfNeeded:[[notification userInfo] objectForKey:@"NSObject"]];
}

// NSDrawer delegate methods

- (NSSize)drawerWillResizeContents:(NSDrawer *)sender toSize:(NSSize)contentSize
{
	[OCUserDefaults setFloat:contentSize.width forKey:@"ModelTreeDrawerWidth" sessionSpecific:NO];
	return contentSize;
}

- (void)setupAnimationDict:(NSMutableDictionary *)dict view:(NSView *)view endRect:(NSRect)endRect showHide:(float)dir
{
	[dict setObject:view forKey:NSViewAnimationTargetKey];
	[dict setObject:[NSValue valueWithRect:endRect] forKey:NSViewAnimationEndFrameKey];
	if (dir != 0.0f)
	{
		NSString *effect;
		
		if (dir > 0.0f)
		{
			effect = NSViewAnimationFadeInEffect;
		}
		else
		{
			effect = NSViewAnimationFadeOutEffect;
		}
		[dict setObject:effect forKey:NSViewAnimationEffectKey];
	}
}

- (void)doOptionsAnimationInDir:(float)dir instantly:(BOOL)instantly
{
	NSScrollView *scrollView = [outlineView enclosingScrollView];
	NSRect outlineEndFrame = [scrollView frame];
	NSRect buttonEndFrame = [showHideOptionsButton frame];
	NSRect boxLabelEndFrame = [optionsBoxLabel frame];
	NSRect boxEndFrame = [optionsBox frame];
	NSArray *viewDicts = [NSArray arrayWithObjects:[NSMutableDictionary dictionaryWithCapacity:2], [NSMutableDictionary dictionaryWithCapacity:2], [NSMutableDictionary dictionaryWithCapacity:3], [NSMutableDictionary dictionaryWithCapacity:3], nil];

	outlineEndFrame.size.height -= showHideStartY * dir;
	outlineEndFrame.origin.y += showHideStartY * dir;
	buttonEndFrame.origin.y += showHideStartY * dir;
	boxLabelEndFrame.origin.y += showHideStartY * dir;
	boxEndFrame.origin.y += showHideStartY * dir;
	[self setupAnimationDict:[viewDicts objectAtIndex:0] view:scrollView endRect:outlineEndFrame showHide:0];
	[self setupAnimationDict:[viewDicts objectAtIndex:1] view:showHideOptionsButton endRect:buttonEndFrame showHide:0];
	[self setupAnimationDict:[viewDicts objectAtIndex:2] view:optionsBoxLabel endRect:boxLabelEndFrame showHide:0];
	[self setupAnimationDict:[viewDicts objectAtIndex:3] view:optionsBox endRect:boxEndFrame showHide:dir];
	optionsAnimation = [[NSViewAnimation alloc] initWithViewAnimations:viewDicts];
	if (instantly)
	{
		[optionsAnimation setDuration:0.0f];
	}
	[optionsAnimation setDelegate:self];
	[optionsAnimation startAnimation];
}

- (void)doOptionsAnimationInDir:(float)dir
{
	[self doOptionsAnimationInDir:dir instantly:NO];
}

- (void)animationDidEnd:(NSAnimation*)animation
{
	if (animation == optionsAnimation)
	{
		[optionsAnimation release];
		optionsAnimation = nil;
		[contentView setNeedsDisplay:YES];
	}
}

- (void)hideOptionsInstantly:(BOOL)instantly
{
	optionsShown = NO;
	[self doOptionsAnimationInDir:-1.0f instantly:instantly];
	TCUserDefaults::setBoolForKey(false, MODEL_TREE_OPTIONS_SHOWN_KEY, false);
}

- (void)hideOptions
{
	[self hideOptionsInstantly:NO];
}

- (void)showOptions
{
	optionsShown = YES;
	[self doOptionsAnimationInDir:1.0f];
	TCUserDefaults::setBoolForKey(true, MODEL_TREE_OPTIONS_SHOWN_KEY, false);
}

- (IBAction)showHideOptions:(id)sender
{
	if ([sender state] == NSOffState)
	{
		[self hideOptions];
	}
	else
	{
		[self showOptions];
	}
}

- (IBAction)optionChanged:(id)sender
{
	if (modelTree)
	{
		LDLLineType lineType = (LDLLineType)[[optionsMatrix selectedCell] tag];
		bool checked = [[optionsMatrix selectedCell] state] == NSOnState;
		
		modelTree->setShowLineType(lineType, checked);
		[rootModelTreeItem release];
		rootModelTreeItem = [[ModelTreeItem alloc] initWithModelTree:modelTree];
		[self reloadOutlineView];
	}
}

@end
