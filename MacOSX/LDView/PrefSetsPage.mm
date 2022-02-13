#import "PrefSetsPage.h"
#import "PrefSetHotKeySheet.h"
#import "PrefSetNewSheet.h"
#import "Preferences.h"
#import "OCLocalStrings.h"
#import "OCUserDefaults.h"
#import "LDViewCategories.h"
#import "MacSetup.h"

#include <LDLib/LDPreferences.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>

@implementation PrefSetsPage

- (void)awakeFromNib
{
	defaultString = [[OCLocalStrings get:@"DefaultPrefSet"] retain];
	sessionNames = [[NSMutableArray alloc] init];
	hotKeys = [[NSMutableDictionary alloc] init];
	[super awakeFromNib];
}

- (void)dealloc
{
	[defaultString release];
	[sessionNames release];
	[hotKeys release];
	[hotKeySheet release];
	[newSheet release];
	[super dealloc];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return [sessionNames count];
}

- (NSString *)hotKeyLabel:(int)index
{
/*
	// I don't want to make this source file Unicode, so I'm going to manually
	// construct the string using the Unicode values for the glyphs I want.
	static unichar uniString[] = {
		0x2325,	// Unicode value for Option glyph.
		0x2318	// Unicode value for Command glyph.
	};
	NSString *prefix = [NSString stringWithCharacters:uniString length:sizeof(uniString) / sizeof(uniString[0])];
*/
	return [NSString stringWithFormat:@"^%d", index];
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
	if ([[aTableColumn identifier] isEqualToString:@"Name"])
	{
		return [sessionNames objectAtIndex:rowIndex];
	}
	else
	{
		NSNumber *value = [hotKeys valueForKey:[sessionNames objectAtIndex:rowIndex]];

		if (value)
		{
			return [self hotKeyLabel:[value intValue]];
		}
		else
		{
			return @"";
		}
	}
}

- (void)selectPrefSet:(NSString *)name
{
	NSUInteger index = 0;

	if (name)
	{
		index = [sessionNames indexOfObject:name];
	}
	if (index != NSNotFound)
	{
		[tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:index] byExtendingSelection:NO];
	}
}

- (NSString *)hotKeyUDKey:(int)index
{
	return [NSString stringWithFormat:@"%s/Key%d", HOT_KEYS_KEY, index];
}

- (void)setupHotKeys
{
	for (int i = 0; i < 10; i++)
	{
		NSString *value = [OCUserDefaults stringForKey:[self hotKeyUDKey:i] defaultValue:nil sessionSpecific:NO];
		
		if (value)
		{
			[hotKeys setObject:[NSNumber numberWithInt:i] forKey:value];
		}
	}
}

- (void)setupTable
{
	TCStringArray *tcSessionNames = TCUserDefaults::getAllSessionNames();
	char *savedSessionName = TCUserDefaults::getSavedSessionNameFromKey(PREFERENCE_SET_KEY);
	int i;
	int count = tcSessionNames->getCount();

	[self setupHotKeys];
	[sessionNames removeAllObjects];
	[sessionNames addObject:defaultString];
	for (i = 0; i < count; i++)
	{
		[sessionNames addObject:[NSString stringWithUTF8String:tcSessionNames->stringAtIndex(i)]];
	}
	tcSessionNames->release();
	[tableView reloadData];
	if (savedSessionName && savedSessionName[0])
	{
		[self selectPrefSet:[NSString stringWithUTF8String:savedSessionName]];
	}
	else
	{
		[self selectPrefSet:nil];
	}
	[self prefSetSelected];
	delete savedSessionName;
}

- (void)setup
{
	[super setup];
	[self setupTable];
}

- (bool)updateLdPreferences
{
	TCStringArray *tcSessionNames = TCUserDefaults::getAllSessionNames();
	int i;
	NSMutableDictionary *oldDict = [NSMutableDictionary dictionary];
	NSMutableDictionary *currentDict = [NSMutableDictionary dictionary];
	NSNumber *object = [NSNumber numberWithInt:1];
	NSArray *hotKeyNames = [hotKeys allKeys];
	NSInteger selectedRow = [tableView selectedRow];
	char *savedSessionName = TCUserDefaults::getSavedSessionNameFromKey(PREFERENCE_SET_KEY);
	NSString *oldSessionName = nil;
	BOOL sessionChanged = NO;

	if (savedSessionName)
	{
		oldSessionName = [NSString stringWithUTF8String:savedSessionName];
		delete savedSessionName;
	}
	// Skip first entry, which is default session.
	for (i = 1; i < [sessionNames count]; i++)
	{
		[currentDict setObject:object forKey:[sessionNames objectAtIndex:i]];
	}
	for (i = 0; i < tcSessionNames->getCount(); i++)
	{
		char *tcSessionName = (*tcSessionNames)[i];
		NSString *sessionName = [NSString stringWithUTF8String:tcSessionName];

		[oldDict setObject:object forKey:sessionName];
		if (![currentDict objectForKey:sessionName])
		{
			TCUserDefaults::removeSession([sessionName UTF8String]);
		}
	}
	tcSessionNames->release();
	// Once again, skip default session.
	for (i = 1; i < [sessionNames count]; i++)
	{
		NSString *sessionName = [sessionNames objectAtIndex:i];
		
		if (![oldDict objectForKey:sessionName])
		{
			TCUserDefaults::setSessionName([sessionName UTF8String], PREFERENCE_SET_KEY);
		}
	}
	if (selectedRow > 0)
	{
		NSString *newSessionName = [sessionNames objectAtIndex:[tableView selectedRow]];

		TCUserDefaults::setSessionName([newSessionName UTF8String], PREFERENCE_SET_KEY);
		if (![newSessionName isEqualToString:oldSessionName])
		{
			sessionChanged = YES;
		}
	}
	else
	{
		TCUserDefaults::setSessionName(NULL, PREFERENCE_SET_KEY);
		if (oldSessionName)
		{
			sessionChanged = YES;
		}
	}
	for (i = 0; i < 10; i++)
	{
		[OCUserDefaults removeValueForKey:[self hotKeyUDKey:i] sessionSpecific:NO];
	}
	for (i = 0; i < [hotKeyNames count]; i++)
	{
		NSString *hotKeyName = [hotKeyNames objectAtIndex:i];
		[OCUserDefaults setString:hotKeyName forKey:[self hotKeyUDKey:[[hotKeys objectForKey:hotKeyName] intValue]] sessionSpecific:NO];
	}
	if (sessionChanged)
	{
		[[self preferences] loadSettings];
		[[self preferences] ldPreferences]->applySettings();
	}
	return [super updateLdPreferences];
}

