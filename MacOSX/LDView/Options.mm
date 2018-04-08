#import "Options.h"
#import "BoolOptionUI.h"
#import "GroupOptionUI.h"
#import "EnumOptionUI.h"
#import "StringOptionUI.h"
#import "PathOptionUI.h"
#import "LongOptionUI.h"
#import "FloatOptionUI.h"
#import "OCLocalStrings.h"
#import "OptionsPanel.h"
#import "LDViewCategories.h"

@implementation Options

- (id) init
{
	self = [super init];
	if (self != nil)
	{
		margin = 6.0f;
		spacing = 6.0f;
		[self ldvLoadNibNamed:@"Options" topLevelObjects:&topLevelObjects];
		[topLevelObjects retain];
	}
	return self;
}

- (void)dealloc
{
	[optionUIs release];
	[self releaseTopLevelObjects:topLevelObjects orTopLevelObject:panel];
	[super dealloc];
}

- (void)calcOptionHeightWithEnum:(NSEnumerator *)enumerator optionUI:(OptionUI *)optionUI y:(CGFloat &)y width:(CGFloat)width leftMargin:(CGFloat)leftMargin rightMargin:(CGFloat)rightMargin numberWidth:(CGFloat)numberWidth optimalWidth:(CGFloat &)optimalWidth update:(bool)update enabled:(BOOL)enabled
{
	LDExporterSetting *setting = [optionUI setting];
	LDExporterSetting::Type type = setting->getType();
	CGFloat otherWidth = 0.0f;
	
	if (type == LDExporterSetting::TLong || type == LDExporterSetting::TFloat)
	{
		y += [optionUI updateLayoutX:margin + leftMargin y:y width:numberWidth - leftMargin - rightMargin update:update optimalWidth:optimalWidth] + spacing;
	}
	else
	{
		y += [optionUI updateLayoutX:margin + leftMargin y:y width:width - leftMargin - rightMargin update:update optimalWidth:otherWidth] + spacing;
	}
	[optionUI setEnabled:enabled];
	if (setting->getGroupSize() > 0)
	{
		CGFloat groupLeftMargin = [optionUI leftGroupMargin];
		CGFloat groupRightMargin = [optionUI rightGroupMargin];
		int i;
		
		enabled = [optionUI groupEnabled];
		leftMargin += groupLeftMargin;
		rightMargin += groupRightMargin;
		optimalWidth -= groupLeftMargin + groupRightMargin;
		for (i = 0; i < setting->getGroupSize(); i++)
		{
			OptionUI *newOptionUI = [enumerator nextObject];

			if (!newOptionUI)
			{
				// This is an error condition, but don't crash.
				break;
			}
			[self calcOptionHeightWithEnum:enumerator optionUI:newOptionUI y:y width:width leftMargin:leftMargin rightMargin:rightMargin numberWidth:numberWidth optimalWidth:optimalWidth update:update enabled:enabled];
		}
		if ([optionUI bottomGroupMargin] > 0.0f)
		{
			if (update)
			{
				[(GroupOptionUI *)optionUI closeGroupAtY:y - spacing];
			}
			y += [optionUI bottomGroupMargin];
		}
		optimalWidth += groupLeftMargin + groupRightMargin;
	}
}

- (CGFloat)calcHeightForWidth:(CGFloat)width optimalWidth:(CGFloat &)optimalWidth update:(bool)update
{
	CGFloat y = margin;
	CGFloat numberWidth;
	NSEnumerator *enumerator = [optionUIs objectEnumerator];
	OptionUI *optionUI;

	width -= margin * 2.0f;
	if (update)
	{
		numberWidth = optimalWidth;
	}
	else
	{
		numberWidth = width;
	}
	while ((optionUI = [enumerator nextObject]) != nil)
	{
		[self calcOptionHeightWithEnum:enumerator optionUI:optionUI y:y width:width leftMargin:0 rightMargin:0 numberWidth:numberWidth optimalWidth:optimalWidth update:update enabled:YES];
	}
	return y;
}

