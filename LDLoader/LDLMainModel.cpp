#include "LDLMainModel.h"
#include <TCFoundation/TCDictionary.h>
#include <stdio.h>
#include <string.h>

LDLMainModel::LDLMainModel(void)
	:m_loadedModels(NULL)
{
	m_mainFlags.lowResStuds = false;
	m_mainFlags.blackEdgeLines = false;
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

TCULong LDLMainModel::getHighlightColorNumber(TCULong colorNumber)
{
	if (m_mainFlags.blackEdgeLines)
	{
		if (colorNumberIsTransparent(colorNumber))
		{
			return 0x3333333;	// Color 32 is transparent dark gray, and has a
								// different RGB value than black, so return
								// the extended color number for the same RGB
								// value as black, but transparent.
		}
		else
		{
			return 0;
		}
	}
	switch (colorNumber)
	{
		case 0:
			return 8;
			break;
		case 1:
			return 9;
			break;
		case 2:
			return 10;
			break;
		case 3:
			return 11;
			break;
		case 4:
			return 12;
			break;
		case 5:
			return 13;
			break;
		case 6:
			return 0;
			break;
		case 7:
			return 8;
			break;
		case 8:
			return 0;
			break;
		case 9:
			return 1;
			break;
		case 10:
			return 2;
			break;
		case 11:
			return 3;
			break;
		case 12:
			return 4;
			break;
		case 13:
			return 5;
			break;
		case 14:
			return 8;
			break;
		case 15:
			return 8;
			break;
		case 16:
/*
			if (defaultColorNumber != 16)
			{
				return getHighlightColor(defaultColorNumber);
			}
			else
*/
			{
				return 16;
			}
			break;
		case 32:
			return 40;
			break;
		case 33:
			return 41;
			break;
		case 34:
			return 42;
			break;
		case 35:
			return 43;
			break;
		case 36:
			return 44;
			break;
		case 37:
			return 45;
			break;
		case 38:
			return 32;
			break;
		case 39:
			return 40;
			break;
		case 40:
			return 32;
			break;
		case 41:
			return 33;
			break;
		case 42:
			return 34;
			break;
		case 43:
			return 35;
			break;
		case 44:
			return 36;
			break;
		case 45:
			return 37;
			break;
		case 46:
			return 40;
			break;
		case 47:
			return 40;
			break;
		case 57:
			return 40;
			break;
		default:
			return 0;
			break;
	}
}
