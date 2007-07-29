#import "PrefSetsPage.h"
#import "PrefSetHotKeyPanel.h"
#import "OCLocalStrings.h"
#import "OCUserDefaults.h"
#include <LDLib/LDPreferences.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>

@implementation PrefSetsPage

- (void)awakeFromNib
{
	sessionNames = [[NSMutableArray alloc] init];
	hotKeys = [[NSMutableDictionary alloc] init];
	[super awakeFromNib];
}

- (void)dealloc
{
	[sessionNames release];
	[hotKeys release];
	[hotKeyPanel release];
	[super dealloc];
}

- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return [sessionNames count];
}

- (NSString *)hotKeyLabel:(int)index
{
	// I don't want to make this source file Unicode, so I'm going to manually
	// construct the string using the Unicode values for the glyphs I want.
	static unichar uniString[] = {
		0x2325,	// Unicode value for Option glyph.
		0x2318	// Unicode value for Command glyph.
	};
	NSString *prefix = [NSString stringWithCharacters:uniString length:sizeof(uniString) / sizeof(uniString[0])];
	
	return [NSString stringWithFormat:@"%@%d", prefix, index];
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)rowIndex
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
	int index = 0;
	
	if (name)
	{
		index = [sessionNames indexOfObject:name] + 1;
	}
	[tableView selectRow:index byExtendingSelection:NO];
}

- (void)setupHotKeys
{
	for (int i = 0; i < 10; i++)
	{
		NSString *key = [NSString stringWithFormat:@"%s/Key%d", HOT_KEYS_KEY, i];
		NSString *value = [OCUserDefaults stringForKey:key];
		
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
	[sessionNames addObject:[OCLocalStrings get:@"DefaultPrefSet"]];
	for (i = 0; i < count; i++)
	{
		[sessionNames addObject:[NSString stringWithCString:tcSessionNames->stringAtIndex(i)]];
	}
	tcSessionNames->release();
	[tableView reloadData];
	if (savedSessionName && savedSessionName[0])
	{
		[self selectPrefSet:[NSString stringWithCString:savedSessionName]];
	}
	else
	{
		[self selectPrefSet:nil];
	}
	delete savedSessionName;
}

- (void)setup
{
	[super setup];
	[self setupTable];
}

- (void)updateLdPreferences
{
	[super updateLdPreferences];
}

- (IBAction)delete:(id)sender
{
}

- (IBAction)hotKey:(id)sender
{
	NSNumber *hotKey;
	NSString *currentPrefSet = [sessionNames objectAtIndex:[tableView selectedRow]];

	if (!hotKeyPanel)
	{
		hotKeyPanel = [[PrefSetHotKeyPanel alloc] initWithParent:self];
	}
	hotKey = [hotKeyPanel getHotKey:[hotKeys objectForKey:currentPrefSet]];
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
	}
}

- (IBAction)new:(id)sender
{
}

- (IBAction)prefSetSelected:(id)sender
{
	[self valueChanged:sender];
}

@end
