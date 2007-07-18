#import "PreferencePage.h"
#import "Preferences.h"

@implementation PreferencePage

- (IBAction)resetPage:(id)sender
{
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
	[self setup];
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

- (void)enableLabel:(NSTextField *)label value:(BOOL)enabled
{
	if (enabled)
	{
		[label setTextColor:[NSColor textColor]];
	}
	else
	{
		[label setTextColor:[NSColor disabledControlTextColor]];
	}
}

- (void)updateLdPreferences
{
}

- (void)apply
{
	[self updateLdPreferences];
	[preferences enableApply:NO];
}

- (void)groupCheck:(id)sender name:(NSString *)groupName
{
	NSString *selectorName;
	[self valueChanged:sender];
	if ([self getCheck:sender])
	{
		selectorName = [@"enable" stringByAppendingString:groupName];
	}
	else
	{
		selectorName = [@"disable" stringByAppendingString:groupName];
	}
	// Try doing THIS in C++!
	[self performSelector:NSSelectorFromString(selectorName)];
}

@end