- (void)calcSize
{
	NSSize size;
	CGFloat width;
	CGFloat optimalWidth = 0.0f;
	CGFloat height;
	bool scrollNeeded;

	[scrollView setHasVerticalScroller:NO];
	size = [scrollView contentSize];
	width = size.width;
	height = [self calcHeightForWidth:width optimalWidth:optimalWidth update:false];
	scrollNeeded = height > size.height;
	size.height = height;
	[docView setFrameSize:size];
	if (scrollNeeded)
	{
		[scrollView setHasVerticalScroller:YES];
		size = [scrollView contentSize];
		width = size.width;
		optimalWidth = 0;
		height = [self calcHeightForWidth:width optimalWidth:optimalWidth update:false];
		size.height = height;
		[docView setFrameSize:size];
	}
	[self calcHeightForWidth:width optimalWidth:optimalWidth update:true];
	[scrollView setNeedsDisplay:YES];
}

- (void)awakeFromNib
{
}

- (IBAction)ok:(id)sender
{
	NSString *error = nil;
	NSEnumerator *enumerator = [optionUIs objectEnumerator];
	OptionUI *optionUI;

	// First, walk through all settings and validate them.  If any of the
	// validations fails, stop and return false.  That means that if there are
	// any validation failures, the settings before the failure won't have their
	// values updated.
	while ((optionUI = [enumerator nextObject]) != nil)
	{
		if (![optionUI validate:error])
		{
			if (error != nil)
			{
				NSRunAlertPanel([OCLocalStrings get:@"Error"], error, [OCLocalStrings get:@"OK"], nil, nil);
			}
			return;
		}
	}
	enumerator = [optionUIs objectEnumerator];
	// If we get here, validation succeeded, so save all the option values.
	while ((optionUI = [enumerator nextObject]) != nil)
	{
		[optionUI commit];
	}
	[NSApp stopModalWithCode:NSModalResponseOK];
}

- (IBAction)cancel:(id)sender
{
	[NSApp stopModalWithCode:NSModalResponseCancel];
}

- (void)addGroup:(LDExporterSetting &)setting
{
	[optionUIs addObject:[[[GroupOptionUI alloc] initWithOptions:self setting:setting spacing:spacing] autorelease]];
}

- (void)addBoolSetting:(LDExporterSetting &)setting
{
	[optionUIs addObject:[[[BoolOptionUI alloc] initWithOptions:self setting:setting] autorelease]];
}

- (void)addFloatSetting:(LDExporterSetting &)setting
{
	[optionUIs addObject:[[[FloatOptionUI alloc] initWithOptions:self setting:setting] autorelease]];
}

- (void)addLongSetting:(LDExporterSetting &)setting
{
	[optionUIs addObject:[[[LongOptionUI alloc] initWithOptions:self setting:setting] autorelease]];
}

- (void)addStringSetting:(LDExporterSetting &)setting
{
	if (setting.isPath())
	{
		// Paths go to TCUserDefault via different functions, and they also have
		// a browse button, which strings lack.
		[optionUIs addObject:[[[PathOptionUI alloc] initWithOptions:self setting:setting] autorelease]];
	}
	else
	{
		[optionUIs addObject:[[[StringOptionUI alloc] initWithOptions:self setting:setting] autorelease]];
	}
}

- (void)addEnumSetting:(LDExporterSetting &)setting
{
	[optionUIs addObject:[[[EnumOptionUI alloc] initWithOptions:self setting:setting] autorelease]];
}

- (void)populate
{
	LDExporterSettingList::iterator it;
	std::stack<int> groupSizes;
	int groupSize = 0;
	NSEnumerator *enumerator;
	OptionUI *optionUI;
	NSView *lastKeyView = okButton;

	optionUIs = [[NSMutableArray alloc] initWithCapacity:settings->size()];
	while ([[docView subviews] count] > 0)
	{
		[[[docView subviews] lastObject] removeFromSuperview];
	}
	for (it = settings->begin(); it != settings->end(); ++it)
	{
		bool inGroup = groupSize > 0;
		
		if (inGroup)
		{
			groupSize--;
			if (groupSize == 0)
			{
				groupSize = groupSizes.top();
				groupSizes.pop();
			}
		}
		if (it->getGroupSize() > 0)
		{
			if (inGroup)
			{
				[self addBoolSetting:*it];
			}
			else
			{
				[self addGroup:*it];
			}
			groupSizes.push(groupSize);
			groupSize = it->getGroupSize();
		}
		else
		{
			switch (it->getType())
			{
				case LDExporterSetting::TBool:
					[self addBoolSetting:*it];
					break;
				case LDExporterSetting::TFloat:
					[self addFloatSetting:*it];
					break;
				case LDExporterSetting::TLong:
					[self addLongSetting:*it];
					break;
				case LDExporterSetting::TString:
					[self addStringSetting:*it];
					break;
				case LDExporterSetting::TEnum:
					[self addEnumSetting:*it];
					break;
				default:
					// Do nothing but get rid of warning.
					break;
			}
		}
	}
	enumerator = [optionUIs objectEnumerator];
	while ((optionUI = [enumerator nextObject]) != nil)
	{
		[lastKeyView setNextKeyView:[optionUI firstKeyView]];
		lastKeyView = [optionUI lastKeyView];
	}
	[lastKeyView setNextKeyView:cancelButton];
	[panel setInitialFirstResponder:[okButton nextKeyView]];
}

