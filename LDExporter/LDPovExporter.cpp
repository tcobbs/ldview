#include "LDPovExporter.h"
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/TCMacros.h>
#include <LDLoader/LDLMainModel.h>
#include <LDLoader/LDLModelLine.h>
#include <LDLoader/LDLTriangleLine.h>
#include <LDLoader/LDLQuadLine.h>
#include <LDLoader/LDLLineLine.h>
#include <LDLoader/LDLCommentLine.h>
#include <LDLoader/LDLPalette.h>
#include <LDLoader/LDLAutoCamera.h>
#include <time.h>
#include <sys/stat.h>
#include <tinyxml.h>

#if defined WIN32 && defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

CharStringMap LDPovExporter::sm_replacementChars;

LDPovExporter::LDPovExporter(void):
LDExporter("PovExporter/")
{
	char *ldrawDir = copyString(LDLModel::lDrawDir());

	if (ldrawDir)
	{
		StringList subDirectories;

		subDirectories.push_back("/pov/p/");
		subDirectories.push_back("/pov/parts/");
		replaceStringCharacter(ldrawDir, '\\', '/');
		stripTrailingPathSeparators(ldrawDir);
		m_ldrawDir = ldrawDir;
		delete ldrawDir;
		for (StringList::const_iterator it = subDirectories.begin();
			it != subDirectories.end(); it++)
		{
			m_searchPath.push_back(m_ldrawDir + *it);
		}
	}
	loadDefaults();
	if (sm_replacementChars.size() == 0)
	{
		sm_replacementChars['.'] = "_dot_";
		sm_replacementChars['-'] = "_dash_";
		sm_replacementChars['/'] = "_slash_";
		sm_replacementChars['\\'] = "_slash_";
		sm_replacementChars['#'] = "_hash_";
		sm_replacementChars[':'] = "_colon_";
		sm_replacementChars['!'] = "_bang_";
	}
}

LDPovExporter::~LDPovExporter(void)
{
}

ucstring LDPovExporter::getTypeDescription(void) const
{
	return ls(_UC("PovTypeDescription"));
}

void LDPovExporter::loadDefaults(void)
{
	char *temp;

	LDExporter::loadDefaults();
	m_quality = longForKey("Quality", 2);
	m_refls = boolForKey("Reflections", true);
	m_shads = boolForKey("Shadows", true);
	m_findReplacements = boolForKey("FindReplacements", false);
	m_xmlMap = boolForKey("XmlMap", true);
	m_xmlMapPath = pathForKey("XmlMapPath");
	m_inlinePov = boolForKey("InlinePov", true);
	m_hideStuds = boolForKey("HideStuds", false);
	m_unmirrorStuds = boolForKey("UnmirrorStuds", true);
	m_floor = boolForKey("Floor", true);
	m_floorAxis = longForKey("FloorAxis", 1);
	m_selectedAspectRatio = longForKey("SelectedAspectRatio", 1);
	m_customAspectRatio = floatForKey("CustomAspectRatio", 1.5f);
	m_edgeRadius = floatForKey("EdgeRadius", 0.15f);
	m_ambient = floatForKey("Ambient", 0.4f);
	m_diffuse = floatForKey("Diffuse", 0.4f);
	m_refl = floatForKey("Refl", 0.08f);
	m_phong = floatForKey("Phong", 0.5f);
	m_phongSize = floatForKey("PhongSize", 40.0f);
	m_transRefl = floatForKey("TransRefl", 0.2f);
	m_transFilter = floatForKey("TransFilter", 0.85f);
	m_transIoR = floatForKey("TransIoR", 1.25f);
	m_rubberRefl = floatForKey("RubberRefl", 0.0f);
	m_rubberPhong = floatForKey("RubberPhong", 0.1f);
	m_rubberPhongSize = floatForKey("RubberPhongSize", 10.0f);
	m_chromeRefl = floatForKey("ChromeRefl", 0.85f);
	m_chromeBril = floatForKey("ChromeBril", 5.0f);
	m_chromeSpec = floatForKey("ChromeSpecular", 0.8f);
	m_chromeRough = floatForKey("ChromeRoughness", 0.01f);
	temp = stringForKey("TopInclude");
	if (temp != NULL)
	{
		m_topInclude = temp;
		delete temp;
	}
	else
	{
		m_topInclude = "";
	}
	temp = stringForKey("BottomInclude");
	if (temp != NULL)
	{
		m_bottomInclude = temp;
		delete temp;
	}
	else
	{
		m_bottomInclude = "";
	}
}

void LDPovExporter::addEdgesSettings(void) const
{
	LDExporter::addEdgesSettings();
	addSetting(ls(_UC("PovEdgeRadius")), m_edgeRadius,
		udKey("EdgeRadius").c_str(), 0.001f, 1000.0f);
}

void LDPovExporter::addGeometrySettings(void) const
{
	LDExporter::addGeometrySettings();
	addSetting(LDExporterSetting(ls(_UC("PovHideStuds")), m_hideStuds,
		udKey("HideStuds").c_str()));
}

void LDPovExporter::initSettings(void) const
{
	// Uncomment the following lines to test a top-level boolean group setting.
	//addSetting(LDExporterSetting(_UC("Top Level Test Group"), m_shads,
	//	udKey("Shadows").c_str()));
	//m_settings.back().setGroupSize(2);
	//addSetting(LDExporterSetting(_UC("Top Level Test Group Item 1"), m_shads,
	//	udKey("Shadows").c_str()));
	//addSetting(LDExporterSetting(_UC("Top Level Test Group Item 2"), m_shads,
	//	udKey("Shadows").c_str()));
	// End of top-level boolean group test.
	addSetting(LDExporterSetting(ls(_UC("PovGeneral")),
		9));
	if (addSetting(LDExporterSetting(ls(_UC("PovQuality")),
		udKey("Quality").c_str())))
	{
		LDExporterSetting &setting = m_settings.back();

		setting.addOption(0, ls(_UC("Pov0Quality")));
		setting.addOption(1, ls(_UC("Pov1Quality")));
		setting.addOption(2, ls(_UC("Pov2Quality")));
		setting.addOption(3, ls(_UC("Pov3Quality")));
		try
		{
			setting.selectOption(m_quality);
		}
		catch (...)
		{
			setting.selectOption(2);
		}
	}
	if (addSetting(LDExporterSetting(ls(_UC("PovAspectRatio")),
		udKey("SelectedAspectRatio").c_str())))
	{
		LDExporterSetting &setting = m_settings.back();

		setting.addOption(0, _UC("5:4"));
		setting.addOption(1, _UC("4:3"));
		setting.addOption(2, _UC("3:2"));
		setting.addOption(3, _UC("5:3"));
		setting.addOption(4, _UC("16:9"));
		setting.addOption(5, _UC("2.35:1"));
		setting.addOption(6, ls(_UC("PovCurAspectRatio")));
		setting.addOption(7, ls(_UC("PovCustom")));
		try
		{
			setting.selectOption(m_selectedAspectRatio);
		}
		catch (...)
		{
			setting.selectOption(1);
		}
	}
	addSetting(LDExporterSetting(ls(_UC("PovCustomAspectRatio")),
		m_customAspectRatio, udKey("CustomAspectRatio").c_str()));
	addSetting(LDExporterSetting(ls(_UC("PovFloor")), m_floor,
		udKey("Floor").c_str()));
	m_settings.back().setGroupSize(1);
	if (addSetting(LDExporterSetting(ls(_UC("PovFloorAxis")),
		udKey("FloorAxis").c_str())))
	{
		LDExporterSetting &setting = m_settings.back();

		setting.addOption(0, ls(_UC("PovFloorAxisX")));
		setting.addOption(1, ls(_UC("PovFloorAxisY")));
		setting.addOption(2, ls(_UC("PovFloorAxisZ")));
		try
		{
			setting.selectOption(m_floorAxis);
		}
		catch (...)
		{
			setting.selectOption(1);
		}
	}
	addSetting(LDExporterSetting(ls(_UC("PovReflections")), m_refls,
		udKey("Reflections").c_str()));
	addSetting(LDExporterSetting(ls(_UC("PovShadows")), m_shads,
		udKey("Shadows").c_str()));
	addSetting(LDExporterSetting(ls(_UC("PovUnmirrorStuds")), m_unmirrorStuds,
		udKey("UnmirrorStuds").c_str()));
	addSetting(LDExporterSetting(ls(_UC("PovTopInclude")), m_topInclude.c_str(),
		udKey("TopInclude").c_str()));
	addSetting(LDExporterSetting(ls(_UC("PovBottomInclude")),
		m_bottomInclude.c_str(), udKey("BottomInclude").c_str()));
	LDExporter::initSettings();
	// Uncomment the below to test top-level number settings to verify
	// alignment.
	//addSetting(ls(_UC("PovQuality")), m_quality, udKey("Quality").c_str(), 0,
	//	3);
	// Uncomment the below to test a long setting.
	//addSetting(_UC("Long Setting"), 42l, udKey("TestLongSetting").c_str(), -10l,
	//	100l);
	addSetting(LDExporterSetting(ls(_UC("PovNativeGeometry")),
		3));
	addSetting(LDExporterSetting(ls(_UC("PovInlinePov")), m_inlinePov,
		udKey("InlinePov").c_str()));
	addSetting(LDExporterSetting(ls(_UC("PovFindReplacements")),
		m_findReplacements, udKey("FindReplacements").c_str()));
	if (addSetting(LDExporterSetting(ls(_UC("PovXmlMap")), m_xmlMap,
		udKey("XmlMap").c_str())))
	{
		m_settings.back().setGroupSize(1);
	}
	if (addSetting(LDExporterSetting(ls(_UC("PovXmlMapPath")),
		m_xmlMapPath.c_str(), udKey("XmlMapPath").c_str())))
	{
		m_settings.back().setIsPath(true);
	}
	addSetting(LDExporterSetting(ls(_UC("PovLighting")), 2));
	addSetting(ls(_UC("PovAmbient")), m_ambient, udKey("Ambient").c_str(), 0.0f,
		1.0f);
	addSetting(ls(_UC("PovDiffuse")), m_diffuse, udKey("Diffuse").c_str(), 0.0f,
		1.0f);
	addSetting(LDExporterSetting(ls(_UC("PovMaterialProps")), 3));
	addSetting(ls(_UC("PovRefl")), m_refl, udKey("Refl").c_str(), 0.0f,
		1.0f);
	addSetting(ls(_UC("PovPhong")), m_phong, udKey("Phong").c_str(), 0.0f,
		10.0f);
	addSetting(ls(_UC("PovPhongSize")), m_phongSize, udKey("PhongSize").c_str(),
		0.0f, 10000.0f);
	addSetting(LDExporterSetting(ls(_UC("PovTransMaterialProps")), 3));
	addSetting(ls(_UC("PovRefl")), m_transRefl, udKey("TransRefl").c_str(),
		0.0f, 1.0f);
	addSetting(ls(_UC("PovFilter")), m_transFilter,
		udKey("TransFilter").c_str(), 0.0f, 1.0f);
	addSetting(ls(_UC("PovIoR")), m_transIoR, udKey("TransIoR").c_str(),
		1.0f, 1000.0f);
	addSetting(LDExporterSetting(ls(_UC("PovRubberMaterialProps")), 3));
	addSetting(ls(_UC("PovRefl")), m_rubberRefl, udKey("RubberRefl").c_str(),
		0.0f, 1.0f);
	addSetting(ls(_UC("PovPhong")), m_rubberPhong, udKey("RubberPhong").c_str(),
		0.0f, 10.0f);
	addSetting(ls(_UC("PovPhongSize")), m_rubberPhongSize,
		udKey("RubberPhongSize").c_str(), 0.0f, 10000.0f);
	addSetting(LDExporterSetting(ls(_UC("PovChromeMaterialProps")), 4));
	addSetting(ls(_UC("PovRefl")), m_chromeRefl, udKey("ChromeRefl").c_str(),
		0.0f, 1.0f);
	addSetting(ls(_UC("PovBril")), m_chromeBril, udKey("ChromeBril").c_str(),
		0.0f, 10000.0f);
	addSetting(ls(_UC("PovSpec")), m_chromeSpec, udKey("ChromeSpecular").c_str(),
		0.0f, 1.0f);
	addSetting(ls(_UC("PovRough")), m_chromeRough, udKey("ChromeRoughness").c_str(),
		0.0f, 1.0f);
}

