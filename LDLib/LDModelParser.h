#ifndef __LDMODELPARSER_H__
#define __LDMODELPARSER_H__

#include <TCFoundation/TCObject.h>

class LDLMainModel;
class LDLModel;
class TREMainModel;
class TREModel;
class LDLShapeLine;
class LDLModelLine;

class LDModelParser : public TCObject
{
public:
	LDModelParser(void);
	virtual bool parseMainModel(LDLMainModel *mainLDLModel);
	TREMainModel *getMainTREModel(void) { return m_mainTREModel; }
protected:
	virtual ~LDModelParser(void);
	virtual void dealloc(void);
	virtual bool parseModel(LDLModel *ldlModel, TREModel *treModel);
	virtual bool parseModel(LDLModelLine *modelLine, TREModel *treModel);
	virtual void parseTriangle(LDLShapeLine *shapeLine, TREModel *treModel);
	virtual void parseQuad(LDLShapeLine *shapeLine, TREModel *treModel);
	virtual bool addSubModel(LDLModelLine *modelLine, TREModel *treModel,
		TREModel *subModel);
//	virtual int parseShapeVertices(LDLShapeLine *shapeLine,
//		TREModel *treModel);

	LDLMainModel *m_mainLDLModel;
	TREMainModel *m_mainTREModel;
	struct
	{
		bool flattenParts:1;
	} m_flags;
};

#endif // __LDMODELPARSER_H__
