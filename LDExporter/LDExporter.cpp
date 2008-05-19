#include "LDExporter.h"
#include "LDPovExporter.h"
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCMacros.h>
#include <LDLoader/LDLMainModel.h>
#include <LDLoader/LDLAutoCamera.h>

LDExporter::LDExporter(const char *udPrefix /*= ""*/):
m_width(4.0f),
m_height(3.0f),
m_fov(25.0f),
m_xPan(0.0f),
m_yPan(0.0f),
m_udPrefix(udPrefix)
{
	TCFloat flipMatrix[16];
	TCFloat tempMatrix[16];

	TCVector::initIdentityMatrix(flipMatrix);
	// The following flips around to LDraw coordinate system
	flipMatrix[5] = -1.0f;
	flipMatrix[10] = -1.0f;
	// The following is for 2/3 view (30 degrees latitude, 45 degrees longitude)
	tempMatrix[0] = (TCFloat)(sqrt(2.0) / 2.0);
	tempMatrix[1] = (TCFloat)(sqrt(2.0) / 4.0);
	tempMatrix[2] = (TCFloat)(-sqrt(1.5) / 2.0);
	tempMatrix[3] = 0.0f;
	tempMatrix[4] = 0.0f;
	tempMatrix[5] = (TCFloat)(sin(M_PI / 3.0));
	tempMatrix[6] = 0.5f;
	tempMatrix[7] = 0.0f;
	tempMatrix[8] = (TCFloat)(sqrt(2.0) / 2.0);
	tempMatrix[9] = (TCFloat)(-sqrt(2.0) / 4.0);
	tempMatrix[10] = (TCFloat)(sqrt(1.5) / 2.0);
	tempMatrix[11] = 0.0f;
	tempMatrix[12] = 0.0f;
	tempMatrix[13] = 0.0f;
	tempMatrix[14] = 0.0f;
	tempMatrix[15] = 1.0f;
	TCVector::multMatrix(flipMatrix, tempMatrix, m_rotationMatrix);

	m_primSub = boolForKey("PrimitiveSubstitution", true);
	m_seamWidth = floatForKey("SeamWidth", 0.5);
	m_edges = boolForKey("Edges", false);
	m_conditionalEdges = boolForKey("ConditionalEdges", false);
}

LDExporter::~LDExporter(void)
{
}

void LDExporter::dealloc(void)
{
	LDLPrimitiveCheck::dealloc();
}

// Note: static method.
int LDExporter::run(void)
{
	LDExporter exporter;
	return exporter.runInternal();
}

int LDExporter::doExport(LDLModel * /*pTopModel*/)
{
	consolePrintf("Programmer error: LDExporter::doExport called.\n");
	return 1;
}

TCFloat LDExporter::getHFov(void)
{
	return (TCFloat)(2.0 * rad2deg(atan(tan(deg2rad(m_fov / 2.0)) *
		(double)m_width / (double)m_height)));
}

int LDExporter::runInternal(void)
{
	char *outputType = stringForKey("OutputType", NULL, false);
	int retValue;
	LDExporter *pExporter = NULL;

	if (outputType == NULL)
	{
		outputType = stringForKey("OT", "POV", false);
	}
	if (strcasecmp(outputType, "POV") == 0)
	{
		pExporter = new LDPovExporter;
	}
	retValue = runInternal(pExporter);
	delete outputType;
	TCObject::release(pExporter);
	return retValue;
}