void LDPovExporter::dealloc(void)
{
	LDExporter::dealloc();
}

void LDPovExporter::loadXmlMatrices(TiXmlElement *matrices)
{
	TiXmlElement *element;

	for (element = matrices->FirstChildElement(); element != NULL;
		element = element->NextSiblingElement())
	{
		m_xmlMatrices[element->Value()] = element->GetText();
	}
}

std::string LDPovExporter::loadPovMapping(
	TiXmlElement *element,
	const char *ldrawElementName,
	PovMapping &mapping)
{
	TiXmlElement *child = element->FirstChildElement("POVName");
	std::string ldrawValue;
	std::string povVersion;

	if (child == NULL)
	{
		return "";
	}
	for (; child != NULL; child = child->NextSiblingElement("POVName"))
	{
		PovName name;
		TiXmlAttribute *attr;

		name.name = child->GetText();
		for (attr = child->FirstAttribute(); attr != NULL; attr = attr->Next())
		{
			name.attributes[attr->Name()] = attr->Value();
		}
		mapping.names.push_back(name);
	}
	child = element->FirstChildElement(ldrawElementName);
	if (child == NULL)
	{
		return "";
	}
	ldrawValue = child->GetText();
	child = element->FirstChildElement("POVVersion");
	if (child)
	{
		povVersion = child->GetText();
	}
	for (child = element->FirstChildElement("POVFilename"); child != NULL;
		child = child->NextSiblingElement("POVFilename"))
	{
		std::string filename = child->GetText();

		mapping.povFilenames.push_back(filename);
		if (povVersion.size() > 0)
		{
			m_includeVersions[filename] = povVersion;
		}
	}
	return ldrawValue;
}

void LDPovExporter::loadXmlColors(TiXmlElement *colors)
{
	TiXmlElement *element;

	for (element = colors->FirstChildElement(); element != NULL;
		element = element->NextSiblingElement())
	{
		PovMapping colorMapping;
		std::string ldrawValue = loadPovMapping(element, "LDrawNumber",
			colorMapping);

		if (ldrawValue.size() > 0)
		{
			m_xmlColors[(TCULong)atoi(ldrawValue.c_str())] = colorMapping;
		}
	}
}

void LDPovExporter::loadXmlElements(TiXmlElement *elements)
{
	TiXmlElement *element;

	for (element = elements->FirstChildElement(); element != NULL;
		element = element->NextSiblingElement())
	{
		PovElement povElement;
		std::string ldrawFilename = loadPovMapping(element, "LDrawFilename",
			povElement);

		if (ldrawFilename.size() > 0)
		{
			TiXmlElement *child = element->FirstChildElement("MatrixRef");
			std::string matrixString;
			TCFloat *m;

			if (child)
			{
				matrixString = m_xmlMatrices[child->GetText()];
			}
			if (matrixString.size() == 0)
			{
				child = element->FirstChildElement("Matrix");

				if (child)
				{
					matrixString = child->GetText();
				}
			}
			m = povElement.matrix;
			if (sscanf(matrixString.c_str(),
				"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,", &m[0],
				&m[1], &m[2], &m[3], &m[4], &m[5], &m[6], &m[7], &m[8], &m[9],
				&m[10], &m[11], &m[12], &m[13], &m[14], &m[15]) != 16)
			{
				TCVector::initIdentityMatrix(m);
			}
			m_xmlElements[ldrawFilename] = povElement;
		}

	}
}

void LDPovExporter::loadLDrawPovXml(void)
{
	std::string filename;

	if (m_xmlMapPath.size() > 0)
	{
		filename = m_xmlMapPath;
	}
	else
	{
		filename = m_ldrawDir + "/pov/LDrawPOV.xml";
	}
	TiXmlDocument doc(filename);

	if (doc.LoadFile())
	{
		TiXmlHandle hDoc(&doc);
		TiXmlElement *root =
			hDoc.FirstChildElement("LDrawPOV").Element();
		TiXmlElement *element;

		if (root == NULL)
		{
			return;
		}
		element = root->FirstChildElement("Colors");
		if (element != NULL)
		{
			loadXmlColors(element);
		}
		element = root->FirstChildElement("Matrices");
		if (element != NULL)
		{
			loadXmlMatrices(element);
		}
		element = root->FirstChildElement("Elements");
		if (element != NULL)
		{
			loadXmlElements(element);
		}
	}
}

int LDPovExporter::doExport(LDLModel *pTopModel)
{
	std::string filename = m_filename;

	loadDefaults();
	m_pTopModel = pTopModel;
	if (filename.size() == 0)
	{
		filename = pTopModel->getFilename();
		char *dotSpot = strrchr(&filename[0], '.');

		if (dotSpot)
		{
			filename.resize(dotSpot - &filename[0]);
		}
		filename += ".pov";
	}
	if ((m_pPovFile = fopen(filename.c_str(), "w")) != NULL)
	{
		if (m_xmlMap)
		{
			loadLDrawPovXml();
		}
		if (!writeHeader())
		{
			return 1;
		}
		if (m_topInclude.size() > 0)
		{
			fprintf(m_pPovFile, "#include \"%s\"\n\n", m_topInclude.c_str());
		}
		if (!writeCamera())
		{
			return 1;
		}
		if (!writeLights())
		{
			return 1;
		}
		fprintf(m_pPovFile, "\nbackground { color rgb <BG_R,BG_G,BG_B> }\n\n");
		if (m_edges)
		{
			TCFloat matrix[16];

			TCVector::initIdentityMatrix(matrix);
			m_pTopModel->scanPoints(this,
				(LDLScanPointCallback)&LDPovExporter::scanEdgePoint, matrix,
				(LDLModel::ScanPointType)(LDLModel::SPTEdgeLine |
				LDLModel::SPTConditionalLine));
			writeEdgeLineMacro();
			writeEdgeColor();
		}
		m_colorsUsed[7] = true;
		if (!scanModelColors(m_pTopModel))
		{
			return 1;
		}
		if (!writeModelColors())
		{
			return 1;
		}
		if (!writeEdges())
		{
			return 1;
		}
		if (!writeModel(m_pTopModel, TCVector::getIdentityMatrix()))
		{
			return 1;
		}
		writeMainModel();
		writeFloor();
		if (m_bottomInclude.size() > 0)
		{
			fprintf(m_pPovFile, "#include \"%s\"\n\n", m_bottomInclude.c_str());
		}
		fclose(m_pPovFile);
	}
	else
	{
		consolePrintf(_UC("%s"), (const char *)ls(_UC("PovErrorCreatingPov")));
	}
	return 0;
}

void LDPovExporter::writeMainModel(void)
{
	fprintf(m_pPovFile, "// ", m_pTopModel->getName());
	if (m_pTopModel->getName())
	{
		fprintf(m_pPovFile, "%s\n", m_pTopModel->getName());
	}
	else
	{
		char *name = filenameFromPath(m_pTopModel->getFilename());

		fprintf(m_pPovFile, "%s\n", name);
		delete name;
	}
	fprintf(m_pPovFile, "object {\n\t%s\n",
		getDeclareName(m_pTopModel, false).c_str());
	writeColor(7);
	fprintf(m_pPovFile, "\n}\n\n");
}

void LDPovExporter::writeFloor(void)
{
	fprintf(m_pPovFile, "// Floor\n");
	fprintf(m_pPovFile, "#if (FLOOR != 0)\n");
	fprintf(m_pPovFile, "object {\n");
	fprintf(m_pPovFile, "\tplane { FLOOR_AXIS, FLOOR_LOC hollow }\n");
	fprintf(m_pPovFile, "\ttexture {\n");
	fprintf(m_pPovFile,
		"\t\tpigment { color rgb <FLOOR_R,FLOOR_G,FLOOR_B> }\n");
	fprintf(m_pPovFile,
		"\t\tfinish { ambient FLOOR_AMB diffuse FLOOR_DIFF }\n");
	fprintf(m_pPovFile, "\t}\n");
	fprintf(m_pPovFile, "}\n");
	fprintf(m_pPovFile, "#end\n\n");
}

std::string LDPovExporter::getAspectRatio(void)
{
	switch (m_selectedAspectRatio)
	{
	case 0:
		m_width = 5;
		m_height = 4;
		return "5/4";
	case 2:
		m_width = 3;
		m_height = 2;
		return "3/2";
	case 3:
		m_width = 5;
		m_height = 3;
		return "5/3";
	case 4:
		m_width = 16;
		m_height = 9;
		return "16/9";
	case 5:
		m_width = 235;
		m_height = 100;
		return "2.35";
	case 6:
		{
			std::string aspect = ftostr(m_width);

			aspect += "/";
			aspect += ftostr(m_height);
			return aspect;
		}
	case 7:
		m_width = m_customAspectRatio;
		m_height = 1.0f;
		return ftostr(m_width / m_height);
	default:
		m_width = 4;
		m_height = 3;
		return "4/3";
	}
}

