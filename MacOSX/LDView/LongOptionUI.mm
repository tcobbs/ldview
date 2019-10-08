//
//  LongOptionUI.mm
//  LDView
//
//  Created by Travis Cobbs on 6/17/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LongOptionUI.h"
#import "LDViewCategories.h"
#import "OCLocalStrings.h"

#include <LDExporter/LDExporterSetting.h>
#include <TCFoundation/mystring.h>


@implementation LongOptionUI

- (bool)validate:(NSString *&)error
{
	long fieldValue;
	
	if (sscanf([[textField stringValue] UTF8String], "%ld", &fieldValue) == 1)
	{
		if (setting->hasMin() && fieldValue < setting->getMinLongValue())
		{
			[textField setIntValue:(int)setting->getMinLongValue()];
		}
		else if (setting->hasMax() && fieldValue > setting->getMaxLongValue())
		{
			[textField setIntValue:(int)setting->getMaxLongValue()];
		}
		else
		{
			value = fieldValue;
			return true;
		}
		// If we get here, the value was either too small or too large.  Set
		// error to contain an error listing the valid range.
		error = [NSString stringWithFormat:[OCLocalStrings get:@"CocoaValueOutOfRange"], [NSString stringWithUTF8String:ftostr(setting->getMinLongValue()).c_str()], [NSString stringWithUTF8String:ftostr(setting->getMaxLongValue()).c_str()]];
	}
	else
	{
		// The scanf didn't find a long, so tell the user what is expected.
		error = [OCLocalStrings get:@"IntegerRequired"];
	}
	return false;
}

- (void)commit
{
	setting->setValue(value, true);
}

- (void)valueChanged
{
	[textField setIntValue:(int)setting->getLongValue()];
}

@end
