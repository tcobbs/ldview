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

@implementation Options

- (id) init
{
	self = [super init];
	if (self != nil)
	{
		margin = 6.0f;
		spacing = 6.0f;
		[NSBundle loadNibNamed:@"Options" owner:self];
	}
	return self;
}

- (void)dealloc
{
	[optionUIs release];
	[panel release];
	[super dealloc];
}

- (void)calcOptionHeightWithEnum:(NSEnumerator *)enumerator optionUI:(OptionUI *)optionUI y:(float &)y width:(float)width leftMargin:(float)leftMargin rightMargin:(float)rightMargin numberWidth:(float)numberWidth optimalWidth:(float &)optimalWidth update:(bool)update enabled:(BOOL)enabled
{
	LDExporterSetting *setting = [optionUI setting];
	LDExporterSetting::Type type = setting->getType();
	float otherWidth = 0.0f;
	
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
		float groupLeftMargin = [optionUI leftGroupMargin];
		float groupRightMargin = [optionUI rightGroupMargin];
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

- (float)calcHeightForWidth:(float)width optimalWidth:(float &)optimalWidth update:(bool)update
{
	float y = margin;
	float numberWidth;
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
	float width;
	float optimalWidth = 0.0f;
	float height;
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
	[NSApp stopModalWithCode:NSOKButton];
}

- (IBAction)cancel:(id)sender
{
	[NSApp stopModalWithCode:NSCancelButton];
}

- (void)addGroup:(LDExporterSetting &)setting
{
	[optionUIs addObject:[[[GroupOptionUI alloc] initWithOptions:self setting:setting] autorelease]];
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
	for (it = settings->begin(); it != settings->end(); it++)
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
			NSRect optionRect = [optionUI frame];
			NSClipView *clipView = [scrollView contentView];
			NSRect docVisibleRect = [scrollView documentVisibleRect];
			float optionBottom = optionRect.origin.y + optionRect.size.height;
			float docVisibleBottom = docVisibleRect.origin.y + docVisibleRect.size.height;
			float delta = optionBottom - docVisibleBottom;

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
	}
}

- (int)runModalWithSettings:(LDExporterSettingList &)theSettings titlePrefix:(NSString *)titlePrefix
{
	int retValue;
	NSString *titleFormat = [panel title];

	[panel setTitle:[NSString stringWithFormat:titleFormat, titlePrefix]];
	settings = &theSettings;
	[self populate];
	[self calcSize];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(newFirstResponder:) name:OPDidChangeFirstResponderNotification object:panel];
	[panel setFrameUsingName:titlePrefix];
	[panel setFrameAutosaveName:titlePrefix];
	retValue = [NSApp runModalForWindow:panel];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:OPDidChangeFirstResponderNotification object:panel];
	settings = NULL;
	return retValue;
}

- (void)windowWillClose:(NSNotification *)aNotification
{
	if ([aNotification object] == panel)
	{
		[NSApp stopModalWithCode:NSCancelButton];
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

@end
