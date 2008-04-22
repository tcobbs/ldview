#import "GoToStep.h"

@implementation GoToStep

- (id) init
{
	return [super initWithNibName:@"GoToStep"];
}

- (void)controlTextDidChange:(NSNotification *)notification
{
	step = [stepField intValue];
	[okButton setEnabled:step > 0];
}

- (int)step
{
	return step;
}

@end
