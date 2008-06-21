#import "OptionsPanel.h"

@implementation OptionsPanel

NSString *OPDidChangeFirstResponderNotification = @"OPDidChangeFirstResponderNotification";

- (BOOL)makeFirstResponder:(NSResponder *)aResponder
{
	if ([super makeFirstResponder:aResponder])
	{
		[[NSNotificationCenter defaultCenter] postNotificationName:OPDidChangeFirstResponderNotification object:self userInfo:[NSDictionary dictionaryWithObject:aResponder forKey:@"Responder"]];
		return YES;
	}
	else
	{
		return NO;
	}
}

@end