bool LDPovExporter::writeHeader(void)
{
	time_t genTime = time(NULL);
	const char *author = m_pTopModel->getAuthor();
	char *filename = filenameFromPath(m_pTopModel->getFilename());
	std::string floorAxis;
	std::string floorLoc;

	fprintf(m_pPovFile, "// %s %s%s%s %s\n", (const char *)ls("PovGeneratedBy"),
		m_appName.c_str(), m_appVersion.size() > 0 ? " " : "",
		m_appVersion.c_str(), m_appCopyright.c_str());
	fprintf(m_pPovFile, "// %s %s\n", (const char *)ls("PovSee"),
		m_appUrl.c_str());
	fprintf(m_pPovFile, "// %s %s", (const char *)ls("PovDate"),
		ctime(&genTime));
	if (filename != NULL)
	{
		fprintf(m_pPovFile, "// %s %s\n", (const char *)ls("PovLDrawFile"),
			filename);
		delete filename;
	}
	if (author != NULL)
	{
		fprintf(m_pPovFile, "// %s %s\n", (const char *)ls("PovLDrawAuthor"),
			author);
	}
	fprintf(m_pPovFile, ls("PovNote"), m_appName.c_str());
	fprintf(m_pPovFile, "#declare MIN_X = %s;\n",
		ftostr(m_boundingMin[0]).c_str());
	fprintf(m_pPovFile, "#declare MAX_X = %s;\n",
		ftostr(m_boundingMax[0]).c_str());
	fprintf(m_pPovFile, "#declare MIN_Y = %s;\n",
		ftostr(m_boundingMin[1]).c_str());
	fprintf(m_pPovFile, "#declare MAX_Y = %s;\n",
		ftostr(m_boundingMax[1]).c_str());
	fprintf(m_pPovFile, "#declare MIN_Z = %s;\n",
		ftostr(m_boundingMin[2]).c_str());
	fprintf(m_pPovFile, "#declare MAX_Z = %s;\n",
		ftostr(m_boundingMax[2]).c_str());
	fprintf(m_pPovFile, "#declare QUAL = %ld;\t// %s\n", m_quality,
		(const char *)ls("PovQualDesc"));
	fprintf(m_pPovFile, "#declare FLOOR = %d;\t// %s\n", m_floor ? 1 : 0,
		(const char *)ls("PovFloorDesc"));
	switch (m_floorAxis)
	{
	case 0:
		floorAxis = "x";
		floorLoc = "MIN_X";
		break;
	case 2:
		floorAxis = "z";
		floorLoc = "MAX_Z";
		break;
	default:
		floorAxis = "y";
		floorLoc = "MAX_Y";
		break;
	}
	fprintf(m_pPovFile, "#declare FLOOR_LOC = %s;\t// %s\n", floorLoc.c_str(),
		(const char *)ls("PovFloorLocDesc"));
	fprintf(m_pPovFile, "#declare FLOOR_AXIS = %s;\t// %s\n", floorAxis.c_str(),
		(const char *)ls("PovFloorAxisDesc"));
	fprintf(m_pPovFile, "#declare FLOOR_R = 0.8;\t// %s\n",
		(const char *)ls("PovFLOOR_RDesc"));
	fprintf(m_pPovFile, "#declare FLOOR_G = 0.8;\t// %s\n",
		(const char *)ls("PovFLOOR_GDesc"));
	fprintf(m_pPovFile, "#declare FLOOR_B = 0.8;\t// %s\n",
		(const char *)ls("PovFLOOR_BDesc"));
	fprintf(m_pPovFile, "#declare FLOOR_AMB = 0.4;\t// %s\n",
		(const char *)ls("PovFLOOR_AMBDesc"));
	fprintf(m_pPovFile, "#declare FLOOR_DIFF = 0.4;\t// %s\n",
		(const char *)ls("PovFLOOR_DIFFDesc"));
	fprintf(m_pPovFile, "#declare REFLS = %d;\t// %s\n", m_refls ? 1 : 0,
		(const char *)ls("PovReflsDesc"));
	fprintf(m_pPovFile, "#declare SHADS = %d;\t// %s\n", m_shads ? 1 : 0,
		(const char *)ls("PovShadsDesc"));
	fprintf(m_pPovFile, "#declare AMB = %s;\n", ftostr(m_ambient).c_str());
	fprintf(m_pPovFile, "#declare DIF = %s;\n", ftostr(m_diffuse).c_str());
	fprintf(m_pPovFile, "#declare REFL = %s;\n", ftostr(m_refl).c_str());
	fprintf(m_pPovFile, "#declare PHONG = %s;\n", ftostr(m_phong).c_str());
	fprintf(m_pPovFile, "#declare PHONGS = %s;\n", ftostr(m_phongSize).c_str());
	fprintf(m_pPovFile, "#declare TREFL = %s;\n", ftostr(m_transRefl).c_str());
	fprintf(m_pPovFile, "#declare TFILT = %s;\n",
		ftostr(m_transFilter).c_str());
	fprintf(m_pPovFile, "#declare IOR = %s;\n", ftostr(m_transIoR).c_str());
	fprintf(m_pPovFile, "#declare RUBBER_REFL = %s;\n",
		ftostr(m_rubberRefl).c_str());
	fprintf(m_pPovFile, "#declare RUBBER_PHONG = %s;\n",
		ftostr(m_rubberPhong).c_str());
	fprintf(m_pPovFile, "#declare RUBBER_PHONGS = %s;\n",
		ftostr(m_rubberPhongSize).c_str());
	fprintf(m_pPovFile, "#declare CHROME_REFL = %s;\n",
		ftostr(m_chromeRefl).c_str());
	fprintf(m_pPovFile, "#declare CHROME_BRIL = %s;\n",
		ftostr(m_chromeBril).c_str());
	fprintf(m_pPovFile, "#declare CHROME_SPEC = %s;\n",
		ftostr(m_chromeSpec).c_str());
	fprintf(m_pPovFile, "#declare CHROME_ROUGH = %s;\n",
		ftostr(m_chromeRough).c_str());
	fprintf(m_pPovFile, "#declare SW = %s;\t// %s\n",
		ftostr(m_seamWidth).c_str(), (const char *)ls("PovSeamWidthDesc"));
	fprintf(m_pPovFile, "#declare STUDS = %d;\t// %s\n", m_hideStuds ? 0 : 1,
		(const char *)ls("PovStudsDesc"));
	fprintf(m_pPovFile, "#declare IPOV = %d;\t// %s\n", m_inlinePov ? 1 : 0,
		(const char *)ls("PovInlinePovDesc"));
	if (m_edges)
	{
		fprintf(m_pPovFile, "#declare EDGERAD = %s;\n",
			ftostr(m_edgeRadius).c_str());
	}
	fprintf(m_pPovFile, "#declare BG_R = %s;\t// %s\n",
		ftostr(m_backgroundR).c_str(), (const char *)ls("PovBG_RDesc"));
	fprintf(m_pPovFile, "#declare BG_G = %s;\t// %s\n",
		ftostr(m_backgroundG).c_str(), (const char *)ls("PovBG_GDesc"));
	fprintf(m_pPovFile, "#declare BG_B = %s;\t// %s\n",
		ftostr(m_backgroundB).c_str(), (const char *)ls("PovBG_BDesc"));
	if (m_xmlMap)
	{
		fprintf(m_pPovFile, "#declare ORIGVER = version;\t// %s\n",
			(const char *)ls("OrigVerDesc"));
	}
	fprintf(m_pPovFile, "\n");

	if (m_findReplacements)
	{
		writeInclude("lpovcolordefs.inc");
	}
	return true;
}

std::string LDPovExporter::getModelFilename(const LDLModel *pModel)
{
	std::string buf;
	const char *modelFilename = pModel->getName();

	if (!modelFilename && pModel == m_pTopModel)
	{
		char *temp = filenameFromPath(m_pTopModel->getFilename());
		buf = temp;
		delete temp;
	}
	else
	{
		buf = modelFilename;
	}
	return buf;
}

std::string LDPovExporter::getDeclareName(
	LDLModel *pModel,
	bool mirrored)
{
	return getDeclareName(getModelFilename(pModel), mirrored);
}

std::string LDPovExporter::replaceSpecialChacters(const char *string)
{
	size_t newLen = 0;
	size_t i;
	std::string retVal;

	for (i = 0; string[i]; i++)
	{
		CharStringMap::const_iterator it = sm_replacementChars.find(string[i]);

		if (it != sm_replacementChars.end())
		{
			newLen += it->second.size();
		}
		else
		{
			newLen++;
		}
	}
	retVal.reserve(newLen);
	for (i = 0; string[i]; i++)
	{
		CharStringMap::const_iterator it = sm_replacementChars.find(string[i]);

		if (it != sm_replacementChars.end())
		{
			retVal += it->second;
		}
		else
		{
			retVal += string[i];
		}
	}
	return retVal;
}

std::string LDPovExporter::getDeclareName(
	const std::string &modelFilename,
	bool mirrored)
{
	StringStringMap::const_iterator it;
	std::string key;

	if (mirrored)
	{
		key = modelFilename + ":mirror";
	}
	else
	{
		key = modelFilename;
	}
	it = m_declareNames.find(lowerCaseString(key));
	if (it != m_declareNames.end())
	{
		return it->second;
	}
	std::string replaced = replaceSpecialChacters(modelFilename.c_str());
	std::string retValue;

	convertStringToLower(&replaced[0]);
	if (isdigit(replaced[0]))
	{
		retValue = "_";
		retValue += replaced;
	}
	else
	{
		retValue = replaced;
	}
	if (mirrored)
	{
		retValue += "_mirror";
	}
	m_declareNames[lowerCaseString(key)] = retValue;
	return retValue;
}

bool LDPovExporter::scanModelColors(LDLModel *pModel)
{
	LDLFileLineArray *fileLines = pModel->getFileLines();
	std::string declareName = getDeclareName(pModel, false);

	if (m_processedModels[declareName])
	{
		return true;
	}
	m_processedModels[declareName] = true;
	if (fileLines)
	{
		int i;
		int count = pModel->getActiveLineCount();

		for (i = 0; i < count; i++)
		{
			LDLFileLine *pFileLine = (*fileLines)[i];

			if (pFileLine->getLineType() == LDLLineTypeModel)
			{
				LDLModelLine *pModelLine = (LDLModelLine *)pFileLine;
				LDLModel *pModel = pModelLine->getModel(true);

				if (pModel)
				{
					m_colorsUsed[pModelLine->getColorNumber()] = true;
					scanModelColors(pModel);
				}
			}
			else if (pFileLine->isShapeLine())
			{
				LDLShapeLine *pShapeLine = (LDLShapeLine *)pFileLine;

				if (pShapeLine->getLineType() != LDLLineTypeLine &&
					pShapeLine->getLineType() != LDLLineTypeConditionalLine)
				{
					m_colorsUsed[pShapeLine->getColorNumber()] = true;
				}
			}
		}
	}
	return true;
}

bool LDPovExporter::writeModelColors(void)
{
	for (IntBoolMap::const_iterator it = m_colorsUsed.begin();
		it != m_colorsUsed.end(); it++)
	{
		writeColorDeclaration(it->first);
	}
	m_processedModels.clear();
	return 1;
}

bool LDPovExporter::writeEdges(void)
{
	if (m_edgePoints.size() > 0)
	{
		fprintf(m_pPovFile,
			"#declare Edges = union\n"
			"{\n");
		for (VectorList::const_iterator it = m_edgePoints.begin();
			it != m_edgePoints.end(); it++)
		{
			const TCVector &point1 = *it;

			it++;
			if (it != m_edgePoints.end())
			{
				const TCVector &point2 = *it;

				fprintf(m_pPovFile,
					"	EdgeLine(");
				writePoint(point1);
				fprintf(m_pPovFile, ",");
				writePoint(point2);
				fprintf(m_pPovFile, ",EdgeColor)\n");
			}
		}
		fprintf(m_pPovFile, "}\n\n");
	}
	return true;
}

