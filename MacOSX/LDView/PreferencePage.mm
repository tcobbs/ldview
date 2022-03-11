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
	[super dealloc];
}

- (void)awakeFromNib
{
	[super awakeFromNib];
	one = [[NSNumber alloc] initWithInt:1];
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

- (bool)updateLdPreferences
{
	return true;
}

- (void)apply
{
	if ([self updateLdPreferences])
	{
		[preferences enableApply:NO];
	}
	else
	{
		[preferences setApplyFailed:self];
	}
}

- (void)textDidChange:(NSNotification *)aNotification
{
	[self valueChanged:[aNotification object]];
}

- (BOOL)canSwitchPages
{
	return YES;
}

- (void)browseForFolder:(void *)contextInfo
{
	[self browseForFolder:contextInfo initialDir:nil];
}

- (void)openPanelDidEnd:(NSOpenPanel *)openPanel returnCode:(NSModalResponse)returnCode contextInfo:(void  *)contextInfo
{	
}

- (void)browseForFolder:(void *)contextInfo initialDir:(NSString *)dir
{
	[self browse:contextInfo forFolder:YES initialDir:dir];
}

- (void)browse:(void *)contextInfo forFolder:(BOOL)folder
{
	[self browse:contextInfo forFolder:folder initialDir:nil];
}

- (void)browse:(void *)contextInfo forFolder:(BOOL)folder initialDir:(NSString *)dir
{
	[self browse:contextInfo forFolder:folder initialDir:dir allowedFileTypes:nil];
}

- (void)browse:(void *)contextInfo forFolder:(BOOL)folder initialDir:(NSString *)dir allowedFileTypes:(NSArray *)allowedFileTypes
{
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];

	openPanel.allowsMultipleSelection = NO;
	openPanel.canChooseFiles = !folder;
	openPanel.canChooseDirectories = folder;
	if (allowedFileTypes != nil)
	{
		openPanel.allowedFileTypes = allowedFileTypes;
	}
	if (dir != nil && dir.length > 0)
	{
		openPanel.directoryURL = [NSURL fileURLWithPath:dir];
	}
	[openPanel beginSheetModalForWindow:[preferences window] completionHandler:^(NSModalResponse response){
		[self openPanelDidEnd:openPanel returnCode:response contextInfo:contextInfo];
	}];
}

@end
