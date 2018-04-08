#import "SaveExportViewOwner.h"
#import "LDViewCategories.h"
#import "Options.h"
#import "OCLocalStrings.h"

#include <LDLib/LDUserDefaultsKeys.h>

@implementation SaveExportViewOwner

- (id)initWithModelViewer:(LDrawModelViewer *)aModelViewer
{
	self = [super init];
	if (self)
	{
		int i;
	
		modelViewer = aModelViewer;
		typeDict = [[NSMutableDictionary alloc] init];
		for (i = LDrawModelViewer::ETFirst; i <= LDrawModelViewer::ETLast; i++)
		{
			const LDExporter *exporter = modelViewer->getExporter((LDrawModelViewer::ExportType)i);
			
			if (exporter != NULL)
			{
				// There seems to be a GCC objc++ bug: if the following is put
				// directly into the call that uses it below (without the local
				// variable, the ucstring gets destroyed before it gets into
				// the stringWithUCString method.
				ucstring desc = exporter->getTypeDescription();
				std::string extension = exporter->getExtension();

				[typeDict setObject:[NSNumber numberWithInt:i] forKey:[NSNumber numberWithUnsignedInteger:[fileTypes count]]];
				[fileTypes addObject:[NSString stringWithUCString:desc]];
				[extensions addObject:[NSString stringWithASCIICString:extension.c_str()]];
			}
		}
		udTypeKey = [[NSString alloc] initWithASCIICString:SAVE_EXPORT_TYPE_KEY];
		[self ldvLoadNibNamed:@"SaveExportView" topLevelObjects:&topLevelObjects];
		[topLevelObjects retain];
	}
	return self;
}

- (void)dealloc
{
	[typeDict release];
	[super dealloc];
}

- (LDExporter *)currentExporter
{
	LDrawModelViewer::ExportType type = [self exportType];

	return modelViewer->getExporter(type);
}

- (BOOL)haveOptions
{
	const LDExporter *exporter = [self currentExporter];

	if (exporter != NULL)
	{
		return exporter->getSettings().size() > 0;
	}
	return NO;
}

- (IBAction)fileTypeOptions:(id)sender
{
	LDExporter *exporter = [self currentExporter];

	if (exporter != NULL)
	{
		Options *options = [[Options alloc] init];
		NSString *prefixFormat = [OCLocalStrings get:@"ExtExportFormat"];
		NSString *titlePrefix;
		std::string extension = exporter->getExtension();
		
		convertStringToUpper(&extension[0]);
		titlePrefix = [NSString stringWithFormat:prefixFormat, [NSString stringWithASCIICString:extension.c_str()]];
		if ([options runModalWithSettings:exporter->getSettings() titlePrefix:titlePrefix] == NSModalResponseCancel)
		{
			// Force the exporter to be recreated so if they reset the options
			// before canceling, the reset will be lost.
			modelViewer->getExporter((LDrawModelViewer::ExportType)0, true);
		}
		[options release];
	}
}

- (LDrawModelViewer::ExportType)exportType
{
	return (LDrawModelViewer::ExportType)[[typeDict objectForKey:[NSNumber numberWithInteger:[fileTypePopUp indexOfSelectedItem]]] intValue];
}

- (void)saveSettings
{
	[super saveSettings];
}

@end
