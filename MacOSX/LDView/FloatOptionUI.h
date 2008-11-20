//
//  FloatOptionUI.h
//  LDView
//
//  Created by Travis Cobbs on 6/17/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "NumberOptionUI.h"


@interface FloatOptionUI : NumberOptionUI
{
	float value;
}

- (bool)validate:(NSString *&)error;
- (void)commit;
- (void)valueChanged;

@end