- (void)hotKeyPressed:(int)index
{
	NSArray *names = [hotKeys allKeysForObject:[NSNumber numberWithInt:index]];

	if ([names count])
	{
		[tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:[sessionNames indexOfObject:[names objectAtIndex:0]]] byExtendingSelection:NO];
		[self updateLdPreferences];
	}
}

- (IBAction)delete:(id)sender
{
	NSInteger selectedRow = [tableView selectedRow];

	[hotKeys removeObjectForKey:[sessionNames objectAtIndex:selectedRow]];
	[sessionNames removeObjectAtIndex:selectedRow];
	[tableView reloadData];
	[self valueChanged:sender];
}

- (IBAction)hotKey:(id)sender
{
	NSString *currentPrefSet = [sessionNames objectAtIndex:[tableView selectedRow]];

	if (!hotKeySheet)
	{
		hotKeySheet = [[PrefSetHotKeySheet alloc] initWithParent:self];
	}
	[hotKeySheet getHotKey:[hotKeys objectForKey:currentPrefSet] withCompletion:^(NSNumber *hotKey){
		if (hotKey)
		{
			if ([hotKey intValue] >= 0)
			{
				NSArray *oldKeys = [hotKeys allKeysForObject:hotKey];

				[hotKeys setObject:hotKey forKey:currentPrefSet];
				if ([oldKeys count])
				{
					[hotKeys removeObjectForKey:[oldKeys objectAtIndex:0]];
				}
			}
			else
			{
				[hotKeys removeObjectForKey:currentPrefSet];
			}
			[tableView reloadData];
			[self valueChanged:sender];
		}
	}];
}

static NSInteger nameSortFunction(id left, id right, void *context)
{
	if ([left isEqualToString:(NSString *)context])
	{
		return NSOrderedAscending;
	}
	else if ([right isEqualToString:(NSString *)context])
	{
		return NSOrderedDescending;
	}
	else
	{
		return [left caseInsensitiveCompare:right];
	}
}

- (IBAction)new:(id)sender
{
	if (!newSheet)
	{
		newSheet = [[PrefSetNewSheet alloc] initWithParent:self];
	}
	[newSheet getNameWithCompletionHandler:^(NSString *name){
		if (name != nil)
		{
			if ([name length] > 0)
			{
				NSEnumerator *enumerator = [sessionNames objectEnumerator];
				NSString *sessionName;
				bool found = false;
				
				while (!found && (sessionName = [enumerator nextObject]) != nil)
				{
					if ([sessionName caseInsensitiveCompare:name] == NSOrderedSame)
					{
						found = true;
					}
				}
				if (found)
				{
					NSRunAlertPanel([OCLocalStrings get:@"Error"], [OCLocalStrings get:@"PrefSetAlreadyExists"], [OCLocalStrings get:@"OK"], nil, nil);
					[self performSelectorOnMainThread:@selector(new:) withObject:sender waitUntilDone:NO];
					return;
				}
				if ([name rangeOfCharacterFromSet:[NSCharacterSet characterSetWithCharactersInString:@"\\/"]].length > 0)
				{
					NSRunAlertPanel([OCLocalStrings get:@"Error"], [OCLocalStrings get:@"PrefSetNameBadChars"], [OCLocalStrings get:@"OK"], nil, nil);
					[self performSelectorOnMainThread:@selector(new:) withObject:sender waitUntilDone:NO];
					return;
				}
				[sessionNames addObject:name];
				[sessionNames sortUsingFunction:nameSortFunction context:defaultString];
				[tableView reloadData];
				[tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:[sessionNames indexOfObject:name]] byExtendingSelection:NO];
				[self valueChanged:sender];
			}
		}
	}];
}

- (void)prefSetSelected
{
	NSInteger selectedRow = [tableView selectedRow];
	
	[deleteButton setEnabled:selectedRow > 0];
	[hotKeyButton setEnabled:selectedRow >= 0];
}

- (IBAction)prefSetSelected:(id)sender
{
	[self prefSetSelected];
	[self valueChanged:sender];
}

- (BOOL)canSwitchPages
{
	if ([preferences isApplyEnabled])
	{
		// ToDo: Localize the following.
		switch (NSRunAlertPanel(@"Warning", @"All Preference Sets changes will be lost if you switch to another tab before applying your changes.  What do you want to do?", [OCLocalStrings get:@"Cancel"], @"Abandon Changes", @"Apply Changes Now"))
		{
			case NSAlertAlternateReturn:
				[self setup];
				break;
			case NSAlertOtherReturn:
				[preferences apply:self];
				break;
			default:
				return NO;
		}
	}
	return YES;
}

@end
