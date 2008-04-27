#ifndef __LDMODELTREE_H__
#define __LDMODELTREE_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include <TCFoundation/TCStlIncludes.h>
#include <LDLoader/LDLFileLine.h>

class LDModelTree;
class LDLModel;
class LDLModelLine;

typedef TCTypedObjectArray<LDModelTree> LDModelTreeArray;

class LDModelTree : public TCObject
{
public:
	LDModelTree(LDLModel *model = NULL);
	void scanModel(LDLModel *model, int defaultColor) const;
	const LDLModel *getModel(void) const { return m_model; }
	const LDModelTreeArray *getChildren(bool filter = true) const;
	bool hasChildren(bool filter = true) const;
	int getNumChildren(bool filter = true) const;
	const std::string &getText(void) const { return m_text; }
	LDLLineType getLineType(void) const { return m_lineType; }
	void setShowLineType(LDLLineType lineType, bool value);
	bool getShowLineType(LDLLineType lineType) const
	{
		return (m_activeLineTypes & (1 << lineType)) != 0;
	}
	bool getViewPopulated(void) const { return m_viewPopulated; }
	void setViewPopulated(bool value) { m_viewPopulated = value; }
	bool getBackgroundRGB(TCFloat &r, TCFloat &g, TCFloat &b) const;
	bool getBackgroundRGB(TCByte &r, TCByte &g, TCByte &b) const;
	bool getBackgroundRGB(int &r, int &g, int &b) const;
protected:
	LDModelTree(TCULong activeLineTypes, TCULong allLineTypes);
	virtual ~LDModelTree(void);
	virtual void dealloc(void);
	void scanLine(LDLFileLine *fileLine, int defaultColor);
	void setModel(LDLModel *model);
	bool childFilterCheck(const LDModelTree *child) const;
	void clearFilteredChildren(void);
	std::string lineTypeKey(LDLLineType lineType) const;
	//virtual void scanModelLine(LDLModelLine *modelLine, int defaultColor);

	LDLModel *m_model;
	mutable LDModelTreeArray *m_children;
	mutable LDModelTreeArray *m_filteredChildren;
	std::string m_text;
	LDLLineType m_lineType;
	int m_defaultColor;
	TCULong m_activeLineTypes;
	TCULong m_allLineTypes;
	bool m_viewPopulated;
};

#endif // __LDMODELTREE_H__