int LDExporter::runInternal(LDExporter *pExporter)
{
	int retValue;

	if (pExporter != NULL)
	{
		std::string filename = getFilename();

		if (filename.size() > 0)
		{
			LDLMainModel *pMainModel = new LDLMainModel;

			if (pMainModel->load(filename.c_str()))
			{
				char *cameraGlobe = stringForKey("CameraGlobe", NULL, false);
				LDLCamera camera;
				LDLAutoCamera *pAutoCamera = new LDLAutoCamera;

				pMainModel->getBoundingBox(pExporter->m_boundingMin,
					pExporter->m_boundingMax);
				pExporter->m_center = (pExporter->m_boundingMin +
					pExporter->m_boundingMax) / 2.0f;
				pExporter->m_radius = pMainModel->getMaxRadius(pExporter->m_center);
				pAutoCamera->setModel(pMainModel);
				pAutoCamera->setModelCenter(pExporter->m_center);
				pAutoCamera->setRotationMatrix(m_rotationMatrix);
				pAutoCamera->setCamera(camera);
				pAutoCamera->setCameraGlobe(cameraGlobe);
				//pAutoCamera->setDistanceMultiplier(distanceMultiplier);
				// Width and height are only needed for aspect ratio, not
				// absolute size.
				pAutoCamera->setWidth(m_width);
				pAutoCamera->setHeight(m_height);
				pAutoCamera->setFov(m_fov);

				pAutoCamera->zoomToFit();
				pExporter->m_camera = pAutoCamera->getCamera();
				pAutoCamera->release();
				retValue = pExporter->doExport(pMainModel);
			}
			else
			{
#ifdef TC_NO_UNICODE
				consolePrintf(ls(_UC("ErrorLoadingModel")), filename.c_str());
#else // TC_NO_UNICODE
				std::wstring wfilename;
				stringtowstring(wfilename, filename);
				consolePrintf(ls(_UC("ErrorLoadingModel")), wfilename.c_str());
#endif // TC_NO_UNICODE
				retValue = 1;
			}
			TCObject::release(pMainModel);
		}
		else
		{
			consolePrintf(ls(_UC("NoFilename")));
			retValue = 1;
		}
	}
	else
	{
		consolePrintf(ls(_UC("UnknownOutputType")));
		retValue = 1;
	}
	return retValue;
}

std::string LDExporter::getFilename(void)
{
	std::string retValue;
	TCStringArray *commandLine = TCUserDefaults::getProcessedCommandLine();

	if (commandLine)
	{
		int i;
		int count = commandLine->getCount();

		for (i = 0; i < count && !retValue.size(); i++)
		{
			char *arg = commandLine->stringAtIndex(i);

			if (arg[0] != '-')
			{
				retValue = arg;
			}
		}
	}
	return retValue;
}

std::string LDExporter::udKey(const char *key)
{
	return m_udPrefix + key;
}

void LDExporter::setStringForKey(
	const char* value,
	const char* key,
	bool sessionSpecific /*= true*/)
{
	TCUserDefaults::setStringForKey(value, udKey(key).c_str(), sessionSpecific);
}

char* LDExporter::stringForKey(
	const char* key,
	const char* defaultValue /*= NULL*/,
	bool sessionSpecific /*= true*/)
{
	return TCUserDefaults::stringForKey(udKey(key).c_str(), defaultValue,
		sessionSpecific);
}

void LDExporter::setLongForKey(
	long value,
	const char* key,
	bool sessionSpecific /*= true*/)
{
	TCUserDefaults::setLongForKey(value, udKey(key).c_str(), sessionSpecific);
}

long LDExporter::longForKey(
	const char* key,
	long defaultValue /*= 0*/,
	bool sessionSpecific /*= true*/)
{
	return TCUserDefaults::longForKey(udKey(key).c_str(), defaultValue, 
		sessionSpecific);
}

void LDExporter::setBoolForKey(
	bool value,
	const char *key,
	bool sessionSpecific /*= true*/)
{
	TCUserDefaults::setBoolForKey(value, udKey(key).c_str(), sessionSpecific);
}

bool LDExporter::boolForKey(
	const char *key,
	bool defaultValue /*= false*/,
	bool sessionSpecific /*= true*/)
{
	return TCUserDefaults::boolForKey(udKey(key).c_str(), defaultValue,
		sessionSpecific);
}

void LDExporter::setFloatForKey(
	float value,
	const char* key,
	bool sessionSpecific /*= true*/)
{
	TCUserDefaults::setFloatForKey(value, udKey(key).c_str(), sessionSpecific);
}

float LDExporter::floatForKey(
	const char* key,
	float defaultValue /*= 0.0f*/,
	bool sessionSpecific /*= true*/)
{
	return TCUserDefaults::floatForKey(udKey(key).c_str(), defaultValue,
		sessionSpecific);
}

