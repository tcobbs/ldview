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
	virtual void scanModel(LDLModel *model, int defaultColor) const;
	const LDLModel *getModel(void) const { return m_model; }
	const LDModelTreeArray *getChildren(void) const;
	bool hasChildren(void) const;
	int getNumChildren(void) const;
	bool getChildrenLoaded(void) const { return m_childrenLoaded; }
	const std::string &getText(void) const { return m_text; }
	LDLLineType getLineType(void) const { return m_lineType; }
protected:
	virtual ~LDModelTree(void);
	virtual void dealloc(void);
	virtual void scanLine(LDLFileLine *fileLine, int defaultColor);
	virtual void setModel(LDLModel *model);
	//virtual void scanModelLine(LDLModelLine *modelLine, int defaultColor);

	LDLModel *m_model;
	mutable LDModelTreeArray *m_children;
	mutable bool m_childrenLoaded;
	std::string m_text;
	LDLLineType m_lineType;
	int m_defaultColor;
};

#endif // __LDMODELTREE_H__