bool LDPovExporter::writeModel(LDLModel *pModel, const TCFloat *matrix)
{
	LDLFileLineArray *fileLines = pModel->getFileLines();
	bool mirrored = m_unmirrorStuds &&
		TCVector::determinant(matrix) < 0.0f && pModel->hasStuds();
	std::string declareName = getDeclareName(pModel, mirrored);

	if (m_processedModels[declareName] || m_emptyModels[declareName])
	{
		return true;
	}
	m_processedModels[declareName] = true;
	if (!findModelInclude(pModel))
	{
		if (fileLines)
		{
			int count = pModel->getActiveLineCount();

			for (int i = 0; i < count; i++)
			{
				LDLFileLine *pFileLine = (*fileLines)[i];

				if (pFileLine->getLineType() == LDLLineTypeModel)
				{
					LDLModelLine *pModelLine = (LDLModelLine *)pFileLine;
					LDLModel *pModel = pModelLine->getModel(true);

					if (pModel != NULL)
					{
						TCFloat newMatrix[16];

						TCVector::multMatrix(matrix, pModelLine->getMatrix(),
							newMatrix);
						writeModel(pModel, newMatrix);
					}
				}
			}
			return writeModelObject(pModel, mirrored, matrix);
		}
	}
	return true;
}

// NOTE: static function
void LDPovExporter::cleanupFloats(TCFloat *array, int count /*= 16*/)
{
	int i;

	for (i = 0; i < count; i++)
	{
		if (fabs(array[i]) < 1e-6)
		{
			array[i] = 0.0f;
		}
	}
}

// NOTE: static function
void LDPovExporter::cleanupDoubles(double *array, int count /*= 16*/)
{
	int i;

	for (i = 0; i < count; i++)
	{
		if (fabs(array[i]) < 1e-9)
		{
			array[i] = 0.0f;
		}
	}
}

void LDPovExporter::getCameraString(char *&povCamera)
{
	TCFloat tmpMatrix[16];
	TCFloat matrix[16];
	TCFloat centerMatrix[16];
	TCFloat positionMatrix[16];
	TCFloat cameraMatrix[16];
	TCFloat otherMatrix[16] = {1,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,1};
	char locationString[1024];
	char lookAtString[1204];
	char upString[1024];
	TCVector directionVector = TCVector(0.0f, 0.0f, 1.0f);
	TCVector locationVector;
	TCVector lookAtVector;
	TCVector upVector = TCVector(0.0f, -1.0f, 0.0f);
	double direction[3];
	double up[3];
	double location[3];
	LDLFacing facing;
	char cameraString[4096];
	double lookAt[3];
	double tempV[3];
	std::string aspectRatio = getAspectRatio();

	TCVector cameraPosition = m_camera.getPosition();

	TCVector::initIdentityMatrix(positionMatrix);
	positionMatrix[12] = cameraPosition[0] - m_xPan;
	positionMatrix[13] = -cameraPosition[1] + m_yPan;
	positionMatrix[14] = -cameraPosition[2];
	TCVector::initIdentityMatrix(centerMatrix);
	centerMatrix[12] = m_center[0];
	centerMatrix[13] = m_center[1];
	centerMatrix[14] = m_center[2];
	TCVector::multMatrix(otherMatrix, m_rotationMatrix, tmpMatrix);
	TCVector::invertMatrix(tmpMatrix, cameraMatrix);
	TCVector::multMatrix(centerMatrix, cameraMatrix, tmpMatrix);
	TCVector::multMatrix(tmpMatrix, positionMatrix, matrix);

	facing = m_camera.getFacing();
	facing[0] = -facing[0];
	facing.getInverseMatrix(cameraMatrix);
	TCVector::multMatrix(matrix, cameraMatrix, tmpMatrix);
	memcpy(matrix, tmpMatrix, sizeof(matrix));
	cleanupFloats(matrix);
	locationVector = TCVector(matrix[12], matrix[13], matrix[14]);
	location[0] = (double)matrix[12];
	location[1] = (double)matrix[13];
	location[2] = (double)matrix[14];
	cleanupFloats(matrix);
	// Note that the location accuracy isn't nearly as important as the
	// directional accuracy, so we don't have to re-do this string prior
	// to putting it on the clipboard in the POV code copy.
	sprintf(locationString, "%s,%s,%s", ftostr(location[0]).c_str(),
		ftostr(location[1]).c_str(), ftostr(location[2]).c_str());

	matrix[12] = matrix[13] = matrix[14] = 0.0f;
	directionVector = directionVector.transformPoint(matrix);
	upVector = upVector.transformPoint(matrix);
	// Grab the values prior to normalization.  That will make the
	// normalization more accurate in double precision.
	directionVector.upConvert(direction);
	lookAtVector = locationVector + directionVector *
		locationVector.length();
	upVector.upConvert(up);
	directionVector = directionVector.normalize();
	upVector = upVector.normalize();
	cleanupFloats(directionVector, 3);
	cleanupFloats(upVector, 3);
	TCVector::doubleNormalize(up);
	TCVector::doubleNormalize(direction);
	TCVector::doubleMultiply(direction, tempV,
		TCVector::doubleLength(location));
	TCVector::doubleAdd(location, tempV, lookAt);
	// Re-do the strings with higher accuracy, so they'll be
	// accepted by POV-Ray.
	sprintf(upString, "%s,%s,%s", ftostr(up[0], 20).c_str(),
		ftostr(up[1], 20).c_str(), ftostr(up[2], 20).c_str());
	sprintf(lookAtString, "%s,%s,%s", ftostr(lookAt[0], 20).c_str(),
		ftostr(lookAt[1], 20).c_str(), ftostr(lookAt[2], 20).c_str());
	sprintf(cameraString,
		"camera {\n"
		"\t#declare ASPECT = %s;\n"
		"\tlocation < %s >\n"
		"\tsky < %s >\n"
		"\tright ASPECT * < -1,0,0 >\n"
		"\tlook_at < %s >\n"
		"\tangle %s\n"
		"}\n",
		aspectRatio.c_str(), locationString, upString, lookAtString,
		ftostr(getHFov()).c_str());
	povCamera = copyString(cameraString);
}

void LDPovExporter::writeLight(TCFloat lat, TCFloat lon, TCFloat radius)
{
	TCVector lightVector(0.0f, 0.0f, radius);
	TCVector lightLoc;
	TCFloat latMatrix[16];
	TCFloat lonMatrix[16];
	TCFloat lightMatrix[16];
	TCFloat tempMatrix[16];
	TCFloat flipMatrix[16];
	TCFloat latRad = (TCFloat)deg2rad(lat);
	TCFloat lonRad = (TCFloat)deg2rad(-lon);

	TCVector::initIdentityMatrix(latMatrix);
	TCVector::initIdentityMatrix(lonMatrix);
	TCVector::initIdentityMatrix(flipMatrix);
	latMatrix[5] = (TCFloat)cos(latRad);
	latMatrix[6] = (TCFloat)-sin(latRad);
	latMatrix[9] = (TCFloat)sin(latRad);
	latMatrix[10] = (TCFloat)cos(latRad);
	lonMatrix[0] = (TCFloat)cos(lonRad);
	lonMatrix[2] = (TCFloat)sin(lonRad);
	lonMatrix[8] = (TCFloat)-sin(lonRad);
	lonMatrix[10] = (TCFloat)cos(lonRad);
	flipMatrix[5] = -1.0f;
	flipMatrix[10] = -1.0f;
	TCVector::multMatrix(lonMatrix, latMatrix, tempMatrix);
	TCVector::multMatrix(flipMatrix, tempMatrix, lightMatrix);
	lightVector.transformPoint(lightMatrix, lightLoc);
	lightLoc += m_center;
	fprintf(m_pPovFile,
		"light_source {\n"
		"	<%.6g,%.6g,%.6g>	// Latitude,Longitude,Radius: %.6g,%.6g,%.6g\n"
		"	color rgb <1,1,1>\n"
		"}\n", lightLoc[0], lightLoc[1], lightLoc[2], lat, lon, radius);
}

bool LDPovExporter::writeLights(void)
{
	fprintf(m_pPovFile, "// Lights\n");
	writeLight(45.0, 0.0, m_radius * 2.0f);
	writeLight(30.0, 120.0, m_radius * 2.0f);
	writeLight(60.0, -120.0, m_radius * 2.0f);
	//writeLight(45.0, 0.0, m_radius * 2.0f);
	//writeLight(30.0, 120.0, m_radius * 2.0f);
	//writeLight(60.0, -120.0, m_radius * 2.0f);
	return true;
}

bool LDPovExporter::writeCamera(void)
{
	char *cameraString;

	getCameraString(cameraString);
	fprintf(m_pPovFile, "// Camera\n");
	fprintf(m_pPovFile, "%s\n", cameraString);
	delete cameraString;
	return true;
}

std::string LDPovExporter::findInclude(const std::string &filename)
{
	for (StringList::const_iterator it = m_searchPath.begin();
		it != m_searchPath.end(); it++)
	{
		std::string path = *it + filename;
		struct stat statData;

		if (stat(path.c_str(), &statData) == 0)
		{
			if ((statData.st_mode & S_IFDIR) == 0)
			{
				return path;
			}
		}
	}
	return "";
}

bool LDPovExporter::writeInclude(
	const std::string &filename,
	bool lineFeed /*= true*/,
	const LDLModel *pModel /*= NULL*/)
{
	if (m_includes.find(filename) == m_includes.end())
	{
		StringStringMap::iterator it = m_includeVersions.find(filename);
		std::string version;

		if (it != m_includeVersions.end())
		{
			version = it->second;
		}
		if (version.size() > 0)
		{
			fprintf(m_pPovFile, "#if (version > %s) #version %s; #end\n",
				version.c_str(), version.c_str());
		}
		fprintf(m_pPovFile, "#include \"%s\"", filename.c_str());
		if (pModel)
		{
			writeDescriptionComment(pModel);
		}
		else
		{
			fprintf(m_pPovFile, "\n");
		}
		if (version.size() > 0)
		{
			fprintf(m_pPovFile, "#if (version < ORIGVER) #version ORIGVER; #end");
		}
		if (lineFeed)
		{
			fprintf(m_pPovFile, "\n");
		}
		m_includes.insert(filename);
		return true;
	}
	return false;
}

std::string LDPovExporter::findMainPovName(const PovMapping &mapping)
{
	for (PovNameList::const_iterator it = mapping.names.begin();
		it != mapping.names.end(); it++)
	{
		const PovName &name = *it;

		if (name.attributes.size() == 0)
		{
			return name.name;
		}
	}
	return "";
}

const PovName *LDPovExporter::findPovName(
	const PovMapping &mapping,
	const char *attrName,
	const char *attrValue)
{
	for (PovNameList::const_iterator it = mapping.names.begin();
		it != mapping.names.end(); it++)
	{
		const PovName &name = *it;
		StringStringMap::const_iterator it2 = name.attributes.find(attrName);

		if (it2 != name.attributes.end() && it2->second == attrValue)
		{
			return &name;
		}
	}
	return NULL;
}

