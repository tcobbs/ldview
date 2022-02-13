#import "LocationSheet.h"
#import "LDViewCategories.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>

@implementation LocationSheet

- (id)initWithX:(float)x y:(float)y z:(float)z
{
	if ((self = [super init]) != nil)
	{
		self->x = x;
		self->y = y;
		self->z = z;
	}
	return self;
}

- (void)awakeFromNib
{
	[self setField:xField toFloat:x];
	[self setField:yField toFloat:y];
	[self setField:zField toFloat:z];
	[super awakeFromNib];
}

- (void)controlTextDidChange:(NSNotification *)notification
{
	NSTextField *textField = [notification object];

	if (textField == xField)
	{
		x = [textField floatValue];
	}
	else if (textField == yField)
	{
		y = [textField floatValue];
	}
	else
	{
		z = [textField floatValue];
	}
}

- (float)x
{
	return x;
}

- (float)y
{
	return y;
}

- (float)z
{
	return z;
}

@end