- (void)makeOptionUIVisible:(OptionUI *)optionUI
{
	NSRect optionRect = [optionUI frame];
	NSClipView *clipView = [scrollView contentView];
	NSRect docVisibleRect = [scrollView documentVisibleRect];
	CGFloat optionBottom = optionRect.origin.y + optionRect.size.height;
	CGFloat docVisibleBottom = docVisibleRect.origin.y + docVisibleRect.size.height;
	CGFloat delta = optionBottom - docVisibleBottom;
	
	if (delta > 0.0f)
	{
		// Why does the clip view have a different coordinate system?
		NSPoint scrollPoint = [docView convertPoint:NSMakePoint(0.0f, docVisibleRect.origin.y + delta) toView:clipView];
		
		[clipView scrollToPoint:scrollPoint];
		[scrollView reflectScrolledClipView:clipView];
	}
	delta = optionRect.origin.y - docVisibleRect.origin.y;
	if (delta < 0.0f)
	{
		// Why does the clip view have a different coordinate system?
		NSPoint scrollPoint = [docView convertPoint:NSMakePoint(0.0f, docVisibleRect.origin.y + delta) toView:clipView];
		
		[clipView scrollToPoint:scrollPoint];
		[scrollView reflectScrolledClipView:clipView];
	}
}

- (void)newFirstResponder:(NSNotification *)notification
{
	id responder = [[notification userInfo] objectForKey:@"Responder"];

	if ([responder respondsToSelector:@selector(cell)])
	{
		OptionUI *optionUI = [[responder cell] representedObject];

		if (optionUI == nil && [responder isKindOfClass:[NSPopUpButton class]])
		{
			// For some reason NSPopUpButtonCell always returns nil from
			// -representedObject.
			optionUI = [[responder itemAtIndex:0] representedObject];
		}
		if (optionUI != nil)
		{
			[self makeOptionUIVisible:optionUI];
		}
	}
}

- (NSInteger)runModalWithSettings:(LDExporterSettingList &)theSettings titlePrefix:(NSString *)titlePrefix
{
	NSInteger retValue;
	NSString *titleFormat = [panel title];

	[panel setTitle:[NSString stringWithFormat:titleFormat, titlePrefix]];
	settings = &theSettings;
	[self populate];
	[self calcSize];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(newFirstResponder:) name:OPDidChangeFirstResponderNotification object:panel];
	[panel setFrameUsingName:titlePrefix];
	[panel setFrameAutosaveName:titlePrefix];
	retValue = [NSApp runModalForWindow:panel];
    [panel orderOut:self];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:OPDidChangeFirstResponderNotification object:panel];
	settings = NULL;
	return retValue;
}

- (void)windowWillClose:(NSNotification *)aNotification
{
	if ([aNotification object] == panel)
	{
		[NSApp stopModalWithCode:NSModalResponseCancel];
	}
}

- (id)docView
{
	return docView;
}

- (void)windowDidResize:(NSNotification *)aNotification
{
	[self calcSize];
}

- (void)updateEnabled
{
	[self calcSize];
}

- (IBAction)resetAll:(id)sender
{
	for (int i = 0; i < [optionUIs count]; i++)
	{
		[[optionUIs objectAtIndex:i] reset];
	}
	[self updateEnabled];
}

@end
