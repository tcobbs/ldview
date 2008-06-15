#import "SaveExportViewOwner.h"
#import "LDViewCategories.h"

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
				[typeDict setObject:[NSNumber numberWithInt:i] forKey:[NSNumber numberWithInt:[fileTypes count]]];
				[fileTypes addObject:[NSString stringWithUCString:exporter->getTypeDescription()]];
				[extensions addObject:[NSString stringWithASCIICString:exporter->getExtension().c_str()]];
			}
		}
		udTypeKey = [[NSString alloc] initWithASCIICString:SAVE_EXPORT_TYPE_KEY];
		[NSBundle loadNibNamed:@"SaveExportView" owner:self];
	}
	return self;
}

- (void)dealloc
{
	[typeDict release];
	[super dealloc];
}

- (const LDExporter *)currentExporter
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
	const LDExporter *exporter = [self currentExporter];

	if (exporter != NULL)
	{
	}
}

- (LDrawModelViewer::ExportType)exportType
{
	return (LDrawModelViewer::ExportType)[[typeDict objectForKey:[NSNumber numberWithInt:[fileTypePopUp indexOfSelectedItem]]] intValue];
}

- (void)saveSettings
{
	[super saveSettings];
}

@end
