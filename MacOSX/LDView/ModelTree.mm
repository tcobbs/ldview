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
#import "LDViewCategories.h"

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

- (void)updateStatusText
{
	int row = [outlineView selectedRow];
	const LDModelTree *itemModelTree = NULL;
	
	if (row != -1)
	{
		ModelTreeItem *item = [outlineView itemAtRow:row];
		
		itemModelTree = [item modelTree];
	}
	if (itemModelTree != NULL)
	{
		[statusTextField setStringValue:[NSString stringWithUCString:itemModelTree->getStatusText().c_str()]];
	}
	else
	{
		[statusTextField setStringValue:[OCLocalStrings get:@"NoSelection"]];
	}
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
		float rowWidth = [[child stringValue] sizeWithAttributes:[NSDictionary dictionaryWithObject:font forKey: NSFontAttributeName]].width + indent;
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

- (void)selectAndExpandPath:(std::string)path
{
	ModelTreeItem *item = rootModelTreeItem;

	while (path.size() > 0)
	{
		int lineNumber = atoi(&path[1]) - 1;

		item = [item childAtIndex:lineNumber];
		if (item != nil)
		{
			size_t index = path.find('/', 1);
			if (index < path.size())
			{
				path = path.substr(index);
				[outlineView expandItem:item];
			}
			else
			{
				[outlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:[outlineView rowForItem:item]] byExtendingSelection:TRUE];
				path = "";
			}
		}
	}
}

