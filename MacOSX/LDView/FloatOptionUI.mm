//
//  FloatOptionUI.mm
//  LDView
//
//  Created by Travis Cobbs on 6/17/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "FloatOptionUI.h"
#import "LDViewCategories.h"
#import "OCLocalStrings.h"

#include <LDExporter/LDExporterSetting.h>
#include <TCFoundation/mystring.h>


@implementation FloatOptionUI

- (bool)validate:(NSString *&)error
{
	float fieldValue;

	if (sscanf([[textField stringValue] UTF8String], "%f", &fieldValue) == 1)
	{
		if (setting->hasMin() && fieldValue < setting->getMinFloatValue())
		{
			[textField setFloatValue:setting->getMinFloatValue()];
		}
		else if (setting->hasMax() && fieldValue > setting->getMaxFloatValue())
		{
			[textField setFloatValue:setting->getMaxFloatValue()];
		}
		else
		{
			value = fieldValue;
			return true;
		}
		// If we get here, the value was either too small or too large.  Set
		// error to contain an error listing the valid range.
		error = [NSString stringWithFormat:[OCLocalStrings get:@"CocoaValueOutOfRange"], [NSString stringWithUTF8String:ftostr(setting->getMinFloatValue()).c_str()], [NSString stringWithUTF8String:ftostr(setting->getMaxFloatValue()).c_str()]];
	}
	else
	{
		// The scanf didn't find a float, so tell the user what is expected.
		error = [OCLocalStrings get:@"DecimalNumberRequired"];
	}
	return false;
}

- (void)commit
{
	setting->setValue(value, true);
}

- (void)valueChanged
{
	[textField setFloatValue:setting->getFloatValue()];
}

@end
