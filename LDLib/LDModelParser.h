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
	void setPrimitiveSubstitutionFlag(bool value)
	{
		m_flags.primitiveSubstitution = value;
	}
	bool getPrimitiveSubstitutionFlag(void)
	{
		return m_flags.primitiveSubstitution;
	}
	void setEdgeLinesFlag(bool value) { m_flags.edgeLines = value; }
	bool getEdgeLinesFlag(void) { return m_flags.edgeLines; }
	void setCurveQuality(int value) { m_curveQuality = value; }
	int getCurveQuality(void) { return m_curveQuality; }
	virtual void setSeamWidth(float seamWidth);
	virtual float getSeamWidth(void);
protected:
	virtual ~LDModelParser(void);
	virtual void dealloc(void);
	virtual bool parseModel(LDLModel *ldlModel, TREModel *treModel);
	virtual bool parseModel(LDLModelLine *modelLine, TREModel *treModel);
	virtual void parseLine(LDLShapeLine *shapeLine, TREModel *treModel);
	virtual void parseTriangle(LDLShapeLine *shapeLine, TREModel *treModel);
	virtual void parseQuad(LDLShapeLine *shapeLine, TREModel *treModel);
	virtual bool addSubModel(LDLModelLine *modelLine, TREModel *treParentModel,
		TREModel *treModel);
	virtual bool performPrimitiveSubstitution(LDLModel *ldlModel,
		TREModel *treModel);
	virtual bool substituteStud(TREModel *treModel, int numSegments);
	virtual bool substituteStud(TREModel *treModel);
	virtual bool substituteStu2(TREModel *treModel);
	virtual bool substituteStu22(TREModel *treModel, bool isA = false);
	virtual bool substituteStu23(TREModel *treModel, bool isA = false);
	virtual bool substituteStu24(TREModel *treModel, bool isA = false);
	virtual bool substituteCylinder(TREModel *treModel, float fraction);
	virtual bool substituteCone(TREModel *treModel, float fraction, int size);
//	virtual int parseShapeVertices(LDLShapeLine *shapeLine,
//		TREModel *treModel);
	virtual float startingFraction(const char *filename);
	virtual bool startsWithFraction(const char *filename);
	virtual bool startsWithFraction2(const char *filename);
	virtual bool isPrimitive(const char *filename, const char *suffix);
	virtual bool isCyli(const char *filename);
	virtual bool is1DigitCon(const char *filename);
	virtual bool is2DigitCon(const char *filename);
	virtual bool isCon(const char *filename);
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
		bool edgeLines:1;
	} m_flags;
};

#endif // __LDMODELPARSER_H__