- (void)reloadOutlineView
{
	// The following is intentionally a copy.
	StringList paths = [[modelWindow modelView] modelViewer]->getHighlightPaths();
	
	[outlineView reloadData];
	[[outlineView outlineTableColumn] setWidth:100];
	[self resizeIfNeeded:rootModelTreeItem];
	[outlineView deselectAll:self];
	for (StringList::const_iterator it = paths.begin(); it != paths.end(); it++)
	{
		[self selectAndExpandPath:modelTree->adjustHighlightPath(*it)];
	}
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

- (long)colorFromR:(int)r g:(int)g b:(int)b
{
	return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

- (void)convertColor:(int)color toR:(int &)r g:(int &)g b:(int &)b
{
	r = (color >> 16) & 0xFF;
	g = (color >> 8) & 0xFF;
	b = color & 0xFF;
}

- (void)awakeFromNib
{
	float width = [OCUserDefaults floatForKey:[self widthKey] defaultValue:-1.0f sessionSpecific:NO];
	int r, g, b;
	long defHighlightColor = [self colorFromR:160 g:224 b:255];
	long highlightColor = TCUserDefaults::longForKey(MODEL_TREE_HIGHLIGHT_COLOR_KEY, defHighlightColor, false);
	LDrawModelViewer *modelViewer = [[modelWindow modelView] modelViewer];

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
	[highlightCheck setCheck:TCUserDefaults::boolForKey(MODEL_TREE_HIGHLIGHT_KEY, false, false)];
	[self convertColor:highlightColor toR:r g:g b:b];
	[highlightColorWell setR:r g:g b:b];
	if (modelViewer != NULL)
	{
		modelViewer->setHighlightColor(r, g, b);
	}
	[self updateStatusText];
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

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	return [[self modelTreeItem:item] numberOfChildren];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
	return [[self modelTreeItem:item] numberOfChildren] > 0;
}

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
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

- (void)addItem:(LDModelTree *)item toPaths:(NSMutableString *)paths
{
	[paths appendString:[NSString stringWithCString:item->getTreePath().c_str() encoding:NSASCIIStringEncoding]];
	[paths appendString:@"\n"];
}

- (void)outlineViewSelectionDidChange:(NSNotification *)notification
{
	[self updateHighlight];
	[self updateStatusText];
}

- (IBAction)highlightColor:(id)sender
{
	LDrawModelViewer *modelViewer = [[modelWindow modelView] modelViewer];
	
	if (modelViewer != NULL)
	{
		int r, g, b;
		
		[highlightColorWell getR:&r g:&g b:&b];
		modelViewer->setHighlightColor(r, g, b);
		TCUserDefaults::setLongForKey([self colorFromR:r g:g b:b], MODEL_TREE_HIGHLIGHT_COLOR_KEY, false);
	}
}

- (IBAction)highlight:(id)sender
{
	[self updateHighlight];
	TCUserDefaults::setBoolForKey([highlightCheck getCheck], MODEL_TREE_HIGHLIGHT_KEY, false);
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
	NSRect highlightEndFrame = [highlightCheck frame];
	NSRect statusTextEndFrame = [statusTextField frame];
	NSRect highlightColorEndFrame = [highlightColorWell frame];
	NSArray *viewDicts = [NSArray arrayWithObjects:[NSMutableDictionary dictionaryWithCapacity:2], [NSMutableDictionary dictionaryWithCapacity:2], [NSMutableDictionary dictionaryWithCapacity:3], [NSMutableDictionary dictionaryWithCapacity:3], [NSMutableDictionary dictionaryWithCapacity:2], [NSMutableDictionary dictionaryWithCapacity:2], [NSMutableDictionary dictionaryWithCapacity:2], nil];

	outlineEndFrame.size.height -= showHideStartY * dir;
	outlineEndFrame.origin.y += showHideStartY * dir;
	buttonEndFrame.origin.y += showHideStartY * dir;
	highlightEndFrame.origin.y += showHideStartY * dir;
	statusTextEndFrame.origin.y += showHideStartY * dir;
	highlightColorEndFrame.origin.y += showHideStartY * dir;
	boxLabelEndFrame.origin.y += showHideStartY * dir;
	boxEndFrame.origin.y += showHideStartY * dir;
	[self setupAnimationDict:[viewDicts objectAtIndex:0] view:scrollView endRect:outlineEndFrame showHide:0];
	[self setupAnimationDict:[viewDicts objectAtIndex:1] view:showHideOptionsButton endRect:buttonEndFrame showHide:0];
	[self setupAnimationDict:[viewDicts objectAtIndex:2] view:optionsBoxLabel endRect:boxLabelEndFrame showHide:0];
	[self setupAnimationDict:[viewDicts objectAtIndex:3] view:optionsBox endRect:boxEndFrame showHide:dir];
	[self setupAnimationDict:[viewDicts objectAtIndex:4] view:highlightCheck endRect:highlightEndFrame showHide:0];
	[self setupAnimationDict:[viewDicts objectAtIndex:5] view:statusTextField endRect:statusTextEndFrame showHide:0];
	[self setupAnimationDict:[viewDicts objectAtIndex:6] view:highlightColorWell endRect:highlightColorEndFrame showHide:0];
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

- (void)enumSelectedItemsWithSelector:(SEL)selector withObject:(id)object
{
	int count = [outlineView numberOfSelectedRows];
	NSIndexSet *selectedRowIndices = [outlineView selectedRowIndexes];
	unsigned int currentIndex = [selectedRowIndices firstIndex];
	
	for (int i = 0; i < count; i++)
	{
		ModelTreeItem *item = [outlineView itemAtRow:currentIndex];
		const LDModelTree *itemModelTree = [item modelTree];

		if (itemModelTree)
		{
			[self performSelector:selector withObject:(id)itemModelTree withObject:object];
		}
		currentIndex = [selectedRowIndices indexGreaterThanIndex:currentIndex];
	}
}

- (void)copyItem:(LDModelTree *)item toString:(NSMutableString *)copyText
{
	if (item->getLineType() != LDLLineTypeEmpty)
	{
		[copyText appendString:[NSString stringWithCString:item->getText().c_str() encoding:NSASCIIStringEncoding]];
	}
	[copyText appendString:@"\n"];
}

- (IBAction)copy:(id)sender
{
	if ([[modelWindow window] firstResponder] == outlineView)
	{
		NSMutableString *copyText = [[NSMutableString alloc] init];
		[self enumSelectedItemsWithSelector:@selector(copyItem:toString:) withObject:copyText];
		if ([copyText length] > 0)
		{
			[modelWindow copyStringToPasteboard:copyText];
		}
		[copyText release];
	}
}

- (void)updateHighlight
{
	LDrawModelViewer *modelViewer = [[modelWindow modelView] modelViewer];
	
	if ([highlightCheck getCheck])
	{
		NSMutableString *paths = [NSMutableString string];
		
		[self enumSelectedItemsWithSelector:@selector(addItem:toPaths:) withObject:paths];
		const char *cString = [[paths retain] cStringUsingEncoding:NSASCIIStringEncoding];
		modelViewer->setHighlightPaths(cString);
		[paths release];
	}
	else
	{
		modelViewer->setHighlightPaths("");
	}
}

@end
