#include "TREMainModel.h"

#include <TCFoundation/TCDictionary.h>

#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <netinet/in.h>
#endif

#include <GL/gl.h>


TREMainModel::TREMainModel(void)
	:m_loadedModels(NULL)
{
}

TREMainModel::TREMainModel(const TREMainModel &other)
	:TREModel(other),
	m_loadedModels((TCDictionary *)TCObject::copy(other.m_loadedModels)),
	m_mainFlags(other.m_mainFlags)
{
}

TREMainModel::~TREMainModel(void)
{
}

void TREMainModel::dealloc(void)
{
	TCObject::release(m_loadedModels);
	TREModel::dealloc();
}

TCObject *TREMainModel::copy(void)
{
	return new TREMainModel(*this);
}

TCDictionary *TREMainModel::getLoadedModels(void)
{
	if (!m_loadedModels)
	{
		m_loadedModels = new TCDictionary(0);
	}
	return m_loadedModels;
}

void TREMainModel::draw(void)
{
	TCULong color = htonl(0xBBBBBBFF);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnable(GL_NORMALIZE);
	glColor4ubv((GLubyte*)&color);
	TREModel::draw(NULL);
}
