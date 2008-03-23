#import "LDrawPage.h"

@implementation LDrawPage

- (void)setup
{	
	[super setup];
}

- (void)updateLdPreferences
{
	[super updateLdPreferences];
}

- (IBAction)resetPage:(id)sender
{
	//ldPreferences->loadDefaultLDrawSettings(false);
	[super resetPage:sender];
}

- (IBAction)addRemoveExtraFolder:(id)sender
{
}

- (IBAction)extraFolderSelected:(id)sender
{
}

- (IBAction)ldrawFolderBrowse:(id)sender
{
}

@end
