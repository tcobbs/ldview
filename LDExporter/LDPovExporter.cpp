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

#if defined WIN32 && defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

LDPovExporter::LDPovExporter(void):
LDExporter("PovExporter/")
{
	char *ldrawDir = copyString(LDLModel::lDrawDir());

	if (ldrawDir)
	{
		StringList subDirectories;
		std::string base;

		subDirectories.push_back("/pov/p/");
		subDirectories.push_back("/pov/parts/");
		replaceStringCharacter(ldrawDir, '\\', '/');
		stripTrailingPathSeparators(ldrawDir);
		base = ldrawDir;
		delete ldrawDir;
		for (StringList::const_iterator it = subDirectories.begin();
			it != subDirectories.end(); it++)
		{
			m_searchPath.push_back(base + *it);
		}
	}
	m_findReplacements = boolForKey("FindReplacements", false);
	m_inlinePov = boolForKey("InlinePov", true);
	m_hideStuds = boolForKey("HideStuds", false);
	m_edgeRadius = floatForKey("EdgeRadius", 0.15f);
	m_quality = longForKey("Quality", 2);
}

LDPovExporter::~LDPovExporter(void)
{
}

void LDPovExporter::dealloc(void)
{
	LDExporter::dealloc();
}

int LDPovExporter::doExport(LDLModel *pTopModel)
{
	std::string filename = m_filename;

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
		if (!writeHeader())
		{
			return 1;
		}
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
		if (!writeModel(m_pTopModel))
		{
			return 1;
		}
		fprintf(m_pPovFile, "object { %s", getDeclareName(m_pTopModel).c_str());
		writeColor(7);
		fprintf(m_pPovFile, " }\n\n");
		fprintf(m_pPovFile, "background { color rgb <0, 0, 0> }\n\n");
		if (!writeCamera())
		{
			return 1;
		}
		if (!writeLights())
		{
			return 1;
		}
		fclose(m_pPovFile);
	}
	else
	{
		consolePrintf(ls(_UC("ErrorCreatingPov")));
	}
	return 0;
}

bool LDPovExporter::writeHeader(void)
{
	time_t genTime = time(NULL);
	const char *author = m_pTopModel->getAuthor();
	char *filename = filenameFromPath(m_pTopModel->getFilename());

	fprintf(m_pPovFile, "// %s %s\n", ls("PovGeneratedBy"),
		"LDExport 1.0 (C) 2007 Travis Cobbs");
	fprintf(m_pPovFile, "// %s %s\n", ls("PovSee"),
		"http://ldview.sourceforge.net/");
	fprintf(m_pPovFile, "// %s %s", ls("PovDate"), ctime(&genTime));
	if (filename != NULL)
	{
		fprintf(m_pPovFile, "// %s %s\n", ls("PovLDrawFile"), filename);
		delete filename;
	}
	if (author != NULL)
	{
		fprintf(m_pPovFile, "// %s %s\n", ls("PovLDrawAuthor"), author);
	}
	fprintf(m_pPovFile, ls("PovNote"));
	fprintf(m_pPovFile, "#declare AMB = 0.4;\n");
	fprintf(m_pPovFile, "#declare DIF = 0.4;\n");
	fprintf(m_pPovFile, "#declare REFL = 0.08;\n");
	fprintf(m_pPovFile, "#declare PHONG = 0.5;\n");
	fprintf(m_pPovFile, "#declare PHONGS = 40;\n");
	fprintf(m_pPovFile, "#declare TREFL = 0.2;\n");
	fprintf(m_pPovFile, "#declare REFRF = 0.85;\n");
	fprintf(m_pPovFile, "#declare IOR = 1.25;\n");
	fprintf(m_pPovFile, "#declare RUBBER_PHONG = 0.1;\n");
	fprintf(m_pPovFile, "#declare RUBBER_PHONGS = 10;\n");
	fprintf(m_pPovFile, "#declare RUBBER_REFL = 0;\n");
	fprintf(m_pPovFile, "#declare CHROME_REFL = 0.85;\n");
	fprintf(m_pPovFile, "#declare CHROME_BRIL = 5;\n");
	fprintf(m_pPovFile, "#declare CHROME_SPEC = 0.8;\n");
	fprintf(m_pPovFile, "#declare CHROME_ROUGH = 1/100;\n");
	fprintf(m_pPovFile, "#declare SW = %s;\t// %s\n",
		ftostr(m_seamWidth).c_str(), ls("PovSeamWidthDesc"));
	fprintf(m_pPovFile, "#declare STUDS = %d;\t// %s\n", m_hideStuds ? 0 : 1,
		ls("PovStudsDesc"));
	fprintf(m_pPovFile, "#declare IPOV = %d;\t// %s\n", m_inlinePov ? 1 : 0,
		ls("PovInlinePovDesc"));
	fprintf(m_pPovFile, "#declare QUAL = %d;\t// %s\n", m_quality,
		ls("PovQualDesc"));
	fprintf(m_pPovFile, "#declare REFLS = 1;\t// %s\n", ls("PovReflDesc"));
	fprintf(m_pPovFile, "#declare SHADS = 1;\t// %s\n", ls("PovShadDesc"));
	if (m_edges)
	{
		fprintf(m_pPovFile, "#declare EDGERAD = %s;\n",
			ftostr(m_edgeRadius).c_str());
	}
	//fprintf(m_pPovFile, "#declare O7071 = sqrt(0.5);\n");
	fprintf(m_pPovFile, "\n");

	if (m_findReplacements)
	{
		fprintf(m_pPovFile, "#include \"lpovcolordefs.inc\"\n\n");
	}
	return true;
}

