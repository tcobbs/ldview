#import "PrefSetHotKeyPanel.h"
#import "PrefSetsPage.h"

@implementation PrefSetHotKeyPanel

- (id)initWithParent:(PrefSetsPage *)value
{
	parent = value;	// Don't retain.
	[NSBundle loadNibNamed:@"PrefSetHotKey.nib" owner:self];
	return [super init];
}

- (NSNumber *)getHotKey:(NSNumber *)initialValue
{
	int modalResult;

	if (initialValue)
	{
		[popUp selectItemAtIndex:[initialValue intValue] + 1];
	}
	else
	{
		[popUp selectItemAtIndex:0];
	}
	okPressed = NO;
	[[NSApplication sharedApplication] beginSheet:panel modalForWindow:[[parent preferences] window] modalDelegate:self didEndSelector:nil contextInfo:NULL];
	modalResult = [[NSApplication sharedApplication] runModalForWindow:panel];
	[panel orderOut:self];
	if (modalResult == NSOKButton)
	{
		return [NSNumber numberWithInt:[popUp indexOfSelectedItem] - 1];
	}
	else
	{
		return nil;
	}
}

- (void)awakeFromNib
{
	for (int i = 0; i < 10; i++)
	{
		[popUp addItemWithTitle:[parent hotKeyLabel:i]];
	}
}

- (IBAction)cancel:(id)sender
{
	[[NSApplication sharedApplication] endSheet:panel];
	[[NSApplication sharedApplication] stopModalWithCode:NSCancelButton];
}

- (IBAction)ok:(id)sender
{
	okPressed = YES;
	[[NSApplication sharedApplication] endSheet:panel];
	[[NSApplication sharedApplication] stopModalWithCode:NSOKButton];
}

@end
