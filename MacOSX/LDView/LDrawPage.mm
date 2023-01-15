// The following is necessary to get rid of some truly screwed up warnings
#pragma GCC visibility push(default)

#import "LDrawPage.h"
#import "OCLocalStrings.h"
#import "LDViewCategories.h"

#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDPreferences.h>
#include <LDLib/LDUserDefaultsKeys.h>

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
		[extraFolders addObject:[NSString stringWithUTF8String:extraDirs[i].c_str()]];
	}
	[extraFoldersTableView reloadData];
}

- (void)setup
{
	[ldrawDirField setStringValue:[NSString stringWithUTF8String:ldPreferences->getLDrawDir()]];
	[self setupExtraFolders:ldPreferences->getExtraDirs()];
	[self setCheck:generateThumbnailsCheck value:[self generateThumbnails]];
	[super setup];
}

+ (bool)verifyLDrawDir:(NSString *)ldrawDir
{
	NSFileManager *fileManager = [NSFileManager defaultManager];
	BOOL isDir;

	if ([ldrawDir length] == 0 && !TCUserDefaults::boolForKey(VERIFY_LDRAW_DIR_KEY, true, false))
	{
		return true;
	}
	if ([fileManager fileExistsAtPath:ldrawDir isDirectory:&isDir] && isDir)
	{
		NSString *partsDir = [ldrawDir stringByAppendingPathComponent:@"parts"];

		if ([fileManager fileExistsAtPath:partsDir isDirectory:&isDir] && isDir)
		{
			NSString *pDir = [ldrawDir stringByAppendingPathComponent:@"p"];

			if ([fileManager fileExistsAtPath:pDir isDirectory:&isDir] && isDir)
			{
				return true;
			}
		}
	}
	return false;
}

- (void)updateLDrawDir:(NSString *)ldrawDir apply:(BOOL)apply
{
	const char *value = [ldrawDir UTF8String];
	
	ldPreferences->setLDrawDir(value);
	if (apply)
	{
		ldPreferences->applyLDrawSettings();
		ldPreferences->commitLDrawSettings();
		[ldrawDirField setStringValue:ldrawDir];
		LDrawModelViewer::resetUnofficialDownloadTimes();
	}
}

- (NSString *)ldrawDir
{
	return [NSString stringWithUTF8String:ldPreferences->getLDrawDir()];
}

- (void)updateLDrawDir:(NSString *)ldrawDir
{
	[self updateLDrawDir:ldrawDir apply:YES];
}

- (void)setGenerateThumbnails:(bool)value
{
	BOOL disableThumbnails = value ? NO : YES;
	NSUserDefaults *sud = [NSUserDefaults standardUserDefaults];
	NSMutableDictionary<NSString *, id>* udDomain = [[sud persistentDomainForName:@"com.cobbsville.LDViewQuickLook"] mutableCopy];
	[udDomain setObject:[NSNumber numberWithBool:disableThumbnails] forKey:@"DisableThumbnails"];
	[[NSUserDefaults standardUserDefaults] setPersistentDomain:udDomain forName:@"com.cobbsville.LDViewQuickLook"];
	[sud synchronize];
	[udDomain release];
}

- (bool)updateLdPreferences
{
	StringVector extraDirs;

	if (![[self class] verifyLDrawDir:[ldrawDirField stringValue]])
	{
		[preferences setApplyFailed:self];
		NSAlert *alert = [[[NSAlert alloc] init] autorelease];
		alert.messageText = [OCLocalStrings get:@"InvalidDir"];
		alert.informativeText = [OCLocalStrings get:@"LDrawNotInDir"];
		alert.alertStyle = NSAlertStyleCritical;
		[alert beginSheetModalForWindow:tabPage.view.window completionHandler:nil];
		return false;
	}
	[self updateLDrawDir:[ldrawDirField stringValue] apply:NO];
	for (NSUInteger i = 0; i < [extraFolders count]; i++)
	{
		extraDirs.push_back([[extraFolders objectAtIndex:i] UTF8String]);
	}
	ldPreferences->setExtraDirs(extraDirs);
	[self setGenerateThumbnails:[self getCheck:generateThumbnailsCheck]];
	return [super updateLdPreferences];
}

- (IBAction)resetPage:(id)sender
{
	ldPreferences->loadDefaultLDrawSettings(false);
	[self setGenerateThumbnails:YES];
	[super resetPage:sender];
}

- (void)openPanelDidEnd:(NSOpenPanel *)openPanel returnCode:(NSModalResponse)returnCode contextInfo:(void  *)contextInfo
{
	if (returnCode == NSModalResponseOK)
	{
		NSString *filename = [openPanel ldvFilename];
		
		if (filename)
		{
			if (contextInfo == ldrawDirField)
			{
				[ldrawDirField setStringValue:filename];
			}
			else
			{
				[extraFolders addObject:filename];
				[extraFoldersTableView reloadData];
			}
			[self valueChanged:self];
		}
	}
}

- (void)addExtraFolder
{
	[self browseForFolder:NULL];
}

- (void)removeExtraFolder
{
	NSInteger index = [extraFoldersTableView selectedRow];
	
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

	[segmentCell setToolTip:[OCLocalStrings get:@"AddExtraSearchFolder"] forSegment:0];
	[segmentCell setToolTip:[OCLocalStrings get:@"RemoveExtraSearchFolder"] forSegment:1];
	tableViewReorder = [[TableViewReorder alloc] initWithTableView:extraFoldersTableView owner:self dragType:@"LDViewExtraDirsDragType"];
	[super awakeFromNib];
}

- (IBAction)extraFolderSelected:(id)sender
{
	[addRemoveExtraFolder setEnabled:[extraFoldersTableView selectedRow] != -1 forSegment:1];
}

- (IBAction)ldrawFolderBrowse:(id)sender
{
	[self browseForFolder:ldrawDirField];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return (NSInteger)extraFolders.count;
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
	return [extraFolders objectAtIndex:rowIndex];
}

- (void)tableViewReorderDidOccur:(id)sender
{
	[self valueChanged:self];
}

- (bool)generateThumbnails
{
	NSUserDefaults *sud = [NSUserDefaults standardUserDefaults];
	NSDictionary<NSString *, id>* udDomain = [sud persistentDomainForName:@"com.cobbsville.LDViewQuickLook"];
	BOOL disableThumbnails = [[udDomain objectForKey:@"DisableThumbnails"] boolValue];
	return disableThumbnails ? false : true;
}

@end