bool LDPovExporter::findXmlModelInclude(const LDLModel *pModel)
{
	const std::string modelFilename = getModelFilename(pModel);
	std::string key = lowerCaseString(modelFilename);

	PovElementMap::const_iterator it = m_xmlElements.find(key);
	if (it != m_xmlElements.end())
	{
		const PovElement &element = it->second;
		StringList::const_iterator itFilename;
		bool wrote = false;
		size_t i = 0;
		std::string declareFilename;

		for (itFilename = element.povFilenames.begin();
			itFilename != element.povFilenames.end(); itFilename++)
		{
			const LDLModel *pDescModel = NULL;

			if (i == element.povFilenames.size() - 1)
			{
				pDescModel = pModel;
			}
			if (writeInclude(*itFilename, true, pDescModel))
			{
				wrote = true;
			}
			i++;
		}
		declareFilename = modelFilename;
		convertStringToLower(&declareFilename[0]);
		m_declareNames[declareFilename] = findMainPovName(element);
		m_declareNames[declareFilename + ":mirror"] =
			m_declareNames[declareFilename];
		m_matrices[key] = element.matrix;
		return true;
	}
	return false;
}

bool LDPovExporter::findModelInclude(const LDLModel *pModel)
{
	const std::string &modelFilename = getModelFilename(pModel);
	if (m_xmlMap && findXmlModelInclude(pModel))
	{
		return true;
	}
	if (!m_findReplacements)
	{
		return false;
	}
	char *incFilename = copyString(modelFilename.c_str(), 20);
	char *dotSpot = strrchr(incFilename, '.');

	if (dotSpot)
	{
		*dotSpot = 0;
	}
	strcat(incFilename, ".inc");
	for (StringList::const_iterator it = m_searchPath.begin();
		it != m_searchPath.end(); it++)
	{
		std::string filename = *it + incFilename;
		FILE *pIncFile = fopen(filename.c_str(), "r");

		if (pIncFile)
		{
			char buf[1024];
			bool found = true;

			for ( ; ; )
			{
				char *spot;

				if (fgets(buf, sizeof(buf), pIncFile) == NULL)
				{
					found = false;
					break;
				}
				buf[sizeof(buf) - 1] = 0;
				stripLeadingWhitespace(buf);
				if (stringHasCaseInsensitivePrefix(buf, "#declare"))
				{
					size_t declareLen = strlen("#declare");
					memmove(buf, &buf[declareLen],
						strlen(buf) + 1 - declareLen);
					replaceStringCharacter(buf, '\t', ' ');
					// Replace all multi-spaces with single spaces.
					spot = buf;
					while ((spot = strstr(spot, "  ")) != NULL)
					{
						memmove(spot, spot + 1, strlen(spot));
					}
					if (buf[0] == ' ')
					{
						char declareName[1024];

						if (sscanf(buf, " %s", declareName) == 1)
						{
							m_declareNames[lowerCaseString(modelFilename)] =
								declareName;
							break;
						}
					}
				}
			}
			fclose(pIncFile);
			if (found)
			{
				writeInclude(incFilename, true, pModel);
				return true;
			}
		}
	}
	return false;
}

void LDPovExporter::writeDescriptionComment(const LDLModel *pModel)
{
		if (pModel->getDescription() != NULL)
		{
			fprintf(m_pPovFile," // %s\n", pModel->getDescription());
		}
		else
		{
			fprintf(m_pPovFile,"\n");
		}
}

bool LDPovExporter::findModelGeometry(
	LDLModel *pModel,
	IntShapeLineListMap &colorGeometryMap,
	bool mirrored)
{
	if (pModel == m_pTopModel)
	{
		// Even if there isn't any output, you'll get a parse error if we
		// don't spit out the object for the main model, so set haveOutput
		// to true.
		return true;
	}
	LDLFileLineArray *fileLines = pModel->getFileLines();
	int count = pModel->getActiveLineCount();
	bool retValue = false;

	for (int i = 0; i < count; i++)
	{
		LDLFileLine *pFileLine = (*fileLines)[i];

		if (pFileLine->getLineType() == LDLLineTypeModel)
		{
			LDLModelLine *pModelLine = (LDLModelLine *)pFileLine;
			LDLModel *pModel = pModelLine->getModel(true);

			if (pModel != NULL)
			{
				if (!m_emptyModels[getDeclareName(pModel, mirrored)])
				{
					retValue = true;
				}
			}
		}
		else if (pFileLine->getLineType() == LDLLineTypeTriangle ||
			pFileLine->getLineType() == LDLLineTypeQuad)
		{
			LDLShapeLine *pShapeLine = (LDLShapeLine *)pFileLine;

			colorGeometryMap[pShapeLine->getColorNumber()].push_back(
				pShapeLine);
			retValue = true;
		}
	}
	return retValue;
}

bool LDPovExporter::writeModelObject(
	LDLModel *pModel,
	bool mirrored,
	const TCFloat *matrix)
{
	if (!m_primSub || !performPrimitiveSubstitution(pModel, false))
	{
		std::string modelFilename = getModelFilename(pModel);
		LDLFileLineArray *fileLines = pModel->getFileLines();
		int i;
		int count = pModel->getActiveLineCount();
		std::string declareName = getDeclareName(pModel, mirrored);
		IntShapeLineListMap colorGeometryMap;

		if (findModelGeometry(pModel, colorGeometryMap, mirrored))
		{
			bool ifStarted = false;
			bool elseStarted = false;
			bool povMode = false;
			bool studsStarted = false;

			if (pModel->isPart())
			{
				TCVector min, max;

				pModel->getBoundingBox(min, max);
				fprintf(m_pPovFile, "#declare %s =\n", declareName.c_str());
				//writeDescriptionComment(pModel);
				fprintf(m_pPovFile,
					"#if (QUAL = 0)\n"
					"box {\n\t");
				writePoint(min);
				fprintf(m_pPovFile, ",");
				writePoint(max);
				fprintf(m_pPovFile, "\n"
					"}\n"
					"#else\n"
					"union {\n");
			}
			else
			{
				fprintf(m_pPovFile, "#declare %s = union {\n", declareName.c_str());
				//writeDescriptionComment(pModel);
			}
			for (i = 0; i < count; i++)
			{
				LDLFileLine *pFileLine = (*fileLines)[i];

				if (pFileLine->getLineType() == LDLLineTypeModel)
				{
					if (((LDLModelLine *)pFileLine)->getModel(true) != NULL)
					{
						writeModelLine((LDLModelLine *)pFileLine, studsStarted,
							mirrored, matrix);
					}
				}
				else if (pFileLine->getLineType() == LDLLineTypeComment)
				{
					endStuds(studsStarted);
					writeCommentLine((LDLCommentLine *)pFileLine, ifStarted,
						elseStarted, povMode);
				}
			}
			endStuds(studsStarted);
			if (ifStarted)
			{
				consolePrintf("0 L3P ENDPOV missing from file.\n");
			}
			writeGeometry(colorGeometryMap);
			if (pModel == m_pTopModel)
			{
				if (m_edges)
				{
					fprintf(m_pPovFile,
						"	object { Edges }\n");
				}
				fprintf(m_pPovFile,
					"#if (REFLS = 0)\n"
					"	no_reflection\n"
					"#end\n");
				fprintf(m_pPovFile,
					"#if (SHADS = 0)\n"
					"	no_shadow\n"
					"#end\n");
			}
			if (pModel->isPart())
			{
				fprintf(m_pPovFile, "}\n#end\n\n");
			}
			else
			{
				fprintf(m_pPovFile, "}\n\n");
			}
		}
		else
		{
			m_emptyModels[declareName] = true;
		}
	}
	return true;
}

void LDPovExporter::writeGeometry(const IntShapeLineListMap &colorGeometryMap)
{
	for (IntShapeLineListMap::const_iterator itMap = colorGeometryMap.begin();
		itMap != colorGeometryMap.end(); itMap++)
	{
		const ShapeLineList &list = itMap->second;

		startMesh();
		for (ShapeLineList::const_iterator itList = list.begin();
			itList != list.end(); itList++)
		{
			LDLShapeLine *shapeLine = *itList;

			if (shapeLine->getLineType() == LDLLineTypeTriangle)
			{
				writeTriangleLine((LDLTriangleLine *)shapeLine);
			}
			else if (shapeLine->getLineType() == LDLLineTypeQuad)
			{
				writeQuadLine((LDLQuadLine *)shapeLine);
			}
		}
		if (itMap->first != 16)
		{
			fprintf(m_pPovFile, "\t\t");
			writeColor(itMap->first);
			fprintf(m_pPovFile, "\n");
		}
		endMesh();
	}
}

std::string LDPovExporter::getSizeSeamString(TCFloat size)
{
	if (fEq(size, 0.0f))
	{
		return "0";
	}
	else
	{
		char buf[1024];

		sprintf(buf, "1-SW/%s", ftostr(size).c_str());
		return buf;
	}
}

std::string LDPovExporter::getOfsSeamString(TCFloat ofs, TCFloat size)
{
	if (fEq(ofs, 0.0f) || fEq(size, 0.0f))
	{
		return "0";
	}
	else
	{
		char buf[1024];

		sprintf(buf, "SW/%s", ftostr(size / ofs).c_str());
		return buf;
	}
}

void LDPovExporter::writeSeamMatrix(LDLModelLine *pModelLine)
{
	LDLModel *pModel = pModelLine->getModel(true);

	if (pModel && pModel->isPart())
	{
		TCVector min, max, size, center;

		pModel->getBoundingBox(min, max);
		size = max - min;
		center = (min + max) / 2.0f;
		fprintf(m_pPovFile, "matrix <%s,0,0,0,%s,0,0,0,%s,%s,%s,%s>\n\t\t",
			getSizeSeamString(size[0]).c_str(),
			getSizeSeamString(size[1]).c_str(),
			getSizeSeamString(size[2]).c_str(),
			getOfsSeamString(center[0], size[0]).c_str(),
			getOfsSeamString(center[1], size[1]).c_str(),
			getOfsSeamString(center[2], size[2]).c_str());
	}
}

void LDPovExporter::writeMatrix(
	TCFloat *matrix,
	const char *filename /*= NULL*/)
{
	if (filename != NULL)
	{
		std::string key = lowerCaseString(filename);
		MatrixMap::const_iterator it = m_matrices.find(key);

		if (it != m_matrices.end())
		{
			const TCFloat *mapMatrix = it->second;
			TCFloat newMatrix[16];

			TCVector::multMatrix(matrix, mapMatrix, newMatrix);
			writeMatrix(newMatrix);
			return;
		}
	}
	fprintf(m_pPovFile, "matrix <");
	for (int col = 0; col < 4; col++)
	{
		for (int row = 0; row < 3; row++)
		{
			float value = matrix[col * 4 + row];

			if (row == 0 && col == 0)
			{
				fprintf(m_pPovFile, "%s", ftostr(value).c_str());
			}
			else
			{
				fprintf(m_pPovFile, ",%s", ftostr(value).c_str());
			}
		}
	}
	fprintf(m_pPovFile, ">");
}

