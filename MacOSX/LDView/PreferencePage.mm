#import "PreferencePage.h"
#import "Preferences.h"

@implementation PreferencePage

- (void)findTextFields:(NSView *)parentView
{
	NSArray *subviews = [parentView subviews];
	
	for (int i = 0; i < [subviews count]; i++)
	{
		NSView *subview = [subviews objectAtIndex:i];
		
		if ([subview isKindOfClass:[NSTextField class]])
		{
			NSTextField *textField = (NSTextField *)subview;
			
			if ([textField drawsBackground])
			{
				[[NSNotificationCenter defaultCenter] addObserver:self
														 selector:@selector(textDidEndEditing:)
															 name:NSControlTextDidEndEditingNotification
														   object:subview];
				[[NSNotificationCenter defaultCenter] addObserver:self
														 selector:@selector(textDidChange:)
															 name:NSControlTextDidChangeNotification
														   object:subview];
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
	[changedTextFields release];
	[super dealloc];
}

- (void)awakeFromNib
{
	one = [[NSNumber alloc] initWithInt:1];
	changedTextFields = [[NSMutableDictionary alloc] init];
	[self findTextFields:[tabPage view]];
}

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
	[self groupCheck:sender name:groupName init:NO];
}

- (void)groupCheck:(id)sender name:(NSString *)groupName init:(BOOL)init
{
	NSString *selectorName;
	if (!init)
	{
		[self valueChanged:sender];
	}
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

+ (NSNumber *)numberKey:(id)object
{
	return [NSNumber numberWithUnsignedInt:(unsigned int)object];
}

- (void)textDidChange:(NSNotification *)aNotification
{
	[changedTextFields setObject:one forKey:[[self class] numberKey:[aNotification object]]];
}

- (void)textDidEndEditing:(NSNotification *)aNotification
{
	NSNumber *numberKey = [[self class] numberKey:[aNotification object]];
	if ([changedTextFields objectForKey:numberKey])
	{
		[self valueChanged:[aNotification object]];
		[changedTextFields removeObjectForKey:numberKey];
	}
}

@end
