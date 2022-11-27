#import "OpenGLDriverInfo.h"
#import "OCLocalStrings.h"
#import "LDViewCategories.h"

@implementation OpenGLDriverInfo

- (void)dealloc
{
	[self releaseTopLevelObjects:topLevelObjects orTopLevelObject:panel];
	[super dealloc];
}

- (id) init
{
	self = [super init];
	if (self != nil)
	{
		[self ldvLoadNibNamed:@"OpenGLDriverInfo" topLevelObjects:&topLevelObjects];
		[topLevelObjects retain];
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
	[panel orderOut:self];
}

- (IBAction)ok:(id)sender
{
	[NSApp stopModalWithCode:NSModalResponseOK];
}

- (void)windowWillClose:(NSNotification *)aNotification
{
	if ([aNotification object] == panel)
	{
		[NSApp stopModalWithCode:NSModalResponseOK];
	}
}

@end
