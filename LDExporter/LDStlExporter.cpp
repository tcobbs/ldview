#include "LDStlExporter.h"
#include <TRE/TREModel.h>

#if defined WIN32 && defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

LDStlExporter::LDStlExporter(void)
	: LDExporter("StlExporter/")
{
	loadSettings();
}

LDStlExporter::~LDStlExporter(void)
{
}

void LDStlExporter::initSettings(void) const
{
	if (addSetting(LDExporterSetting(ls(_UC("StlScale")),
		udKey("Scale").c_str())))
	{
		LDExporterSetting &setting = m_settings.back();

		setting.addOption(0, ls(_UC("StlScaleLDU")));
		setting.addOption(1, ls(_UC("StlScaleIn")));
		setting.addOption(2, ls(_UC("StlScaleCM")));
		setting.addOption(3, ls(_UC("StlScaleMM")));
		try
		{
			setting.selectOption(m_scaleSelection);
		}
		catch (...)
		{
			setting.selectOption(2);
		}
	}
//	if (addSetting(LDExporterSetting(ls(_UC("StlBinary")), m_binary,
//		udKey("Binary").c_str())))
//	{
//		m_settings.back().setGroupSize(1);
//		if (addSetting(LDExporterSetting(ls(_UC("StlColorFormat")),
//			udKey("ColorFormat").c_str())))
//		{
//			LDExporterSetting &setting = m_settings.back();
//
//			setting.addOption(0, ls(_UC("StlColorFormatNone")));
//			setting.addOption(1, ls(_UC("StlColorFormatVisCam")));
//			setting.addOption(2, ls(_UC("StlColorFormatMagics")));
//			try
//			{
//				setting.selectOption(m_colorFormat);
//			}
//			catch (...)
//			{
//				setting.selectOption(0);
//			}
//		}
//	}
}

void LDStlExporter::dealloc(void)
{
	LDExporter::dealloc();
}

ucstring LDStlExporter::getTypeDescription(void) const
{
	return ls(_UC("StlTypeDescription"));
}

void LDStlExporter::loadSettings(void)
{
	LDExporter::loadSettings();
	m_binary = boolForKey("Binary", false);
	m_colorFormat = longForKey("ColorFormat", 0, false);
	m_scaleSelection = longForKey("Scale", 2, false);
}

int LDStlExporter::doExport(TREModel *pTopModel)
{
	FILE *file = fopen(m_filename.c_str(), "wb");

	loadSettings();
	if (file)
	{
		float scale = 0.04f;	// Default is cm
		
		switch (m_scaleSelection)
		{
		case 0: // LDU
			scale = 1.0f;
			break;
		case 1: // in
			scale = 1.0f / 64.0f;
			break;
		case 2: // cm
			scale = 0.04f;
			break;
		case 3: // mm
			scale = 0.4f;
			break;
		}
		pTopModel->saveSTL(file, scale);
		fclose(file);
		return 0;
	}
	return 1;
}