bool LDPovExporter::writeColor(int colorNumber, bool slope)
{
	if (colorNumber != 16)
	{
		fprintf(m_pPovFile,
			"\t#if (version >= 3.1) material #else texture #end { Color%d%s }",
			colorNumber, slope ? "_slope" : "");
		return true;
	}
	return false;
}

void LDPovExporter::writeInnerColorDeclaration(
	int colorNumber,
	bool slope)
{
	if (colorNumber != 16)
	{
		int r, g, b, a;
		PovColorMap::const_iterator it = m_xmlColors.end();
		LDLPalette *pPalette = m_pTopModel->getMainModel()->getPalette();
		LDLColorInfo colorInfo;

		pPalette->getRGBA(colorNumber, r, g, b, a);
		colorInfo = pPalette->getAnyColorInfo(colorNumber);
		if (m_xmlMap)
		{
			it = m_xmlColors.find(colorNumber);
		}
		if (!slope)
		{
			fprintf(m_pPovFile, "\n");
		}
		fprintf(m_pPovFile,
			"#declare Color%d%s = #if (version >= 3.1) material { #end\n\ttexture {\n",
			colorNumber, slope ? "_slope" : "");
		if (it != m_xmlColors.end())
		{
			fprintf(m_pPovFile, "\t\t%s\n",
				it->second.names.front().name.c_str());
			if (slope)
			{
				fprintf(m_pPovFile, "\t\t#if (QUAL > 1) normal { bumps 0.3 scale 25*0.02 } #end\n");
			}
			fprintf(m_pPovFile, "\t}\n");
		}
		else
		{
			fprintf(m_pPovFile, "\t\tpigment { ");
			if (a != 255)
			{
				fprintf(m_pPovFile, "#if (QUAL > 1) ");
			}
			writeRGBA(r, g, b, a);
			if (a != 255)
			{
				fprintf(m_pPovFile, " #else ");
				writeRGBA(r, g, b, 255);
				fprintf(m_pPovFile, " #end");
			}
			fprintf(m_pPovFile, " }\n");
			fprintf(m_pPovFile, "#if (QUAL > 1)\n");
			fprintf(m_pPovFile, "\t\tfinish { ambient AMB diffuse DIF }\n");
			if (a == 255)
			{
				if (colorInfo.rubber)
				{
					fprintf(m_pPovFile, "\t\tfinish { phong RUBBER_PHONG phong_size RUBBER_PHONGS reflection RUBBER_REFL ");
				}
				else
				{
					fprintf(m_pPovFile, "\t\tfinish { phong PHONG phong_size PHONGS reflection ");
					if (colorInfo.chrome)
					{
						fprintf(m_pPovFile, "CHROME_REFL brilliance CHROME_BRIL metallic specular CHROME_SPEC roughness CHROME_ROUGH");
					}
					else
					{
						fprintf(m_pPovFile, "REFL ");
					}
				}
				fprintf(m_pPovFile, "}\n");
			}
			else
			{
				fprintf(m_pPovFile, "\t\tfinish { phong PHONG phong_size PHONGS reflection TREFL }\n");
			}
			if (a != 255)
			{
				fprintf(m_pPovFile, "\t\t#if (version >= 3.1) #else finish { refraction 1 ior IOR } #end\n");
			}
			fprintf(m_pPovFile, "#end\n");
			fprintf(m_pPovFile, "\t}\n");
			if (a != 255)
			{
				fprintf(m_pPovFile, "#if (version >= 3.1) #if (QUAL > 1)\n");
				fprintf(m_pPovFile, "\tinterior { ior IOR }\n");
				fprintf(m_pPovFile, "#end #end\n");
			}
		}
		fprintf(m_pPovFile, "#if (version >= 3.1) } #end\n");
	}
}

void LDPovExporter::writeColorDeclaration(int colorNumber)
{
	if (colorNumber != 16)
	{
		int r, g, b, a;
		LDLPalette *pPalette = m_pTopModel->getMainModel()->getPalette();
		LDLColorInfo colorInfo;
		PovColorMap::const_iterator it = m_xmlColors.end();

		pPalette->getRGBA(colorNumber, r, g, b, a);
		if (m_xmlMap)
		{
			it = m_xmlColors.find(colorNumber);
			if (it != m_xmlColors.end())
			{
				PovMapping color = it->second;
				for (StringList::const_iterator it2 = color.povFilenames.begin();
					it2 != color.povFilenames.end(); it2++)
				{
					writeInclude(*it2);
				}
			}
		}
		fprintf(m_pPovFile, "#ifndef (Color%d)", colorNumber);
		colorInfo = pPalette->getAnyColorInfo(colorNumber);
		if (colorInfo.name[0])
		{
			fprintf(m_pPovFile, " // %s", colorInfo.name);
		}
		writeInnerColorDeclaration(colorNumber, false);
		if (it != m_xmlColors.end())
		{
			writeInnerColorDeclaration(colorNumber, true);
		}
		fprintf(m_pPovFile, "#end\n\n");
	}
}

// Note: static function
double LDPovExporter::alphaMod(int color)
{
	if (color > 127)
	{
		return (255.0 - ((255.0 - color) * .1)) / 255.0;
	}
	else
	{
		return color * 1.5 / 255.0;
		//return color / 255.0;
	}
}

void LDPovExporter::writeRGBA(int r, int g, int b, int a)
{
	const char *filter = "0";
	double dr, dg, db;

	if (a != 255)
	{
		filter = "TFILT";
		dr = alphaMod(r);
		dg = alphaMod(g);
		db = alphaMod(b);
	}
	else
	{
		dr = r / 255.0;
		dg = g / 255.0;
		db = b / 255.0;
	}
	fprintf(m_pPovFile, "rgbf <%s,%s,%s,%s>", ftostr(dr).c_str(),
		ftostr(dg).c_str(), ftostr(db).c_str(), filter);
}

void LDPovExporter::writeCommentLine(
	LDLCommentLine *pCommentLine,
	bool &ifStarted,
	bool &elseStarted,
	bool &povMode)
{
	const char *comment = pCommentLine->getProcessedLine();

	if (stringHasCaseInsensitivePrefix(comment, "0 L3P IFPOV"))
	{
		if (ifStarted)
		{
			consolePrintf("Found 0 L3P IFPOV inside a 0 L3P IF%sPOV section; "
				"ignoring.\n", povMode ? "" : "NOT");
		}
		else
		{
			ifStarted = true;
			elseStarted = false;
			povMode = true;
			fprintf(m_pPovFile, "#if (IPOV)\n");
		}
	}
	else if (stringHasCaseInsensitivePrefix(comment, "0 L3P IFNOTPOV"))
	{
		if (ifStarted)
		{
			consolePrintf("Found 0 L3P IFNOTPOV inside a 0 L3P IF%sPOV section;"
				" ignoring.\n", povMode ? "" : "NOT");
		}
		else
		{
			ifStarted = true;
			elseStarted = false;
			povMode = false;
			fprintf(m_pPovFile, "#if (!IPOV)\n");
		}
	}
	else if (stringHasCaseInsensitivePrefix(comment, "0 L3P ELSEPOV"))
	{
		if (elseStarted)
		{
			consolePrintf("Found 0 L3P ELSEPOV inside a 0 L3P ELSEPOV section; "
				"ignoring.\n");
		}
		else if (ifStarted)
		{
			povMode = !povMode;
			elseStarted = true;
			fprintf(m_pPovFile, "#else\n");
		}
		else
		{
			consolePrintf("Found 0 L3P ELSEPOV without being in a 0 L3P IFPOV "
				"or 0 L3P IFNOTPOV section;\nignoring.\n");
		}
	}
	else if (stringHasCaseInsensitivePrefix(comment, "0 L3P ENDPOV"))
	{
		if (ifStarted)
		{
			ifStarted = false;
			fprintf(m_pPovFile, "#end\n");
		}
		else
		{
			consolePrintf("Found 0 L3P ENDPOV without being in a 0 L3P IFPOV or"
				" 0 L3P IFNOTPOV section;\nignoring.\n");
		}
	}
	else if (ifStarted)
	{
		if (povMode)
		{
			char *povLine = copyString(
				strchr(pCommentLine->getLine(), '0') + 1);

			if (povLine[0] == ' ')
			{
				memmove(povLine, &povLine[1], strlen(&povLine[1]) + 1);
			}
			stripTrailingWhitespace(povLine);
			fprintf(m_pPovFile, "%s\n", povLine);
			delete povLine;
		}
	}
	else if (strlen(comment) > 1)
	{
		// Note the processed line will always have a length of 1 on empty
		// comments.  No point outputting them.
		char *line = copyString(strchr(pCommentLine->getLine(), '0') + 1);

		stripLeadingWhitespace(line);
		stripTrailingWhitespace(line);
		fprintf(m_pPovFile, "// %s\n", line);
		delete line;
	}
}

bool LDPovExporter::isStud(LDLModel *pModel)
{
	if (pModel->isPrimitive())
	{
		const char *modelName = pModel->getName();

		if (modelName == NULL)
		{
			modelName = "";
		}
		if (stringHasCaseInsensitivePrefix(modelName, "stu"))
		{
			return true;
		}
	}
	return false;
}

bool LDPovExporter::writeModelLine(
	LDLModelLine *pModelLine,
	bool &studsStarted,
	bool mirrored,
	const TCFloat *matrix)
{
	LDLModel *pModel = pModelLine->getModel(true);
	TCFloat newModelMatrix[16] = { 0.0f };

	if (m_unmirrorStuds && pModelLine->getModel(true)->hasStuds())
	{
		TCVector::multMatrix(matrix, pModelLine->getMatrix(), newModelMatrix);
		if (TCVector::determinant(newModelMatrix) < 0.0f)
		{
			mirrored = true;
		}
		else
		{
			mirrored = false;
		}
	}
	else
	{
		mirrored = false;
	}
	if (pModel)
	{
		std::string declareName = getDeclareName(pModel, mirrored);
		PovElementMap::const_iterator it = m_xmlElements.end();

		if (m_emptyModels[declareName])
		{
			return false;
		}
		if (isStud(pModel))
		{
			startStuds(studsStarted);
		}
		else
		{
			endStuds(studsStarted);
		}
		if (m_xmlMap)
		{
			const std::string modelFilename = getModelFilename(pModel);
			std::string key = modelFilename;

			it = m_xmlElements.find(lowerCaseString(key));
			if (it != m_xmlElements.end() &&
				pModel->colorNumberIsTransparent(pModelLine->getColorNumber()))
			{
				const PovName *name = findPovName(it->second, "Alternate",
					"Clear");

				if (name != NULL)
				{
					declareName = name->name;
				}
			}
		}
		writeInnerModelLine(declareName, pModelLine, mirrored, false,
			studsStarted);
		if (it != m_xmlElements.end())
		{
			const PovName *name = findPovName(it->second, "Texture", "Slope");

			if (name != NULL)
			{
				writeInnerModelLine(name->name, pModelLine, mirrored, true,
					studsStarted);
			}
		}
	}
	return true;
}

