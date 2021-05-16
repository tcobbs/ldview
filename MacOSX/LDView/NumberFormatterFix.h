//
//  NumberFormatterFix.h
//  LDView
//
//  Created by Travis Cobbs on 5/15/21.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NumberFormatterFix : NSNumberFormatter
{
	BOOL minusSign;
}

- (BOOL)getObjectValue:(out id _Nullable * _Nullable)obj forString:(NSString *)string errorDescription:(out NSString * _Nullable * _Nullable)error;
- (nullable NSString *)stringForObjectValue:(nullable id)obj;
- (BOOL)isPartialStringValid:(NSString * _Nonnull * _Nonnull)partialStringPtr proposedSelectedRange:(nullable NSRangePointer)proposedSelRangePtr originalString:(NSString *)origString originalSelectedRange:(NSRange)origSelRange errorDescription:(NSString * _Nullable * _Nullable)error;

@end


NS_ASSUME_NONNULL_END
