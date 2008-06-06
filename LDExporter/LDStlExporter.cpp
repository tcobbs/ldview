#include "LDStlExporter.h"
#include <TRE/TREModel.h>

#if defined WIN32 && defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

LDStlExporter::LDStlExporter(void):
LDExporter("StlExporter/")
{
}

LDStlExporter::~LDStlExporter(void)
{
}

void LDStlExporter::initSettings(void) const
{
	// No settings
}

void LDStlExporter::dealloc(void)
{
	LDExporter::dealloc();
}

ucstring LDStlExporter::getTypeDescription(void) const
{
	return ls(_UC("StlTypeDescription"));
}

void LDStlExporter::loadDefaults(void)
{
	LDExporter::loadDefaults();
}

int LDStlExporter::doExport(TREModel *pTopModel)
{
	FILE *file = fopen(m_filename.c_str(), "wb");

	loadDefaults();
	if (file)
	{
		pTopModel->saveSTL(file);
		fclose(file);
		return 0;
	}
	return 1;
}
