#import "OpenGLDriverInfo.h"
#import "OCLocalStrings.h"

@implementation OpenGLDriverInfo

- (void)dealloc
{
	[panel release];
	[super dealloc];
}

- (id) init
{
	self = [super init];
	if (self != nil)
	{
		[NSBundle loadNibNamed:@"OpenGLDriverInfo.nib" owner:self];
	}
	return self;
}


- (void)showWithInfo:(NSString *)info numExtensions:(int)numExtensions
{
	[textView setString:info];
	//[textView selectAll:self];
	//[textView insertText:info];
	if (numExtensions == 1)
	{
		[textField setStringValue:[OCLocalStrings get:@"OpenGl1Extension"]];
	}
	else
	{
		[textField setStringValue:[NSString stringWithFormat:[OCLocalStrings get:@"OpenGlnExtensions"], numExtensions]];
	}
	[NSApp runModalForWindow:panel];
}

- (IBAction)ok:(id)sender
{
	[NSApp stopModalWithCode:NSOKButton];
}

- (void)windowWillClose:(NSNotification *)aNotification
{
	if ([aNotification object] == panel)
	{
		[NSApp stopModalWithCode:NSOKButton];
	}
}

@end
