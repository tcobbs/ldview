#import "LDrawPage.h"

#include <LDLib/LDPreferences.h>

@implementation LDrawPage

- (void)dealloc
{
	[tableViewReorder release];
	[super dealloc];
}

- (void)setupExtraFolders:(const StringVector &)extraDirs
{
	[extraFolders release];
	extraFolders = [[NSMutableArray alloc] initWithCapacity:extraDirs.size()];
	for (size_t i = 0; i < extraDirs.size(); i++)
	{
		[extraFolders addObject:[NSString stringWithCString:extraDirs[i].c_str() encoding:NSASCIIStringEncoding]];
	}
	[extraFoldersTableView reloadData];
}

- (void)setup
{
	[ldrawDirField setStringValue:[NSString stringWithCString:ldPreferences->getLDrawDir() encoding:NSASCIIStringEncoding]];
	[self setupExtraFolders:ldPreferences->getExtraDirs()];
	[super setup];
}

- (bool)validateLDrawDir
{
	return true;
}

- (void)updateLdPreferences
{
	StringVector extraDirs;

	if ([self validateLDrawDir])
	{
		ldPreferences->setLDrawDir([[ldrawDirField stringValue] cStringUsingEncoding:NSASCIIStringEncoding]);
	}
	for (int i = 0; i < [extraFolders count]; i++)
	{
		extraDirs.push_back([[extraFolders objectAtIndex:i] cStringUsingEncoding:NSASCIIStringEncoding]);
	}
	ldPreferences->setExtraDirs(extraDirs);
	[super updateLdPreferences];
}

- (IBAction)resetPage:(id)sender
{
	ldPreferences->loadDefaultLDrawSettings(false);
	[super resetPage:sender];
}

- (void)openPanelDidEnd:(NSOpenPanel *)openPanel returnCode:(int)returnCode contextInfo:(void  *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		NSString *filename = [openPanel filename];
		
		if (filename)
		{
			[extraFolders addObject:filename];
			[extraFoldersTableView reloadData];
			[self valueChanged:self];
		}
	}
}

- (void)addExtraFolder
{
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];
	
	[openPanel setAllowsMultipleSelection:NO];
	[openPanel setCanChooseFiles:NO];
	[openPanel setCanChooseDirectories:YES];
	[openPanel beginSheetForDirectory:nil file:nil modalForWindow:[preferences window] modalDelegate:self didEndSelector:@selector(openPanelDidEnd:returnCode:contextInfo:) contextInfo:NULL];
}

- (void)removeExtraFolder
{
	int index = [extraFoldersTableView selectedRow];
	
	if (index >= 0)
	{
		[extraFolders removeObjectAtIndex:index];
	}
	[extraFoldersTableView reloadData];
	[self valueChanged:self];
}

- (IBAction)addRemoveExtraFolder:(id)sender
{
	switch ([addRemoveExtraFolder selectedSegment])
	{
		case 0:
			[self addExtraFolder];
			break;
		case 1:
			[self removeExtraFolder];
			break;
	}
}

- (NSMutableArray *)tableRows:(TableViewReorder *)sender
{
	return extraFolders;
}

- (void)awakeFromNib
{
	NSSegmentedCell *segmentCell = [addRemoveExtraFolder cell];

	[segmentCell setToolTip:@"Add extra search folder" forSegment:0];
	[segmentCell setToolTip:@"Remove extra search folder" forSegment:0];
	tableViewReorder = [[TableViewReorder alloc] initWithTableView:extraFoldersTableView owner:self dragType:@"LDViewExtraDirsDragType"];
	[super awakeFromNib];
}

- (IBAction)extraFolderSelected:(id)sender
{
	[addRemoveExtraFolder setEnabled:[extraFoldersTableView selectedRow] != -1 forSegment:1];
}

- (IBAction)ldrawFolderBrowse:(id)sender
{
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)rowIndex
{
	return [extraFolders objectAtIndex:rowIndex];
}

- (void)tableViewReorderDidOccur:(id)sender
{
	[self valueChanged:self];
}

@end
