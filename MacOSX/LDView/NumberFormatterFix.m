//
//  NumberFormatterFix.m
//  LDView
//
//  Created by Travis Cobbs on 5/15/21.
//

#import "NumberFormatterFix.h"

@implementation NumberFormatterFix

- (BOOL)getObjectValue:(out id _Nullable * _Nullable)obj forString:(NSString *)string errorDescription:(out NSString * _Nullable * _Nullable)error
{
	if ([string isEqualToString:@"-"])
	{
		minusSign = YES;
		*obj = [NSNumber numberWithDouble:0.0];
		return YES;
	}
	minusSign = NO;
	return [super getObjectValue:obj forString:string errorDescription:error];
}

- (nullable NSString *)stringForObjectValue:(nullable id)obj
{
	if (minusSign)
	{
		return @"-";
	}
	return [super stringForObjectValue:obj];
}

- (BOOL)isPartialStringValid:(NSString * _Nonnull * _Nonnull)partialStringPtr proposedSelectedRange:(nullable NSRangePointer)proposedSelRangePtr originalString:(NSString *)origString originalSelectedRange:(NSRange)origSelRange errorDescription:(NSString * _Nullable * _Nullable)error
{
	if ([*partialStringPtr isEqualToString:@"-"])
	{
		return YES;
	}
	return [super isPartialStringValid:partialStringPtr proposedSelectedRange:proposedSelRangePtr originalString:origString originalSelectedRange:origSelRange errorDescription:error];
}

@end
