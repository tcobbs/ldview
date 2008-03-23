#include "LDModelTree.h"
#include <LDLoader/LDLModel.h>
#include <LDLoader/LDLCommentLine.h>
#include <LDLoader/LDLConditionalLineLine.h>
#include <LDLoader/LDLEmptyLine.h>
#include <LDLoader/LDLLineLine.h>
#include <LDLoader/LDLModelLine.h>
#include <LDLoader/LDLQuadLine.h>
#include <LDLoader/LDLTriangleLine.h>
#include <LDLoader/LDLUnknownLine.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/TCUserDefaults.h>

LDModelTree::LDModelTree(LDLModel *model /*= NULL*/):
m_model(model),
m_children(NULL),
m_filteredChildren(NULL),
m_activeLineTypes(0),
m_allLineTypes(0),
m_viewPopulated(false)
{
	TCObject::retain(m_model);
	for (int i = LDLLineTypeComment; i <= LDLLineTypeUnknown; i++)
	{
		bool defaultValue = true;

		if (i == LDLLineTypeEmpty || i == LDLLineTypeUnknown)
		{
			defaultValue = false;
		}
		if (TCUserDefaults::boolForKey(lineTypeKey((LDLLineType)i).c_str(),
			defaultValue, false))
		{
			m_activeLineTypes |= 1 << i;
		}
		m_allLineTypes |= 1 << i;
	}
}

LDModelTree::LDModelTree(TCULong activeLineTypes, TCULong allLineTypes):
m_model(NULL),
m_children(NULL),
m_filteredChildren(NULL),
m_activeLineTypes(activeLineTypes),
m_allLineTypes(allLineTypes),
m_viewPopulated(false)
{
}

LDModelTree::~LDModelTree(void)
{
}

void LDModelTree::dealloc(void)
{
	TCObject::release(m_model);
	TCObject::release(m_children);
	TCObject::release(m_filteredChildren);
	TCObject::dealloc();
}

bool LDModelTree::hasChildren(bool filter /*= true*/) const
{
	if (m_model)
	{
		if (filter && m_activeLineTypes != m_allLineTypes)
		{
			return m_model->getActiveLineCount() > 0 &&
				getChildren(true)->getCount() > 0;
		}
		else
		{
			return m_model->getActiveLineCount() > 0;
		}
	}
	else
	{
		return false;
	}
}

int LDModelTree::getNumChildren(bool filter /*= true*/) const
{
	if (m_model)
	{
		if (filter && m_activeLineTypes != m_allLineTypes)
		{
			return getChildren(true)->getCount();
		}
		else
		{
			return m_model->getActiveLineCount();
		}
	}
	else
	{
		return 0;
	}
}

bool LDModelTree::childFilterCheck(const LDModelTree *child) const
{
	if (m_activeLineTypes & (1 << child->getLineType()))
	{
		return true;
	}
	else
	{
		return false;
	}
}

const LDModelTreeArray *LDModelTree::getChildren(bool filter /*= true*/) const
{
	if (m_children == NULL)
	{
		if (hasChildren(false))
		{
			scanModel(m_model, m_defaultColor);
		}
		else
		{
			m_children = new LDModelTreeArray;
		}
	}
	if (filter)
	{
		if (m_filteredChildren == NULL)
		{
			if (m_activeLineTypes == m_allLineTypes)
			{
				m_filteredChildren = m_children;
				m_filteredChildren->retain();
			}
			else
			{
				m_filteredChildren = new LDModelTreeArray;

				for (int i = 0; i < m_children->getCount(); i++)
				{
					LDModelTree *child = (*m_children)[i];

					if (childFilterCheck(child))
					{
						m_filteredChildren->addObject(child);
					}
				}
			}
		}
		return m_filteredChildren;
	}
	else
	{
		return m_children;
	}
}

void LDModelTree::scanModel(LDLModel *model, int defaultColor) const
{
	LDLFileLineArray *fileLines = model->getFileLines();

	if (fileLines)
	{
		int count = model->getActiveLineCount();

		if (count > 0)
		{
			m_children = new LDModelTreeArray(count);
			for (int i = 0; i < count; i++)
			{
				LDModelTree *child = new LDModelTree(m_activeLineTypes,
					m_allLineTypes);

				m_children->addObject(child);
				child->release();
				child->scanLine((*fileLines)[i], defaultColor);
			}
		}
	}
}

void LDModelTree::setModel(LDLModel *model)
{
	if (model != m_model)
	{
		TCObject::release(m_model);
		m_model = model;
		TCObject::retain(m_model);
	}
}

void LDModelTree::scanLine(LDLFileLine *fileLine, int defaultColor)
{
	m_text = fileLine->getLine();
	if (m_text.size() == 0)
	{
		// The typecast is needed below because the QT compile returns a QString
		// from TCLocalStrings::get().
		m_text = (const char *)TCLocalStrings::get("EmptyLine");
	}
	m_lineType = fileLine->getLineType();
	m_defaultColor = defaultColor;
	switch (m_lineType)
	{
	case LDLLineTypeComment:
		break;
	case LDLLineTypeModel:
		setModel(((LDLModelLine *)fileLine)->getModel());
		break;
	case LDLLineTypeLine:
		break;
	case LDLLineTypeTriangle:
		break;
	case LDLLineTypeQuad:
		break;
	case LDLLineTypeConditionalLine:
		break;
	case LDLLineTypeEmpty:
		break;
	case LDLLineTypeUnknown:
		break;
	}
}

void LDModelTree::clearFilteredChildren(void)
{
	if (m_children)
	{
		TCObject::release(m_filteredChildren);
		m_filteredChildren = NULL;
		for (int i = 0; i < m_children->getCount(); i++)
		{
			LDModelTree *child = (*m_children)[i];

			child->m_activeLineTypes = m_activeLineTypes;
			child->clearFilteredChildren();
		}
	}
}

std::string LDModelTree::lineTypeKey(LDLLineType lineType) const
{
	char key[128];

	sprintf(key, "ModelTreeShow%d", lineType);
	return key;
}

void LDModelTree::setShowLineType(LDLLineType lineType, bool value)
{
	if (getShowLineType(lineType) != value)
	{
		if (value)
		{
			m_activeLineTypes |= 1 << lineType;
		}
		else
		{
			m_activeLineTypes &= ~(1 << lineType);
		}
		clearFilteredChildren();
		TCUserDefaults::setBoolForKey(value, lineTypeKey(lineType).c_str(),
			false);
	}
}
