#include "LDLMainModel.h"
#include "LDLPalette.h"
#include <TCFoundation/TCDictionary.h>
#include <TCFoundation/TCStringArray.h>
#include <stdio.h>
#include <string.h>

LDLMainModel::LDLMainModel(void)
	:m_loadedModels(NULL),
	m_mainPalette(new LDLPalette),
	m_extraSearchDirs(NULL)
{
	m_mainFlags.lowResStuds = false;
	m_mainFlags.blackEdgeLines = false;
	m_mainFlags.loadCanceled = false;
}

LDLMainModel::LDLMainModel(const LDLMainModel &other)
	:LDLModel(other),
	m_loadedModels(NULL),
	m_mainFlags(other.m_mainFlags)
{
	if (other.m_loadedModels)
	{
		m_loadedModels = (TCDictionary *)other.m_loadedModels->retain();
	}
}

TCObject *LDLMainModel::copy(void)
{
	return new LDLMainModel(*this);
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
	LDLModel::dealloc();
}

bool LDLMainModel::load(const char *filename)
{
	FILE *file;
	LDLError *error;

	setFilename(filename);
	if (!strlen(lDrawDir()))
	{
		error = newError(LDLEGeneral, "Could not find LDraw directory.");
		error->setLevel(LDLACriticalError);
		sendAlert(error);
		error->release();
		return false;
	}
	file = fopen(filename, "rb");
	m_mainModel = this;
	if (file)
	{
		bool retValue = LDLModel::load(file);

		TCObject::release(m_loadedModels);
		m_loadedModels = NULL;
		return retValue;
	}
	else
	{
		error = newError(LDLEFileNotFound, "Could not find main model file.");
		error->setLevel(LDLACriticalError);
		sendAlert(error);
		error->release();
		return false;
	}
}

void LDLMainModel::print(void)
{
	printf("Model Count: %d\n", sm_modelCount);
	LDLModel::print(0);
}

bool LDLMainModel::colorNumberIsTransparent(TCULong colorNumber)
{
	return (colorNumber >= 32 && colorNumber < 48) ||
		(colorNumber >= 0x3000000 && colorNumber < 0x4000000) ||
		(colorNumber >= 0x5000000 && colorNumber < 0x7000000) ||
		(colorNumber >= 0x7000000 && colorNumber < 0x8000000) ||
		colorNumber == 57;
}

void LDLMainModel::getRGBA(TCULong colorNumber, int& r, int& g, int& b, int& a)
{
	m_mainPalette->getRGBA(colorNumber, r, g, b, a);
}

TCULong LDLMainModel::getEdgeColorNumber(TCULong colorNumber)
{
	if (getBlackEdgeLines())
	{
		if (colorNumberIsTransparent(colorNumber))
		{
			return 0x3333333;	// Color 32 is transparent dark gray, and has a
								// different RGB value than black, so return
								// the extended color number for the same RGB
								// value as black, but transparent.  (Yes, this
								// is supposed to be 7 threes; not 6, and not
								// 8.
		}
		else
		{
			return 0;
		}
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
