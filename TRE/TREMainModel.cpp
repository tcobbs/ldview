#include "TREMainModel.h"
#include "TREVertexStore.h"
#include "TREGL.h"

#include <TCFoundation/TCDictionary.h>

TREMainModel::TREMainModel(void)
	:m_loadedModels(NULL),
	m_vertexStore(new TREVertexStore),
	m_coloredVertexStore(new TREVertexStore)
{
	m_mainModel = this;
}

TREMainModel::TREMainModel(const TREMainModel &other)
	:TREModel(other),
	m_loadedModels((TCDictionary *)TCObject::copy(other.m_loadedModels)),
	m_vertexStore((TREVertexStore *)TCObject::copy(other.m_vertexStore)),
	m_coloredVertexStore((TREVertexStore *)TCObject::copy(
		other.m_coloredVertexStore)),
	m_mainFlags(other.m_mainFlags)
{
	m_mainModel = this;
}

TREMainModel::~TREMainModel(void)
{
}

void TREMainModel::dealloc(void)
{
	TCObject::release(m_loadedModels);
	TCObject::release(m_vertexStore);
	TCObject::release(m_coloredVertexStore);
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

	if (!m_defaultColorListID)
	{
		m_vertexStore->activate();
		compileDefaultColor();
		m_coloredVertexStore->activate();
		compileColored();
	}
	glColor4ubv((GLubyte*)&color);
	m_vertexStore->activate();
	drawDefaultColor();
	m_coloredVertexStore->activate();
	drawColored();
}

TREModel *TREMainModel::modelNamed(const char *name)
{
	return (TREMainModel *)getLoadedModels()->objectForKey(name);
}

void TREMainModel::registerModel(TREModel *model)
{
	getLoadedModels()->setObjectForKey(model, model->getName());
}