void LDPovExporter::indentStud(bool studsStarted)
{
	if (studsStarted)
	{
		fprintf(m_pPovFile, "\t");
	}
}

void LDPovExporter::writeInnerModelLine(
	const std::string &declareName,
	LDLModelLine *pModelLine,
	bool mirrored,
	bool slope,
	bool studsStarted)
{
	LDLModel *pModel = pModelLine->getModel(true);
	bool origMirrored = mirrored;

	indentStud(studsStarted);
	fprintf(m_pPovFile, "\tobject {\n");
	indentStud(studsStarted);
	fprintf(m_pPovFile, "\t\t%s\n", declareName.c_str());
	indentStud(studsStarted);
	fprintf(m_pPovFile, "\t\t");
	writeSeamMatrix(pModelLine);
	if (origMirrored &&
		stringHasCaseInsensitiveSuffix(pModel->getFilename(), "stud.dat"))
	{
		float mirrorMatrix[] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, -1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};
		TCFloat newStudMatrix[16];

		TCVector::multMatrix(pModelLine->getMatrix(), mirrorMatrix,
			newStudMatrix);
		writeMatrix(newStudMatrix, getModelFilename(pModel).c_str());
	}
	else
	{
		writeMatrix(pModelLine->getMatrix(),
			getModelFilename(pModel).c_str());
	}
	fprintf(m_pPovFile, "\n\t");
	indentStud(studsStarted);
	if (writeColor(pModelLine->getColorNumber(), slope))
	{
		fprintf(m_pPovFile, "\n");
		indentStud(studsStarted);
		fprintf(m_pPovFile, "\t");
	}
	fprintf(m_pPovFile, "}\n");
}

void LDPovExporter::endMesh()
{
	fprintf(m_pPovFile, "\t}\n");
}

void LDPovExporter::startStuds(bool &started)
{
	if (!started)
	{
		fprintf(m_pPovFile, "\t#if (STUDS)\n");
		started = true;
	}
}

void LDPovExporter::endStuds(bool &started)
{
	if (started)
	{
		fprintf(m_pPovFile, "\t#end // STUDS\n");
		started = false;
	}
}

void LDPovExporter::startMesh(void)
{
	fprintf(m_pPovFile, "\tmesh {\n");
}

void LDPovExporter::writeTriangleLine(LDLTriangleLine *pTriangleLine)
{
	writeTriangle(pTriangleLine->getPoints());
}

void LDPovExporter::writeQuadLine(LDLQuadLine *pQuadLine)
{
	writeTriangle(pQuadLine->getPoints());
	writeTriangle(pQuadLine->getPoints(), 4, 2);
}

void LDPovExporter::writeEdgeColor(void)
{
	fprintf(m_pPovFile,
		"#ifndef (EdgeColor)\n"
		"#declare EdgeColor = material\n"
		"{\n"
		"	texture\n"
		"	{\n"
		"		pigment { rgbf <.1,.1,.1,0> }\n"
		"		finish { ambient 1 diffuse 0 }\n"
		"	}\n"
		"}\n"
		"#end\n\n");
}

void LDPovExporter::writeEdgeLineMacro(void)
{
	fprintf(m_pPovFile,
		"#macro EdgeLine(Point1, Point2, Color)\n"
		"	object\n"
		"	{\n"
		"		merge\n"
		"		{\n"
		"			cylinder\n"
		"			{\n"
		"				Point1,Point2,EDGERAD\n"
		"			}\n"
		"			sphere\n"
		"			{\n"
		"				Point1,EDGERAD\n"
		"			}\n"
		"			sphere\n"
		"			{\n"
		"				Point2,EDGERAD\n"
		"			}\n"
		"		}\n"
		"		material { Color }\n"
		"		no_shadow\n"
		"	}\n"
		"#end\n\n");
}

void LDPovExporter::writeTriangle(
	const TCVector *points,
	int size /*= -1*/,
	int start /*= 0*/)
{
	fprintf(m_pPovFile, "\t\ttriangle { ");
	writePoints(points, 3, size, start);
	fprintf(m_pPovFile, " }\n");
}

void LDPovExporter::writePoints(
	const TCVector *points,
	int count,
	int size /*= -1*/,
	int start /*= 0*/)
{
	if (size == -1)
	{
		size = count;
	}
	for (int i = 0; i < count; i++)
	{
		if (i > 0)
		{
			fprintf(m_pPovFile, " ");
		}
		writePoint(points[(i + start) % size]);
	}
}

void LDPovExporter::writePoint(const TCVector &point)
{
	fprintf(m_pPovFile, "<%s,%s,%s>", ftostr(point[0]).c_str(),
		ftostr(point[1]).c_str(), ftostr(point[2]).c_str());
}

void LDPovExporter::scanEdgePoint(
	const TCVector &point,
	const LDLFileLine *pFileLine)
{
	if (pFileLine->getLineType() == LDLLineTypeLine)
	{
		m_edgePoints.push_back(point);
	}
	else
	{
	}
}

// NOTE: static function
const char *LDPovExporter::get48Prefix(bool is48)
{
	if (is48)
	{
		return "_48_slash";
	}
	else
	{
		return "";
	}
}

bool LDPovExporter::writeRoundClipRegion(TCFloat fraction, bool closeOff)
{
	// If fraction is 1.0, this function just returns true.  If fraction is
	// greater than 1.0, it just returns false.  Otherwise, it outputs a
	// clipped_by statement that includes 1 or two planes, either in union
	// (explicitly) or in intersection (implicit to the clipped_by statement).
	// Note that planes are specified with a surface normal, and a distance
	// from the origin along that surface normal.  The surface normal indicates
	// the direction of the "top" side of the plane, and the plane isn't really
	// a plane in the since of a 2D slice.  It's a solid object, and everything
	// "under" it is considered part of it.
	if (fraction > 1.0f)
	{
		return false;
	}
	else if (fEq(fraction, 0.5f))
	{
		// We want half, so spit out a plane with a surface normal pointing
		// along the negative Z axis, such that the plane's Z coordinates are
		// all 0.
		fprintf(m_pPovFile,
			"	clipped_by\n"
			"	{\n"
			"		plane\n"
			"		{\n"
			"			<0,0,-1>,0\n"
			"		}\n");
		if (closeOff)
		{
			fprintf(m_pPovFile, "	}\n");
		}
	}
	else if (fEq(fraction, 1.0f))
	{
		if (!closeOff)
		{
			fprintf(m_pPovFile,
				"	clipped_by\n"
				"	{\n");
		}
	}
	else
	{
		double angle = 2.0 * M_PI * fraction + M_PI / 2.0;
		double x = cos(angle);
		double z = sin(angle);

		cleanupDoubles(&x, 1);
		cleanupDoubles(&z, 1);
		if (fraction < 0.5f)
		{
			// Spit out two planes.  The clipped_by statement automatically
			// takes the CSG intersection of the two.  The first plane restricts
			// us to Z >= 0 (just like the fraction=0.5 case above).  The second
			// plane uses fraction to determine the angle needed for the proper
			// pie slice.
			fprintf(m_pPovFile,
				"	clipped_by\n"
				"	{\n"
				"		plane\n"
				"		{\n"
				"			<0,0,-1>,0\n"
				"		}\n"
				"		plane\n"
				"		{\n"
				"			<%s,0,%s>,0\n"
				"		}\n", ftostr(x, 20).c_str(), ftostr(z, 20).c_str());
		}
		else
		{
			// Spit out a union of two planes.  The first plane makes it so that
			// everything with Z >= 0 is kept, so the first half of the circular
			// item is all present.  The second plane restricts the remaining
			// portion using fraction to determine the proper angle.
			fprintf(m_pPovFile,
				"	clipped_by\n"
				"	{\n"
				"		union\n"
				"		{\n"
				"			plane\n"
				"			{\n"
				"				<0,0,-1>,0\n"
				"			}\n"
				"			plane\n"
				"			{\n"
				"				<%s,0,%s>,0\n"
				"			}\n"
				"		}\n", ftostr(x, 20).c_str(), ftostr(z, 20).c_str());
		}
		if (closeOff)
		{
			fprintf(m_pPovFile, "	}\n");
		}
	}
	return true;
}

bool LDPovExporter::substituteEighthSphere(
	bool /*bfc*/,
	bool is48 /*= false*/)
{
	const char *prefix48 = get48Prefix(is48);

	fprintf(m_pPovFile,
		"#declare %s_1_dash_8sphe_dot_dat = sphere // Sphere .125\n"
		"{\n"
		"	<0,0,0>,1\n"
		"	clipped_by\n"
		"	{\n"
		"		box\n"
		"		{\n"
		"			<0,0,0>,<2,2,2>\n"
		"		}\n"
		"	}\n"
		"}\n\n", prefix48);
	return true;
}

bool LDPovExporter::substituteEighthSphereCorner(
	bool /*bfc*/,
	bool is48 /*= false*/)
{
	const char *prefix48 = get48Prefix(is48);

	fprintf(m_pPovFile,
		"#declare %s_1_dash_8sphc_dot_dat = sphere // Sphere Corner .125\n"
		"{\n"
		"	<0,0,0>,sqrt(2)\n"
		"	clipped_by\n"
		"	{\n"
		"		box\n"
		"		{\n"
		"			<0,0,0>,<1,1,1>\n"
		"		}\n"
		"	}\n"
		"}\n\n", prefix48);
	return true;
}

bool LDPovExporter::substituteCylinder(
	TCFloat fraction,
	bool /*bfc*/,
	bool is48 /*= false*/)
{
	const char *prefix48 = get48Prefix(is48);

	fprintf(m_pPovFile,
		"#declare %s_%d_dash_%dcyli_dot_dat = cylinder // Cylinder %s\n"
		"{\n"
		"	<0,0,0>,<0,1,0>,1 open\n", prefix48, m_filenameNumerator,
		m_filenameDenom, ftostr(fraction).c_str());
	writeRoundClipRegion(fraction);
	fprintf(m_pPovFile, "}\n\n");
	return true;
}

bool LDPovExporter::substituteSlopedCylinder(
	TCFloat fraction,
	bool /*bfc*/,
	bool is48 /*= false*/)
{
	const char *prefix48 = get48Prefix(is48);

	fprintf(m_pPovFile,
		"#declare %s_%d_dash_%dcyls_dot_dat = cylinder // Sloped Cylinder %s\n"
		"{\n"
		"	<0,0,0>,<0,2,0>,1 open\n", prefix48, m_filenameNumerator,
		m_filenameDenom, ftostr(fraction).c_str());
	writeRoundClipRegion(fraction, false);
	fprintf(m_pPovFile,
		"		plane\n"
		"		{\n"
		"			<1,1,0>,%s\n"
		"		}\n"
		"	}\n", ftostr(sqrt(2.0) / 2.0, 20).c_str());
	fprintf(m_pPovFile, "}\n\n");
	return true;
}

