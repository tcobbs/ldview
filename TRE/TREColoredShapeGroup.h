#ifndef __TRECOLOREDSHAPEGROUP_H__
#define __TRECOLOREDSHAPEGROUP_H__

#include <TRE/TREShapeGroup.h>

class TREColoredShapeGroup : public TREShapeGroup
{
public:
	TREColoredShapeGroup(void);
	TREColoredShapeGroup(const TREColoredShapeGroup &other);
	virtual int addTriangle(TCULong color, Vector *vertices);
	virtual int addQuad(TCULong color, Vector *vertices);
protected:
	virtual ~TREColoredShapeGroup(void);
	virtual void dealloc(void);
};

#endif __TRECOLOREDSHAPEGROUP_H__
