/* RotationCenter */

#import <Cocoa/Cocoa.h>
#import "GenericSheet.h"
#import <LDLoader/LDLFileLine.h>

@interface Statistics: GenericSheet
{
	IBOutlet NSTextField *partsField;
	IBOutlet NSTextField *modelsField;
	IBOutlet NSTextField *trianglesField;
	IBOutlet NSTextField *quadsField;
	IBOutlet NSTextField *linesField;
	IBOutlet NSTextField *edgeLinesField;
	IBOutlet NSTextField *conditionalLinesField;

	LDLStatistics statistics;
}

- (id)initWithStatistics:(LDLStatistics)statistics;

@end