std::string LDPovExporter::getModelFilename(LDLModel *pModel)
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

std::string LDPovExporter::getDeclareName(LDLModel *pModel)
{
	return getDeclareName(getModelFilename(pModel));
}

std::string LDPovExporter::getDeclareName(const std::string &modelFilename)
{
	StringStringMap::const_iterator it = m_declareNames.find(modelFilename);

	if (it != m_declareNames.end())
	{
		return it->second;
	}
	char *temp1 = stringByReplacingSubstring(modelFilename.c_str(), ".",
		"_dot_");
	char *temp2 = stringByReplacingSubstring(temp1, "-", "_dash_");
	char *temp3 = stringByReplacingSubstring(temp2, "/", "_slash_");
	char *temp4 = stringByReplacingSubstring(temp3, "\\", "_slash_");
	char *replaced = stringByReplacingSubstring(temp4, ":", "_colon_");
	std::string retValue;

	convertStringToLower(replaced);
	if (isdigit(replaced[0]))
	{
		retValue = "_";
		retValue += replaced;
	}
	else
	{
		retValue = replaced;
	}
	delete temp1;
	delete temp2;
	delete temp3;
	delete temp4;
	delete replaced;
	m_declareNames[modelFilename] = retValue;
	return retValue;
}

bool LDPovExporter::scanModelColors(LDLModel *pModel)
{
	LDLFileLineArray *fileLines = pModel->getFileLines();
	std::string declareName = getDeclareName(pModel);

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
				LDLModel *pModel = pModelLine->getModel();

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

bool LDPovExporter::writeModel(LDLModel *pModel)
{
	LDLFileLineArray *fileLines = pModel->getFileLines();
	std::string declareName = getDeclareName(pModel);

	if (m_processedModels[declareName] || m_emptyModels[declareName])
	{
		return true;
	}
	m_processedModels[declareName] = true;
	if (findInclude(getModelFilename(pModel)))
	{
		writeDescriptionComment(pModel);
	}
	else
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
					LDLModel *pModel = pModelLine->getModel();

					if (pModel)
					{
						writeModel(pModel);
					}
				}
			}
			return writeModelObject(pModel);
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
		"camera\n"
		"{\n"
		"\t#declare ASPECT = 4/3;\n"
		"\tlocation < %s >\n"
		"\tsky < %s >\n"
		"\tright ASPECT * < -1,0,0 >\n"
		"\tlook_at < %s >\n"
		"\tangle %s\n"
		"}\n",
		locationString, upString, lookAtString, ftostr(getHFov()).c_str());
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
	writeLight(45.0, -45.0, m_radius * 2.0f);
	//writeLight(45.0, 0.0, m_radius * 2.0f);
	//writeLight(30.0, 120.0, m_radius * 2.0f);
	//writeLight(60.0, -120.0, m_radius * 2.0f);
	//fprintf(m_pPovFile, "light_source {\n"
	//	"	<69,-178.853,-203.853>  // Latitude,Longitude,Radius: 45,0,245.865\n"
	//	"	color rgb <1,1,1>\n"
	//	"}\n"
	//	"light_source {\n"
	//	"	<253.399,-127.933,76.4629>  // Latitude,Longitude,Radius: 30,120,245.865\n"
	//	"	color rgb <1,1,1>\n"
	//	"}\n"
	//	"light_source {\n"
	//	"	<-37.4629,-217.926,31.4664>  // Latitude,Longitude,Radius: 60,-120,245.865\n"
	//	"	color rgb <1,1,1>\n"
	//	"}\n");
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

