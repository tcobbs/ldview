#include "LDLMainModel.h"
#include <TCFoundation/TCDictionary.h>
#include <stdio.h>

LDLMainModel::LDLMainModel(void)
	:m_loadedModels(NULL)
{
	m_mainFlags.lowResStuds = false;
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
	FILE *file = fopen(filename, "rb");

	setFilename(filename);
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
		return false;
	}
}

void LDLMainModel::print(void)
{
	printf("Model Count: %d\n", sm_modelCount);
	LDLModel::print(0);
}
