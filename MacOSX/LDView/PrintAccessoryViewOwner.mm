#import "PrintAccessoryViewOwner.h"
#import "LDViewCategories.h"

#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>

@implementation PrintAccessoryViewOwner

- (id)initWithPrintOperation:(NSPrintOperation *)thePrintOperation
{
	self = [super init];
	if (self != nil)
	{
		printOperation = [thePrintOperation retain];
		[self ldvLoadNibNamed:@"PrintAccessoryView" topLevelObjects:&topLevelObjects];
		[topLevelObjects retain];
	}
	return self;
}

- (CGFloat)pointsToInches:(CGFloat)value
{
	return value / 72.0;
}

- (CGFloat)inchesToPoints:(CGFloat)value
{
	return (CGFloat)(int)(value * 72.0 + 0.5);
}

- (BOOL)getMargin:(float *)pValue udKey:(const char *)udKey
{
	long value = TCUserDefaults::longForKey(udKey, -1, false);
	
	if (value != -1)
	{
		*pValue = (float)value;
		return YES;
	}
	else
	{
		return NO;
	}
}

- (void)initOptions
{
	dpi = (int)TCUserDefaults::longForKey(PRINT_DPI_KEY, 300, false);
	customDpi = (int)TCUserDefaults::longForKey(PRINT_CUSTOM_DPI_KEY, 200, false);
	if ([dpiPopUp indexOfItemWithTag:dpi] != -1)
	{
		[dpiPopUp selectItemWithTag:dpi];
		[dpiField setEnabled:NO];
		[dpiField setStringValue:@""];
	}
	else
	{
		[dpiPopUp selectItemWithTag:0];
		[dpiField setEnabled:YES];
		[dpiField setIntValue:customDpi];
	}
	[printBackgroundCheck setCheck:TCUserDefaults::boolForKey(PRINT_BACKGROUND_KEY, false, false)];
	[adjustThicknessCheck setCheck:TCUserDefaults::boolForKey(PRINT_ADJUST_EDGES_KEY, true, false)];
}

- (void)initMargins
{
	NSPrintInfo *printInfo = [printOperation printInfo];
	NSRect maxRect = [printInfo imageablePageBounds];
	NSSize paperSize = [printInfo paperSize];
	float value;
	
	if ([self getMargin:&value udKey:LEFT_MARGIN_KEY])
	{
		[printInfo setLeftMargin:value];
	}
	if ([self getMargin:&value udKey:RIGHT_MARGIN_KEY])
	{
		[printInfo setRightMargin:value];
	}
	if ([self getMargin:&value udKey:TOP_MARGIN_KEY])
	{
		[printInfo setTopMargin:value];
	}
	if ([self getMargin:&value udKey:BOTTOM_MARGIN_KEY])
	{
		[printInfo setBottomMargin:value];
	}
	minLeft = [self pointsToInches:maxRect.origin.x];
	[leftField setDoubleValue:[self pointsToInches:[printInfo leftMargin]]];
	minRight = [self pointsToInches:paperSize.width - maxRect.size.width - maxRect.origin.x];
	[rightField setDoubleValue:[self pointsToInches:[printInfo rightMargin]]];
	minTop = [self pointsToInches:paperSize.height - maxRect.size.height - maxRect.origin.y];
	[topField setDoubleValue:[self pointsToInches:[printInfo topMargin]]];
	minBottom = [self pointsToInches:maxRect.origin.y];
	[bottomField setDoubleValue:[self pointsToInches:[printInfo bottomMargin]]];
}

- (void)awakeFromNib
{
	// @Todo: Update this to use a view controller to stop using deprecated API.
	START_IGNORE_DEPRECATION
	[printOperation setAccessoryView:accessoryView];
	END_IGNORE_DEPRECATION
	[self initOptions];
	[self initMargins];
}

- (void)dealloc
{
	// @Todo: Update this to use a view controller to stop using deprecated API.
	START_IGNORE_DEPRECATION
	[printOperation setAccessoryView:nil]; 
	END_IGNORE_DEPRECATION
	[printOperation release];
	[self releaseTopLevelObjects:topLevelObjects orTopLevelObject:accessoryView];
	[super dealloc];
}

- (IBAction)dpi:(id)sender
{
	if ([sender selectedTag] == 0)
	{
		[dpiField setEnabled:YES];
		[dpiField setIntValue:customDpi];
		dpi = customDpi;
	}
	else
	{
		dpi = (int)[sender selectedTag];
		[dpiField setEnabled:NO];
		[dpiField setStringValue:@""];
	}
	TCUserDefaults::setLongForKey(dpi, PRINT_DPI_KEY, false);
}

- (void)controlTextDidEndEditing:(NSNotification *)aNotification
{
	NSTextField *field = [aNotification object];
	NSPrintInfo *printInfo = [printOperation printInfo];
	CGFloat value = [field doubleValue];
	const char *udKey = NULL;

	if (field == leftField)
	{
		if ([field floatValue] < minLeft)
		{
			[field setDoubleValue:minLeft];
			value = minLeft;
		}
		[printInfo setLeftMargin:[self inchesToPoints:value]];
		udKey = LEFT_MARGIN_KEY;
	}
	else if (field == rightField)
	{
		if ([field floatValue] < minRight)
		{
			[field setDoubleValue:minRight];
			value = minRight;
		}
		[printInfo setRightMargin:[self inchesToPoints:value]];
		udKey = RIGHT_MARGIN_KEY;
	}
	else if (field == topField)
	{
		if ([field floatValue] < minTop)
		{
			[field setDoubleValue:minTop];
			value = minTop;
		}
		[printInfo setTopMargin:[self inchesToPoints:value]];
		udKey = TOP_MARGIN_KEY;
	}
	else if (field == bottomField)
	{
		if ([field floatValue] < minBottom)
		{
			[field setDoubleValue:minBottom];
			value = minBottom;
		}
		[printInfo setBottomMargin:[self inchesToPoints:value]];
		udKey = BOTTOM_MARGIN_KEY;
	}
	else if (field == dpiField)
	{
		customDpi = [field intValue];
		if (customDpi < 72)
		{
			dpi = 72;
		}
		else if (customDpi > 1200)
		{
			dpi = 1200;
		}
		else
		{
			dpi = customDpi;
		}
		if (dpi != customDpi)
		{
			customDpi = dpi;
			[field setIntValue:customDpi];
		}
		TCUserDefaults::setLongForKey(dpi, PRINT_DPI_KEY, false);
		TCUserDefaults::setLongForKey(dpi, PRINT_CUSTOM_DPI_KEY, false);
	}
	if (udKey)
	{
		TCUserDefaults::setLongForKey((long)[self inchesToPoints:value], udKey, false);
	}
}

- (IBAction)printBackground:(id)sender
{
	TCUserDefaults::setBoolForKey([sender getCheck], PRINT_BACKGROUND_KEY, false);
}

- (IBAction)adjustEdges:(id)sender
{
	TCUserDefaults::setBoolForKey([sender getCheck], PRINT_ADJUST_EDGES_KEY, false);
}

@end
