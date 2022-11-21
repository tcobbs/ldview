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

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

LDModelTree::LDModelTree(LDLModel *model /*= NULL*/):
m_model(model),
m_children(NULL),
m_filteredChildren(NULL),
m_replaced(false),
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

LDModelTree::LDModelTree(
	TCULong activeLineTypes,
	TCULong allLineTypes):
m_model(NULL),
m_children(NULL),
m_filteredChildren(NULL),
m_replaced(false),
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
	return false;
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
			for (int i = 0; i < count; ++i)
			{
				LDLFileLine *fileLine = (*fileLines)[i];
				if (fileLine->getLineNumber() == 0)
				{
					// Line number 0 is the custom config model, which we don't
					// want to show to the user.
					continue;
				}
				LDModelTree *child = new LDModelTree(m_activeLineTypes,
					m_allLineTypes);

				m_children->addObject(child);
				child->release();
				child->scanLine(fileLine, defaultColor);
				child->m_treePath = m_treePath + "/";
				child->m_treePath += ltostr(i + 1);
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
	m_fileLine = fileLine;
	m_text = stringtoucstring(fileLine->getLine());
	if (fileLine->getOriginalLine() != NULL)
	{
		ucstring tempString = stringtoucstring(fileLine->getOriginalLine());
		ucstring tempString2 = tempString;
		stripCRLF(&tempString2[0]);
		stripTrailingWhitespace(&tempString2[0]);
		stripLeadingWhitespace(&tempString2[0]);
		if (tempString2 != m_text)
		{
			m_text += ls(_UC("LDMTOriginalLine"));
			m_text += tempString;
			m_text += ls(_UC("LDMTCloseParen"));
		}
	}
	m_replaced = fileLine->isReplaced();
	if (m_text.size() == 0)
	{
		// The typecast is needed below because the QT compile returns a QString
		// from TCLocalStrings::get().
		m_text = (CUCSTR)TCLocalStrings::get(_UC("EmptyLine"));
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
		m_viewPopulated = false;
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

	snprintf(key, sizeof(key), "ModelTreeShow%d", lineType);
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

bool LDModelTree::getBackgroundRGB(
	TCFloat &r,
	TCFloat &g,
	TCFloat &b,
	bool darkMode) const
{
	if (darkMode)
	{
		return getRGB(0.0f, 0.5f, r, g, b);
	}
	else
	{
		return getRGB(0.85f, 1.0f, r, g, b);
	}
}

bool LDModelTree::getTextRGB(TCFloat &r, TCFloat &g, TCFloat &b) const
{
	return getRGB(0.0f, 0.5f, r, g, b);
}

bool LDModelTree::getRGB(
	TCFloat l,
	TCFloat h,
	TCFloat &r,
	TCFloat &g,
	TCFloat &b) const
{
	float l2 = h - ((h - l) * 0.5f);

	r = g = b = l;
	if (m_replaced)
	{
		float l3 = h - ((h - l) * 0.45f);
		// Green-blue (more green)
		g = h;
		b = l3;
	}
	else
	{
		switch (m_lineType)
		{
		case LDLLineTypeComment:
			// Green
			g = h;
			break;
		case LDLLineTypeLine:
			// Pink/Red
			r = h;
			break;
		case LDLLineTypeTriangle:
			// Blue
			b = h;
			break;
		case LDLLineTypeQuad:
			// Cyan
			g = h;
			b = h;
			break;
		case LDLLineTypeConditionalLine:
			// Orange/Brown
			r = h;
			g = l2;
			break;
		case LDLLineTypeEmpty:
			// Gray
			r = l2;
			g = l2;
			b = l2;
			break;
		case LDLLineTypeUnknown:
			// Yellow
			r = h;
			g = h;
			break;
		default:
			return false;
		}
	}
	return true;
}

bool LDModelTree::getBackgroundRGB(
	TCByte &r,
	TCByte &g,
	TCByte &b,
	bool darkMode) const
{
	TCFloat rf, gf, bf;
	
	if (getBackgroundRGB(rf, gf, bf, darkMode))
	{
		r = (TCByte)(rf * 255.0 + 0.5);
		g = (TCByte)(gf * 255.0 + 0.5);
		b = (TCByte)(bf * 255.0 + 0.5);
		return true;
	}
	else
	{
		return false;
	}
}

bool LDModelTree::getBackgroundRGB(
	int &r,
	int &g,
	int &b,
	bool darkMode) const
{
	TCByte rb, gb, bb;
	
	if (getBackgroundRGB(rb, gb, bb, darkMode))
	{
		r = (int)rb;
		g = (int)gb;
		b = (int)bb;
		return true;
	}
	else
	{
		return false;
	}
}

bool LDModelTree::getTextRGB(TCByte &r, TCByte &g, TCByte &b) const
{
	TCFloat rf, gf, bf;
	
	if (getTextRGB(rf, gf, bf))
	{
		r = (TCByte)(rf * 255.0 + 0.5);
		g = (TCByte)(gf * 255.0 + 0.5);
		b = (TCByte)(bf * 255.0 + 0.5);
		return true;
	}
	else
	{
		return false;
	}
}

bool LDModelTree::getTextRGB(int &r, int &g, int &b) const
{
	TCByte rb, gb, bb;
	
	if (getTextRGB(rb, gb, bb))
	{
		r = (int)rb;
		g = (int)gb;
		b = (int)bb;
		return true;
	}
	else
	{
		return false;
	}
}

const ucstring &LDModelTree::getStatusText(void) const
{
	if (m_statusText.size() == 0)
	{
		if (m_fileLine != NULL)
		{
			char *filename = filenameFromPath(
				m_fileLine->getParentModel()->getFilename());
			m_statusText = stringtoucstring(filename);

			delete[] filename;
			m_statusText += ls(_UC("SpaceLineSpace"));
			m_statusText += ltoucstr(m_fileLine->getLineNumber());
		}
		else
		{
			m_statusText = ls(_UC("Error"));
		}
	}
	return m_statusText;
}

std::string LDModelTree::adjustHighlightPath(std::string path) const
{
//	if (m_children == NULL)
//	{
//		return "";
//	}
	if (m_activeLineTypes != m_allLineTypes)
	{
		int lineNumber = atoi(&path[1]) - 1;
		int newLineNumber = 0;
		std::string prefix("/");
		size_t index = path.find('/', 1);
		int numChildren = getNumChildren(false);

		// Make sure children are loaded.
		getChildren(false);
		if (newLineNumber >= numChildren)
		{
			newLineNumber = numChildren - 1;
		}
		for (int i = 0; i < lineNumber; i++)
		{
			if (childFilterCheck((*m_children)[i]))
			{
				newLineNumber++;
			}
		}
		prefix += ltostr(newLineNumber + 1);
		if (index < path.size())
		{
			path = prefix + (*m_children)[lineNumber]->adjustHighlightPath(
				path.substr(index));
		}
		else
		{
			path = prefix;
		}
	}
	return path;
}

#ifndef TC_NO_UNICODE

const std::string &LDModelTree::getText(void) const
{
	wstringtostring(m_aText, m_text);
	return m_aText;
}


#endif // TC_NO_UNICODE

void LDModelTree::parsePathString(
	const std::string& pathString,
	IntVector& path)
{
	int count;
	char** components = componentsSeparatedByString(pathString.c_str(), "/",
		count);
	path.clear();
	if (count > 0)
	{
		path.reserve(count - 1);
	}
	for (int i = 1; i < count; ++i)
	{
		path.push_back(atoi(components[i]) - 1);
	}
	deleteStringArray(components, count);
}

void LDModelTree::genPathString(
	const IntVector& path,
	std::string& pathString)
{
	pathString.clear();
	for (int value: path)
	{
		pathString += "/" + ltostr(value + 1);
	}
}

bool LDModelTree::search(
	const ucstring& searchStringUC,
	std::string& pathString,
	SearchMode mode) const
{
	if (searchStringUC.empty())
	{
		return false;
	}
	IntVector path;
	parsePathString(pathString, path);
	pathString.clear();
	if (mode == SMType && !path.empty())
	{
		if (path[0] == 0)
		{
			path.clear();
		}
		else
		{
			--path[0];
		}
	}
	int loopEnd = path.empty() ? -1 : path[0];
	std::string searchString;
	ucstringtoutf8(searchString, searchStringUC);
	bool result = mode == SMPrevious ? m_model->searchPrevious(searchString,
		path, loopEnd, m_activeLineTypes) : m_model->searchNext(searchString,
		path, loopEnd, m_activeLineTypes);
	if (result)
	{
		genPathString(path, pathString);
	}
	return result;
}

//bool LDModelTree::searchNext(
//	const ucstring& searchString,
//	IntVector& path,
//	int loopEnd) const
//{
//	getChildren(false);
//	if (m_children == NULL)
//	{
//		path.clear();
//		return false;
//	}
//	IntVector result;
//	int count = m_children->getCount();
//	int endIndex = path.empty() && loopEnd != -1 ? loopEnd + 1: count;
//	int startIndex = path.empty() ? 0 : path[0];
//	for (int i = startIndex; i < endIndex; ++i)
//	{
//		const LDModelTree *child = (*m_children)[i];
//		IntVector childPath;
//		int lineOffset = 0;
//		bool skipText = false;
//		if (!path.empty() && i == path[0])
//		{
//			skipText = true;
//			if (path.size() > 1)
//			{
//				childPath.insert(childPath.end(), path.begin() + 1, path.end());
//			}
//		}
//		if (!skipText)
//		{
//			CUCSTR match = ucstrcasestr(child->m_text.c_str() + lineOffset,
//				searchString.c_str());
//
//			if (match != NULL)
//			{
//				result.push_back(i);
//				path = result;
//				return true;
//			}
//		}
//		if (child->searchNext(searchString, childPath, -1))
//		{
//			result.push_back(i);
//			result.insert(result.end(), childPath.begin(), childPath.end());
//			path = result;
//			return true;
//		}
//	}
//	if (!path.empty() && loopEnd != -1)
//	{
//		path.clear();
//		return searchNext(searchString, path, loopEnd);
//	}
//	path.clear();
//	return false;
//}
//
//bool LDModelTree::searchPrevious(
//	const ucstring& searchString,
//	IntVector& path,
//	int loopEnd) const
//{
//	getChildren(false);
//	if (m_children == NULL)
//	{
//		path.clear();
//		return false;
//	}
//	IntVector result;
//	int count = m_children->getCount();
//	int startIndex = path.empty() ? count - 1 : path[0];
//	int endIndex = path.empty() && loopEnd != -1 ? loopEnd : 0;
//	for (int i = startIndex; i >= endIndex; --i)
//	{
//		const LDModelTree *child = (*m_children)[i];
//		IntVector childPath;
//		int lineOffset = 0;
//		if (!path.empty() && i == path[0])
//		{
//			if (path.size() == 1)
//			{
//				continue;
//			}
//			else
//			{
//				childPath.insert(childPath.end(), path.begin() + 1, path.end());
//			}
//		}
//		if (child->searchPrevious(searchString, childPath, -1))
//		{
//			result.push_back(i);
//			result.insert(result.end(), childPath.begin(), childPath.end());
//			path = result;
//			return true;
//		}
//		CUCSTR match = ucstrcasestr(child->m_text.c_str() + lineOffset,
//			searchString.c_str());
//
//		if (match != NULL)
//		{
//			result.push_back(i);
//			path = result;
//			return true;
//		}
//	}
//	if (!path.empty() && loopEnd != -1)
//	{
//		path.clear();
//		return searchPrevious(searchString, path, loopEnd);
//	}
//	path.clear();
//	return false;
//}
