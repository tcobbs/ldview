#import "PreferencePage.h"
#import "Preferences.h"

@implementation PreferencePage

+ (NSNumber *)numberKey:(id)object
{
	return [NSNumber numberWithUnsignedInt:(unsigned int)object];
}

- (void)findTextFields:(NSView *)parentView
{
	NSArray *subviews = [parentView subviews];
	
	for (int i = 0; i < [subviews count]; i++)
	{
		NSView *subview = [subviews objectAtIndex:i];
		
		if ([subview isKindOfClass:[NSTextField class]])
		{
			NSTextField *textField = (NSTextField *)subview;
			
			if ([textField isEditable])
			{
				[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(textDidChange:) name:NSControlTextDidChangeNotification object:subview];
			}
		}
		else
		{
			[self findTextFields:subview];
		}
	}
}

- (void)dealloc
{
	[one release];
	[origColors release];
	[origBoxTitles release];
	[disabledBoxTitles release];
	[super dealloc];
}

- (void)awakeFromNib
{
	one = [[NSNumber alloc] initWithInt:1];
	origColors = [[NSMutableDictionary alloc] init];
	origBoxTitles = [[NSMutableDictionary alloc] init];
	disabledBoxTitles = [[NSMutableDictionary alloc] init];
	[self findTextFields:[tabPage view]];
}

- (IBAction)resetPage:(id)sender
{
	[self setup];
	[preferences enableApply:YES];
}

 - (Preferences *)preferences
 {
	 return preferences;
 }

- (IBAction)valueChanged:(id)sender
{
	[preferences enableApply:YES];
}

- (void)setPreferences:(Preferences *)value
{
	// Don't retain; it's our parent.
	preferences = value;
	ldPreferences = [preferences ldPreferences];
}

- (void)setup
{
}

- (void)setCheck:(NSButton *)check value:(bool)value
{
	[check setState:value ? NSOnState : NSOffState];
}

- (bool)getCheck:(NSButton *)check
{
	return [check state] == NSOnState;
}

- (void)setColorWell:(NSColorWell *)colorWell r:(int)r g:(int)g b:(int)b
{
	[colorWell setColor:[NSColor colorWithCalibratedRed:r / 255.0 green:g / 255.0 blue:b / 255.0 alpha:1.0f]];
}

- (void)getColorWell:(NSColorWell *)colorWell r:(int *)r g:(int *)g b:(int *)b
{
	[self getColorWell:colorWell r:r g:g b:b a:nil];
}

- (void)getColorWell:(NSColorWell *)colorWell r:(int *)r g:(int *)g b:(int *)b a:(int *)a
{
	float rf, gf, bf, af;
	
	[[colorWell color] getRed:&rf green:&gf blue:&bf alpha:&af];
	*r = (int)(rf * 255.0 + 0.5);
	*g = (int)(gf * 255.0 + 0.5);
	*b = (int)(bf * 255.0 + 0.5);
	if (a)
	{
		*a = (int)(af * 255.0 + 0.5);
	}
}

- (void)enableLabel:(NSTextField *)label value:(BOOL)enabled
{
	if (enabled)
	{
		NSColor *color = [origColors objectForKey:[[self class] numberKey:label]];
		
		if (color)
		{
			[label setTextColor:color];
		}
	}
	else
	{
		if (![origColors objectForKey:[[self class] numberKey:label]])
		{
			[origColors setObject:[label textColor] forKey:[[self class] numberKey:label]];
		}
		[label setTextColor:[NSColor disabledControlTextColor]];
	}
}

- (void)enableBoxTitle:(NSBox *)box value:(BOOL)enabled
{
	NSCell *titleCell = [box titleCell];
	NSNumber *numberKey = [[self class] numberKey:box];

	if (enabled)
	{
		NSAttributedString *boxTitle = [origBoxTitles objectForKey:numberKey];
			
		if (boxTitle)
		{
			[titleCell setAttributedStringValue:boxTitle];
		}
	}
	else
	{
		if (![origBoxTitles objectForKey:numberKey])
		{
			[origBoxTitles setObject:[titleCell attributedStringValue] forKey:numberKey];
		}
		if (![disabledBoxTitles objectForKey:numberKey])
		{
			NSMutableAttributedString *titleString = [[titleCell attributedStringValue] mutableCopy];
			NSRange range;
			NSMutableDictionary *attributes = [[titleString attributesAtIndex:0 effectiveRange:&range] mutableCopy];

			[attributes setObject:[NSColor disabledControlTextColor] forKey:NSForegroundColorAttributeName];
			[titleString setAttributes:[attributes autorelease] range:range];
			[disabledBoxTitles setObject:[titleString autorelease] forKey:numberKey];
		}
		[titleCell setAttributedStringValue:[disabledBoxTitles objectForKey:numberKey]];
	}
	[box setNeedsDisplay:YES];
}

- (void)updateLdPreferences
{
}

- (void)apply
{
	[self updateLdPreferences];
	[preferences enableApply:NO];
}

- (void)groupCheck:(id)sender name:(NSString *)groupName value:(bool)value
{
	[self setCheck:sender value:value];
	[self groupCheck:sender name:groupName init:YES];
}

- (void)groupCheck:(id)sender name:(NSString *)groupName
{
	[self groupCheck:sender name:groupName init:NO];
}

- (void)setUISection:(NSString *)sectionName enabled:(BOOL)enabled
{
	NSString *selectorName;
	
	if (enabled)
	{
		selectorName = [@"enable" stringByAppendingString:sectionName];
	}
	else
	{
		selectorName = [@"disable" stringByAppendingString:sectionName];
	}
	// Try doing THIS in C++!
	[self performSelector:NSSelectorFromString(selectorName)];
}

- (void)groupCheck:(id)sender name:(NSString *)groupName init:(BOOL)init
{
	if (!init)
	{
		[self valueChanged:sender];
	}
	[self setUISection:groupName enabled:[self getCheck:sender]];
}

- (void)textDidChange:(NSNotification *)aNotification
{
	[self valueChanged:[aNotification object]];
}

- (BOOL)canSwitchPages
{
	return YES;
}

@end
