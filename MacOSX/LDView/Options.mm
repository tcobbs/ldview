#import "Options.h"
#import "BoolOptionUI.h"
#import "GroupOptionUI.h"
#import "EnumOptionUI.h"

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
	numberWidth = width;
	if (update)
	{
		numberWidth = optimalWidth;
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
		height = [self calcHeightForWidth:width optimalWidth:optimalWidth update:false];
		size.height = height;
		[docView setFrameSize:size];
	}
	[self calcHeightForWidth:width optimalWidth:optimalWidth update:true];
}

- (void)awakeFromNib
{
//	if ([scrollView documentView] == nil)
//	{
//		NSRect frame = [[scrollView contentView] frame];
//
//		frame.origin.x = frame.origin.y = 0.0f;
//		docView = [[NSView alloc] initWithFrame:frame];
//		[docView setAutoresizingMask:NSViewWidthSizable];
//		[scrollView setDocumentView:docView];
//		[docView release];
//	}
}

- (IBAction)ok:(id)sender
{
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
}

- (void)addLongSetting:(LDExporterSetting &)setting
{
}

- (void)addStringSetting:(LDExporterSetting &)setting
{
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
}

- (int)runModalWithSettings:(LDExporterSettingList &)theSettings
{
	int retValue;

	settings = &theSettings;
	[self populate];
	[self calcSize];
	retValue = [NSApp runModalForWindow:panel];
	if (retValue == NSOKButton)
	{
	}
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