bool LDPovExporter::findInclude(const std::string &modelFilename)
{
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
							m_declareNames[modelFilename] = declareName;
							break;
						}
					}
				}
			}
			fclose(pIncFile);
			if (found)
			{
				fprintf(m_pPovFile, "#include \"%s\"", incFilename);
				return true;
			}
		}
	}
	return false;
}

void LDPovExporter::writeDescriptionComment(LDLModel *pModel)
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
	IntShapeLineListMap &colorGeometryMap)
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

			if (!m_emptyModels[getDeclareName(pModelLine->getModel())])
			{
				retValue = true;
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

bool LDPovExporter::writeModelObject(LDLModel *pModel)
{
	if (!m_primSub || !performPrimitiveSubstitution(pModel, false))
	{
		std::string modelFilename = getModelFilename(pModel);
		LDLFileLineArray *fileLines = pModel->getFileLines();
		int i;
		int count = pModel->getActiveLineCount();
		std::string declareName = getDeclareName(pModel);
		IntShapeLineListMap colorGeometryMap;

		if (findModelGeometry(pModel, colorGeometryMap))
		{
			bool ifStarted = false;
			bool elseStarted = false;
			bool povMode = false;
			bool studsStarted = false;

			if (pModel->isPart())
			{
				TCVector min, max;

				pModel->getBoundingBox(min, max);
				fprintf(m_pPovFile, "#declare %s =", declareName.c_str());
				writeDescriptionComment(pModel);
				fprintf(m_pPovFile,
					"#if (QUAL = 0)\n"
					"box {\n");
				writePoint(min);
				fprintf(m_pPovFile, ",");
				writePoint(max);
				fprintf(m_pPovFile, "\n"
					"}\n"
					"#else\n"
					"union ");
			}
			else
			{
				fprintf(m_pPovFile, "#declare %s = union", declareName.c_str());
				writeDescriptionComment(pModel);
			}
			fprintf(m_pPovFile, "{\n");
			for (i = 0; i < count; i++)
			{
				LDLFileLine *pFileLine = (*fileLines)[i];

				if (pFileLine->getLineType() == LDLLineTypeModel)
				{
					writeModelLine((LDLModelLine *)pFileLine, studsStarted);
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
			writeColor(itMap->first, false);
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
	LDLModel *pModel = pModelLine->getModel();

	if (pModel && pModel->isPart())
	{
		TCVector min, max, size, center;

		pModel->getBoundingBox(min, max);
		size = max - min;
		center = (min + max) / 2.0f;
		fprintf(m_pPovFile, "matrix <%s,0,0,0,%s,0,0,0,%s,%s,%s,%s> ",
			getSizeSeamString(size[0]).c_str(),
			getSizeSeamString(size[1]).c_str(),
			getSizeSeamString(size[2]).c_str(),
			getOfsSeamString(center[0], size[0]).c_str(),
			getOfsSeamString(center[1], size[1]).c_str(),
			getOfsSeamString(center[2], size[2]).c_str());
	}
}

void LDPovExporter::writeMatrix(TCFloat *matrix)
{
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

void LDPovExporter::writeColor(int colorNumber, bool preSpace /*= true*/)
{
	if (colorNumber != 16)
	{
		fprintf(m_pPovFile, "%smaterial { Color%d }", preSpace ? " " : "",
			colorNumber);
	}
}

void LDPovExporter::writeColorDeclaration(int colorNumber)
{
	if (colorNumber != 16)
	{
		int r, g, b, a;
		LDLPalette *pPalette = m_pTopModel->getMainModel()->getPalette();
		LDLColorInfo colorInfo;

		pPalette->getRGBA(colorNumber, r, g, b, a);
		fprintf(m_pPovFile, "#ifndef (Color%d)", colorNumber);
		colorInfo = pPalette->getAnyColorInfo(colorNumber);
		if (colorInfo.name[0])
		{
			fprintf(m_pPovFile, " // %s", colorInfo.name);
		}
		fprintf(m_pPovFile,
			"\n#declare Color%d = material\n{\n\ttexture\n",
			colorNumber);
		fprintf(m_pPovFile, "\t{\n");
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
		fprintf(m_pPovFile, "#end\n");
		fprintf(m_pPovFile, "\t}\n");
		if (a != 255)
		{
			fprintf(m_pPovFile, "#if (QUAL > 1)\n");
			fprintf(m_pPovFile, "\tinterior { ior IOR }\n");
			fprintf(m_pPovFile, "#end\n");
		}
		fprintf(m_pPovFile, "}\n");
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
		filter = "REFRF";
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

bool LDPovExporter::writeModelLine(LDLModelLine *pModelLine, bool &studsStarted)
{
	LDLModel *pModel = pModelLine->getModel();

	if (pModel)
	{
		std::string declareName = getDeclareName(pModel);

		if (m_emptyModels[declareName])
		{
			return false;
		}
		if (isStud(pModel))
		{
			startStuds(studsStarted);
			fprintf(m_pPovFile, "\t");
		}
		else
		{
			endStuds(studsStarted);
		}
		fprintf(m_pPovFile, "\tobject { %s ", declareName.c_str());
		writeSeamMatrix(pModelLine);
		writeMatrix(pModelLine->getMatrix());
		writeColor(pModelLine->getColorNumber());
		fprintf(m_pPovFile, " }\n");
	}
	return true;
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
	fprintf(m_pPovFile, "\tmesh\n\t{\n");
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
		"#declare %s_1_dash_8sphe_dot_dat = sphere // Sphere %.125\n"
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
		"#declare %s_1_dash_8sphc_dot_dat = sphere // Sphere Corner %.125\n"
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
		"	1,%s\n", prefix48, getDeclareName(m_modelName).c_str(),
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
		"	1,%s\n", prefix48, getDeclareName(m_modelName).c_str(),
		ftostr(fraction).c_str(), ftostr(getTorusFraction(size), 20).c_str());
	writeRoundClipRegion(fraction);
	fprintf(m_pPovFile, "}\n\n");
	return true;
}

bool LDPovExporter::substituteStud(void)
{
	fprintf(m_pPovFile,
		"#declare LEGOSPACE = 49.5;\n"
		"#declare stud_dot_dat =\n"
		"#if (QUAL <= 2)\n"
		"cylinder { <0,0,0>, <0,-4,0>, 6 }\n"
		"#else\n"
		"union {\n"
		"	cylinder { <0,0,0>, <0,-4,0>, 6 }\n"
		"	union {\n"
		"		merge {\n"
		"			// L\n"
		"			sphere {<-88,0,0>,4}\n"
		"			cylinder {<-88,0,0>,<0,0,0>,4 open}\n"
		"			sphere {<0,0,0>,4}\n"
		"			cylinder {<0,0,0>,<0,0,25>,4 open}\n"
		"			sphere {<0,0,25>,4}\n"
		"			clipped_by {plane{y,0}}\n"
		"		}\n"
		"\n"
		"		merge {\n"
		"			// E\n"
		"			sphere {<-88,0,25>,4}\n"
		"			cylinder {<-88,0,25>,<-88,0,0>,4 open}\n"
		"			sphere {<-88,0,0>,4}\n"
		"			cylinder {<-88,0,0>,<0,0,0>,4 open}\n"
		"			sphere {<0,0,0>,4}\n"
		"			cylinder {<0,0,0>,<0,0,25>,4 open}\n"
		"			sphere {<0,0,25>,4}\n"
		"			sphere {<-44,0,17>,4}\n"
		"			cylinder {<-44,0,0>,<-44,0,17>,4 open}\n"
		"			clipped_by {plane{y,0}}\n"
		"			translate <0,0,LEGOSPACE>\n"
		"		}\n"
		"\n"
		"		merge {\n"
		"			// G\n"
		"			sphere {<-74,0,25>,4}\n"
		"			torus {12.5,4 clipped_by{plane{x,0}} translate<-74,0,12.5>}\n"
		"			cylinder {<-74,0,0>,<-14,0,0>,4 open}\n"
		"			torus {12.5,4 clipped_by{plane{-x,0}} translate<-14,0,12.5>}\n"
		"			cylinder {<-14,0,25>,<-44,0,25>,4 open}\n"
		"			sphere {<-44,0,25>,4}\n"
		"			cylinder {<-44,0,25>,<-44,0,17>,4 open}\n"
		"			sphere {<-44,0,17>,4}\n"
		"			clipped_by {plane{y,0}}\n"
		"			translate <0,0,LEGOSPACE*2>\n"
		"		}\n"
		"\n"
		"		merge {\n"
		"			// O\n"
		"			torus {12.5,4 clipped_by{plane{x,0}} translate<-74,0,12.5>}\n"
		"			cylinder {<-74,0,0>,<-14,0,0>,4 open}\n"
		"			torus {12.5,4 clipped_by{plane{-x,0}} translate<-14,0,12.5>}\n"
		"			cylinder {<-14,0,25>,<-74,0,25>,4 open}\n"
		"			clipped_by {plane{y,0}}\n"
		"			translate <0,0,LEGOSPACE*3>\n"
		"		}\n"
		"\n"
		"		matrix <1,0,-0.22,0,1,0,0,0,1,0,0,0>\n"
		"		scale .045\n"
		"		translate <1.85,-4,-4.45>\n"
		"	}\n"
		"}\n"
		"#end\n"
		"\n");
	return true;
}
