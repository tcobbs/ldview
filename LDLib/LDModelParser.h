#ifndef __LDMODELPARSER_H__
#define __LDMODELPARSER_H__

#include <TCFoundation/TCObject.h>

class LDLMainModel;
class LDLModel;
class TREMainModel;
class TREModel;
class TRESubModel;
class LDLShapeLine;
class LDLModelLine;

class LDModelParser : public TCObject
{
public:
	LDModelParser(void);
	virtual bool parseMainModel(LDLMainModel *mainLDLModel);
	TREMainModel *getMainTREModel(void) { return m_mainTREModel; }
	void setPrimitiveSubstitution(bool value)
	{
		m_flags.primitiveSubstitution = value;
	}
	bool getPrimitiveSubstitution(void)
	{
		return m_flags.primitiveSubstitution;
	}
	virtual void setSeamWidth(float seamWidth);
	virtual float getSeamWidth(void);
protected:
	virtual ~LDModelParser(void);
	virtual void dealloc(void);
	virtual bool parseModel(LDLModel *ldlModel, TREModel *treModel);
	virtual bool parseModel(LDLModelLine *modelLine, TREModel *treModel);
	virtual void parseTriangle(LDLShapeLine *shapeLine, TREModel *treModel);
	virtual void parseQuad(LDLShapeLine *shapeLine, TREModel *treModel);
	virtual bool addSubModel(LDLModelLine *modelLine, TREModel *treParentModel,
		TREModel *treModel);
	virtual bool performPrimitiveSubstitution(LDLModel *ldlModel,
		TREModel *treModel);
	virtual bool substituteStud(TREModel *treModel);
	virtual bool substituteStu2(TREModel *treModel);
	virtual bool substituteStu22(TREModel *treModel, bool isA = false);
	virtual bool substituteStu23(TREModel *treModel, bool isA = false);
	virtual bool substituteStu24(TREModel *treModel, bool isA = false);
	virtual bool substituteCylinder(TREModel *treModel, float fraction);
//	virtual int parseShapeVertices(LDLShapeLine *shapeLine,
//		TREModel *treModel);
	virtual float startingFraction(const char *filename);
	virtual bool startsWithFraction(const char *filename);
	virtual bool startsWithFraction2(const char *filename);
	virtual bool isPrimitive(const char *filename, const char *suffix);
	virtual bool isCyli(const char *filename);
	virtual int getNumCircleSegments(float fraction = 0.0f);
	virtual void finishPart(TREModel *treModel, TRESubModel *subModel = NULL);

	LDLMainModel *m_mainLDLModel;
	TREMainModel *m_mainTREModel;
	int m_curveQuality;
	float m_seamWidth;
	struct
	{
		bool flattenParts:1;
		bool primitiveSubstitution:1;
		bool seams:1;
	} m_flags;
};

#endif // __LDMODELPARSER_H__