bool LDPovExporter::substituteSlopedCylinder2(
	TCFloat fraction,
	bool /*bfc*/,
	bool is48 /*= false*/)
{
	const char *prefix48 = get48Prefix(is48);
	double angle = 2.0 * M_PI * fraction + M_PI;
	double x = cos(angle);
	double z = sin(angle);

	cleanupDoubles(&x, 1);
	cleanupDoubles(&z, 1);
	if (!fEq(fraction, 0.25) && fraction > 0.25)
	{
		return false;
	}
	fprintf(m_pPovFile,
		"#declare %s_%d_dash_%dcyls2_dot_dat = cylinder // Sloped Cylinder2 %s\n"
		"{\n"
		"	<0,0,0>,<0,1,0>,1 open\n", prefix48, m_filenameNumerator,
		m_filenameDenom, ftostr(fraction).c_str());
	fprintf(m_pPovFile,
		"	clipped_by\n"
		"	{\n"
		"		plane\n"
		"		{\n"
		"			<1,0,0>,0\n"
		"		}\n"
		"		plane\n"
		"		{\n"
		"			<%s,0,%s>,0\n"
		"		}\n"
		"		plane\n"
		"		{\n"
		"			<1,1,0>,0\n"
		"		}\n"
		"	}\n", ftostr(x, 20).c_str(), ftostr(z, 20).c_str());
	fprintf(m_pPovFile, "}\n\n");
	return true;
}

bool LDPovExporter::substituteDisc(
	TCFloat fraction,
	bool /*bfc*/,
	bool is48 /*= false*/)
{
	const char *prefix48 = get48Prefix(is48);

	fprintf(m_pPovFile,
		"#declare %s_%d_dash_%ddisc_dot_dat = disc // Disc %s\n"
		"{\n"
		"	<0,0,0>,<0,1,0>,1\n", prefix48, m_filenameNumerator,
		m_filenameDenom, ftostr(fraction).c_str());
	writeRoundClipRegion(fraction);
	fprintf(m_pPovFile, "}\n\n");
	return true;
}

bool LDPovExporter::substituteChrd(
	TCFloat fraction,
	bool /*bfc*/,
	bool is48 /*= false*/)
{
	const char *prefix48 = get48Prefix(is48);
	double angle = 2.0 * M_PI * fraction;
	double cosAngle = cos(angle);
	double sinAngle = sin(angle);
	TCVector p0;
	TCVector p1(1.0f, 0.0f, 0.0f);
	TCVector p2((TCFloat)cosAngle, 0.0f, (TCFloat)sinAngle);
	double x = -sinAngle;
	double z = cosAngle - 1.0;
	double ofs = -p0.distToLine(p1, p2);

	fprintf(m_pPovFile,
		"#declare %s_%d_dash_%dchrd_dot_dat = disc // Disc %s\n"
		"{\n"
		"	<0,0,0>,<0,1,0>,1\n"
		"	clipped_by\n"
		"	{\n"
		"		plane\n"
		"		{\n"
		"			<%s,0,%s>,%s\n"
		"		}\n"
		"	}\n"
		"}\n\n", prefix48, m_filenameNumerator, m_filenameDenom,
		ftostr(fraction).c_str(), ftostr(x, 20).c_str(), ftostr(z, 20).c_str(),
		ftostr(ofs, 20).c_str());
	return true;
}

bool LDPovExporter::substituteNotDisc(
	TCFloat fraction,
	bool /*bfc*/,
	bool is48 /*= false*/)
{
	const char *prefix48 = get48Prefix(is48);

	fprintf(m_pPovFile,
		"#declare %s_%d_dash_%dndis_dot_dat = disc // Not-Disc %s\n"
		"{\n"
		"	<0,0,0>,<0,1,0>,2,1\n", prefix48, m_filenameNumerator,
		m_filenameDenom, ftostr(fraction).c_str());
	writeRoundClipRegion(fraction, false);
	fprintf(m_pPovFile,
		"		box\n"
		"		{\n"
		"			<-1,-1,-1>,<1,1,1>\n"
		"		}\n"
		"	}\n");
	fprintf(m_pPovFile, "}\n\n");
	return true;
}

bool LDPovExporter::substituteCone(
	TCFloat fraction,
	int size,
	bool /*bfc*/,
	bool is48 /*= false*/)
{
	const char *prefix48 = get48Prefix(is48);

	fprintf(m_pPovFile,
		"#declare %s_%d_dash_%dcon%d_dot_dat = cone // Cone %s\n"
		"{\n"
		"	<0,0,0>,%d,<0,1,0>,%d open\n", prefix48, m_filenameNumerator,
		m_filenameDenom, size, ftostr(fraction).c_str(), size + 1, size);
	writeRoundClipRegion(fraction);
	fprintf(m_pPovFile, "}\n\n");
	return true;
}

bool LDPovExporter::substituteRing(
	TCFloat fraction,
	int size,
	bool /*bfc*/,
	bool is48 /*= false*/,
	bool isOld /*= false*/)
{
	const char *prefix48 = get48Prefix(is48);

	if (isOld)
	{
		fprintf(m_pPovFile,
			"#declare %sring%d_dot_dat = disc // Ring %s\n",
			prefix48, size, ftostr(fraction).c_str());
	}
	else
	{
		fprintf(m_pPovFile,
			"#declare %s_%d_dash_%drin%s%d_dot_dat = disc // Ring %s\n",
			prefix48, m_filenameNumerator, m_filenameDenom,
			size >= 10 ? "" : "g", size, ftostr(fraction).c_str());
	}
	fprintf(m_pPovFile,
		"{\n"
		"	<0,0,0>,<0,1,0>,%d,%d\n", size + 1, size);
	writeRoundClipRegion(fraction);
	fprintf(m_pPovFile, "}\n\n");
	return true;
}

bool LDPovExporter::substituteTorusIO(
	bool inner,
	TCFloat fraction,
	int size,
	bool /*bfc*/,
	bool is48 /*= false*/)
{
	const char *prefix48 = get48Prefix(is48);

	fprintf(m_pPovFile,
		"#declare %s%s = torus // Torus %s\n"
		"{\n"
		"	1,%s\n", prefix48, getDeclareName(m_modelName, false).c_str(),
		ftostr(fraction).c_str(), ftostr(getTorusFraction(size), 20).c_str());
	writeRoundClipRegion(fraction, false);
	if (inner)
	{
		fprintf(m_pPovFile,
			"		cylinder\n"
			"		{\n"
			"			<0,0,0>,<0,1,0>,1"
			"		}\n");
	}
	else
	{
		fprintf(m_pPovFile,
			"		difference\n"
			"		{\n"
			"			cylinder\n"
			"			{\n"
			"				<0,0,0>,<0,1,0>,2"
			"			}\n"
			"			cylinder\n"
			"			{\n"
			"				<0,0,0>,<0,1,0>,1"
			"			}\n"
			"		}\n");
	}
	fprintf(m_pPovFile,
		"	}\n"
		"}\n\n");
	return true;
}

bool LDPovExporter::substituteTorusQ(
	TCFloat fraction,
	int size,
	bool /*bfc*/,
	bool is48 /*= false*/)
{
	const char *prefix48 = get48Prefix(is48);

	fprintf(m_pPovFile,
		"#declare %s%s = torus // Torus %s\n"
		"{\n"
		"	1,%s\n", prefix48, getDeclareName(m_modelName, false).c_str(),
		ftostr(fraction).c_str(), ftostr(getTorusFraction(size), 20).c_str());
	writeRoundClipRegion(fraction);
	fprintf(m_pPovFile, "}\n\n");
	return true;
}

void LDPovExporter::writeLogo(void)
{
	fprintf(m_pPovFile,
		"#declare LEGOSPACE = 49.5;\n"
		"#declare ldx_stud_logo =\n"
		"union {\n"
		"	merge {\n"
		"		// L\n"
		"		sphere {<-88,0,0>,4}\n"
		"		cylinder {<-88,0,0>,<0,0,0>,4 open}\n"
		"		sphere {<0,0,0>,4}\n"
		"		cylinder {<0,0,0>,<0,0,25>,4 open}\n"
		"		sphere {<0,0,25>,4}\n"
		"		clipped_by {plane{y,0}}\n"
		"	}\n"
		"\n"
		"	merge {\n"
		"		// E\n"
		"		sphere {<-88,0,25>,4}\n"
		"		cylinder {<-88,0,25>,<-88,0,0>,4 open}\n"
		"		sphere {<-88,0,0>,4}\n"
		"		cylinder {<-88,0,0>,<0,0,0>,4 open}\n"
		"		sphere {<0,0,0>,4}\n"
		"		cylinder {<0,0,0>,<0,0,25>,4 open}\n"
		"		sphere {<0,0,25>,4}\n"
		"		sphere {<-44,0,17>,4}\n"
		"		cylinder {<-44,0,0>,<-44,0,17>,4 open}\n"
		"		clipped_by {plane{y,0}}\n"
		"		translate <0,0,LEGOSPACE>\n"
		"	}\n"
		"\n"
		"	merge {\n"
		"		// G\n"
		"		sphere {<-74,0,25>,4}\n"
		"		torus {12.5,4 clipped_by{plane{x,0}} translate<-74,0,12.5>}\n"
		"		cylinder {<-74,0,0>,<-14,0,0>,4 open}\n"
		"		torus {12.5,4 clipped_by{plane{-x,0}} translate<-14,0,12.5>}\n"
		"		cylinder {<-14,0,25>,<-44,0,25>,4 open}\n"
		"		sphere {<-44,0,25>,4}\n"
		"		cylinder {<-44,0,25>,<-44,0,17>,4 open}\n"
		"		sphere {<-44,0,17>,4}\n"
		"		clipped_by {plane{y,0}}\n"
		"		translate <0,0,LEGOSPACE*2>\n"
		"	}\n"
		"\n"
		"	merge {\n"
		"		// O\n"
		"		torus {12.5,4 clipped_by{plane{x,0}} translate<-74,0,12.5>}\n"
		"		cylinder {<-74,0,0>,<-14,0,0>,4 open}\n"
		"		torus {12.5,4 clipped_by{plane{-x,0}} translate<-14,0,12.5>}\n"
		"		cylinder {<-14,0,25>,<-74,0,25>,4 open}\n"
		"		clipped_by {plane{y,0}}\n"
		"		translate <0,0,LEGOSPACE*3>\n"
		"	}\n"
		"\n"
		"	matrix <1,0,-0.22,0,1,0,0,0,1,0,0,0>\n"
		"	scale .045\n"
		"	translate <1.85,-4,-4.45>\n"
		"}\n\n");
}

bool LDPovExporter::substituteStud(void)
{
	writeLogo();
	fprintf(m_pPovFile,
			"#declare stud_dot_dat =\n"
			"#if (QUAL <= 2)\n"
			"cylinder { <0,0,0>, <0,-4,0>, 6 }\n"
			"#else\n"
			"union {\n"
			"	cylinder { <0,0,0>, <0,-4,0>, 6 }\n"
			"	object { ldx_stud_logo }\n"
			"}\n"
			"#end\n"
			"\n");
	return true;
}
