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

- (void)dealloc
{
	[rootModelTreeItem release];
	TCObject::release(modelTree);
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
	float width = [OCUserDefaults floatForKey:[self widthKey] defaultValue:-1.0f sessionSpecific:NO];

	[outlineView setIntercellSpacing:NSMakeSize(0.0f, 0.0f)];
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

// NSOutlineView delegate methods

- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	const LDModelTree *itemTree = [item modelTree];
	
	if (itemTree != NULL)
	{
		TCFloat r, g, b;
		
		if (itemTree->getBackgroundRGB(r, g, b))
		{
			[cell setBackgroundColor:[NSColor colorWithCalibratedRed:r green:g blue:b alpha:1.0f]];
			[cell setDrawsBackground:YES];
		}
		else
		{
			[cell setDrawsBackground:NO];
		}
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

- (NSOutlineView *)outlineView
{
	return outlineView;
}

- (BOOL)canCopy
{
	if ([[modelWindow window] firstResponder] == outlineView)
	{
		if ([outlineView numberOfSelectedRows] > 0)
		{
			return YES;
		}
	}
	return NO;
}

- (IBAction)copy:(id)sender
{
	if ([[modelWindow window] firstResponder] == outlineView)
	{
		int count = [outlineView numberOfSelectedRows];
		NSIndexSet *selectedRowIndices = [outlineView selectedRowIndexes];
		NSMutableString *copyText = [[NSMutableString alloc] init];
		NSTableColumn *column = [[outlineView tableColumns] objectAtIndex:0];
		unsigned int currentIndex = [selectedRowIndices firstIndex];

		for (int i = 0; i < count; i++)
		{
			ModelTreeItem *item = [outlineView itemAtRow:currentIndex];
			NSString *rowValue = [self outlineView:outlineView objectValueForTableColumn:column byItem:item];
			const LDModelTree *itemModelTree = [item modelTree];

			if (itemModelTree && itemModelTree->getLineType() != LDLLineTypeEmpty)
			{
				[copyText appendString:rowValue];
			}
			[copyText appendString:@"\n"];
			currentIndex = [selectedRowIndices indexGreaterThanIndex:currentIndex];
		}
		[modelWindow copyStringToPasteboard:copyText];
		[copyText release];
	}
}

@end
