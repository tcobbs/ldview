#import "Statistics.h"
#import "LDViewCategories.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>

@implementation Statistics

- (id)initWithStatistics:(LDLStatistics)statistics
{
	if ((self = [super init]) != nil)
	{
		self->statistics = statistics;
		[super finishInitWithNibName:@"Statistics"];
	}
	return self;
}

- (void)awakeFromNib
{
	[self setField:partsField toFloat:statistics.parts];
	[self setField:modelsField toFloat:statistics.models];
	[self setField:trianglesField toFloat:statistics.triangles];
	[self setField:quadsField toFloat:statistics.quads];
	[self setField:linesField toFloat:statistics.lines];
	[self setField:edgeLinesField toFloat:statistics.edgeLines];
	[self setField:conditionalLinesField toFloat:statistics.conditionalLines];
	[super awakeFromNib];
}

@end
