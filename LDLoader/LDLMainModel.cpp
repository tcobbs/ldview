#include "LDLMainModel.h"
#include "LDLPalette.h"
#include <TCFoundation/TCDictionary.h>
#include <TCFoundation/TCStringArray.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <TCFoundation/TCLocalStrings.h>
#include "LDrawIni.h"

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

LDLMainModel::LDLMainModel(void)
	:m_alertSender(NULL),
	m_loadedModels(NULL),
	m_mainPalette(new LDLPalette),
	m_extraSearchDirs(NULL),
	m_seamWidth(0.0f)
{
	m_mainFlags.lowResStuds = false;
	m_mainFlags.blackEdgeLines = false;
	m_mainFlags.skipValidation = false;
	m_mainFlags.loadCanceled = false;
	m_mainFlags.processLDConfig = true;
}

TCObject *LDLMainModel::copy(void) const
{
	return NULL;
}

TCDictionary *LDLMainModel::getLoadedModels(void)
{
	if (!m_loadedModels)
	{
		m_loadedModels = new TCDictionary(0);
	}
	return m_loadedModels;
}

void LDLMainModel::dealloc(void)
{
	TCObject::release(m_loadedModels);
	TCObject::release(m_mainPalette);
	TCObject::release(m_extraSearchDirs);
	LDLModel::dealloc();
}

bool LDLMainModel::load(const char *filename)
{
	FILE *file;
	LDLError *error;

	setFilename(filename);
	lDrawDir();	// Initializes sm_lDrawIni
	if (sm_lDrawIni)
	{
		LDrawIniComputeRealDirs(sm_lDrawIni, 1, 0, filename);
	}
	if (!strlen(lDrawDir()))
	{
		error = newError(LDLEGeneral,
			TCLocalStrings::get(_UC("LDLMainModelNoLDrawDir")));
		error->setLevel(LDLACriticalError);
		sendAlert(error);
		error->release();
		return false;
	}
	file = fopen(filename, "rb");
	m_mainModel = this;
	if (file)
	{
		bool retValue;

		if (m_mainFlags.processLDConfig)
		{
			processLDConfig();
		}
		retValue = LDLModel::load(file);
		if (sm_lDrawIni)
		{
			// If bool isn't 1 byte, then the filename case callback won't
			// work, so doesn't get used.  Check to see if this will be a
			// problem.
			if (sizeof(bool) != sizeof(char) && fileCaseCallback)
			{
				char *tmpStr;
				int len = strlen(lDrawDir());
				bool failed = false;
				struct stat statData;

				tmpStr = new char[len + 10];
				sprintf(tmpStr, "%s/P", lDrawDir());
				if (stat(tmpStr, &statData) != 0)
				{
					// Check to see if we can access the P directory inside the
					// LDraw directory.  If not, then we have a problem that
					// needs to be reported to the user.
					failed = true;
				}
				if (!failed)
				{
					sprintf(tmpStr, "%s/PARTS", lDrawDir());
					if (stat(tmpStr, &statData) != 0)
					{
						// Check to see if we can access the PARTS directory
						// inside the LDraw directory.  If not, then we have a
						// problem that needs to be reported to the user.
						failed = true;
					}
				}
				delete tmpStr;
				if (failed)
				{
					// Either P or PARTS was inaccessible, so let the user
					// know that they need to rename the directories to be in
					// upper case.
					reportError(LDLEGeneral,
						TCLocalStrings::get(_UC("LDLMainModelFileCase")));
				}
			}
		}
		// The ancestor map has done its job; may as well free up the memory it
		// was using.
		m_ancestorMap.clear();
		return retValue;
	}
	else
	{
		error = newError(LDLEFileNotFound,
			TCLocalStrings::get(_UC("LDLMainModelNoMainModel")));
		error->setLevel(LDLACriticalError);
		sendAlert(error);
		error->release();
		return false;
	}
}

void LDLMainModel::processLDConfig(void)
{
	char filename[1024];
	FILE *configFile;

	sprintf(filename, "%s/ldconfig.ldr", lDrawDir());
	configFile = fopen(filename, "rb");
	if (configFile)
	{
		LDLModel *subModel;

		fclose(configFile);
		subModel = subModelNamed(filename);
	}
}

void LDLMainModel::print(void)
{
	printf("Model Count: %d\n", sm_modelCount);
	LDLModel::print(0);
}

bool LDLMainModel::colorNumberIsTransparent(int colorNumber)
{
	return (colorNumber >= 32 && colorNumber < 48) ||
		(colorNumber >= 0x3000000 && colorNumber < 0x4000000) ||
		(colorNumber >= 0x5000000 && colorNumber < 0x7000000) ||
		(colorNumber >= 0x7000000 && colorNumber < 0x8000000) ||
		colorNumber == 57;
}

void LDLMainModel::getRGBA(int colorNumber, int& r, int& g, int& b, int& a)
{
	m_mainPalette->getRGBA(colorNumber, r, g, b, a);
}

bool LDLMainModel::hasSpecular(int colorNumber)
{
	return m_mainPalette->hasSpecular(colorNumber);
}

bool LDLMainModel::hasShininess(int colorNumber)
{
	return m_mainPalette->hasShininess(colorNumber);
}

void LDLMainModel::getSpecular(int colorNumber, float *specular)
{
	memcpy(specular, m_mainPalette->getAnyColorInfo(colorNumber).specular,
		4 * sizeof(float));
}

void LDLMainModel::getShininess(int colorNumber, float &shininess)
{
	shininess = m_mainPalette->getAnyColorInfo(colorNumber).shininess;
}

int LDLMainModel::getEdgeColorNumber(int colorNumber)
{
	if (getBlackEdgeLines())
	{
		int colorNumber =
			m_mainPalette->getColorNumberForName("Black Edge");

		if (colorNumber < 0)
		{
			colorNumber = 0x2000000;
		}
		return colorNumber;
		//if (colorNumberIsTransparent(colorNumber))
		//{
		//	return 0x3333333;	// Color 32 is transparent dark gray, and has a
		//						// different RGB value than black, so return
		//						// the extended color number for the same RGB
		//						// value as black, but transparent.  (Yes, this
		//						// is supposed to be 7 threes; not 6, and not
		//						// 8.
		//}
		//else
		//{
		//	return 0;
		//}
	}
	return m_mainPalette->getEdgeColorNumber(colorNumber);
}

void LDLMainModel::setExtraSearchDirs(TCStringArray *value)
{
	if (m_extraSearchDirs != value)
	{
		TCObject::release(m_extraSearchDirs);
		m_extraSearchDirs = value;
		TCObject::retain(m_extraSearchDirs);
	}
}

void LDLMainModel::setSeamWidth(float value)
{
	if (value != m_seamWidth)
	{
		m_seamWidth = value;
		m_flags.haveBoundingBox = false;
	}
}
