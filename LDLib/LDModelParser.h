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
		return m_flags.primitiveSubstitution != false;
	}
	void setEdgeLinesFlag(bool value) { m_flags.edgeLines = value; }
	bool getEdgeLinesFlag(void) { return m_flags.edgeLines != false; }
	void setLightingFlag(bool value) { m_flags.lighting = value; }
	bool getLightingFlag(void) { return m_flags.lighting != false; }
	void setTwoSidedLightingFlag(bool value)
	{
		m_flags.twoSidedLighting = value;
	}
	bool getTwoSidedLightingFlag(void)
	{
		return m_flags.twoSidedLighting != false;
	}
	void setBFCFlag(bool value) { m_flags.bfc = value; }
	bool getBFCFlag(void) { return m_flags.bfc != false; }
	void setCompilePartsFlag(bool value) { m_flags.compileParts = value; }
	bool getCompilePartsFlag(void) { return m_flags.compileParts != false; }
	void setCompileAllFlag(bool value) { m_flags.compileAll = value; }
	bool getCompileAllFlag(void) { return m_flags.compileAll != false; }
	void setCurveQuality(int value) { m_curveQuality = value; }
	int getCurveQuality(void) { return m_curveQuality; }
	virtual void setSeamWidth(float seamWidth);
	virtual float getSeamWidth(void);
protected:
	virtual ~LDModelParser(void);
	virtual void dealloc(void);
	virtual bool parseModel(LDLModel *ldlModel, TREModel *treModel, bool bfc);
	virtual bool parseModel(LDLModelLine *modelLine, TREModel *treModel,
		bool bfc);
	virtual void parseLine(LDLShapeLine *shapeLine, TREModel *treModel);
	virtual void parseTriangle(LDLShapeLine *shapeLine, TREModel *treModel,
		bool bfc, bool invert);
	virtual void parseQuad(LDLShapeLine *shapeLine, TREModel *treModel,
		bool bfc, bool invert);
	virtual bool addSubModel(LDLModelLine *modelLine, TREModel *treParentModel,
		TREModel *treModel, bool invert);
	virtual bool performPrimitiveSubstitution(LDLModel *ldlModel,
		TREModel *treModel, bool bfc);
	virtual bool substituteStud(TREModel *treModel, int numSegments);
	virtual bool substituteStud(TREModel *treModel);
	virtual bool substituteStu2(TREModel *treModel);
	virtual bool substituteStu22(TREModel *treModel, bool isA, bool bfc);
	virtual bool substituteStu23(TREModel *treModel, bool isA, bool bfc);
	virtual bool substituteStu24(TREModel *treModel, bool isA, bool bfc);
	virtual bool substituteCylinder(TREModel *treModel, float fraction,
		bool bfc);
	virtual bool substituteSlopedCylinder(TREModel *treModel, float fraction,
		bool bfc);
	virtual bool substituteSlopedCylinder2(TREModel *treModel, float fraction,
		bool bfc);
	virtual bool substituteDisc(TREModel *treModel, float fraction, bool bfc);
	virtual bool substituteNotDisc(TREModel *treModel, float fraction,
		bool bfc);
	virtual bool substituteCircularEdge(TREModel *treModel, float fraction);
	virtual bool substituteCone(TREModel *treModel, float fraction, int size,
		bool bfc);
	virtual bool substituteRing(TREModel *treModel, float fraction, int size,
		bool bfc);
	virtual float startingFraction(const char *filename);
	virtual bool startsWithFraction(const char *filename);
	virtual bool startsWithFraction2(const char *filename);
	virtual bool isPrimitive(const char *filename, const char *suffix);
	virtual bool isCyli(const char *filename);
	virtual bool isCyls(const char *filename);
	virtual bool isCyls2(const char *filename);
	virtual bool isDisc(const char *filename);
	virtual bool isNdis(const char *filename);
	virtual bool isEdge(const char *filename);
	virtual bool isCcyli(const char *filename);
	virtual bool is1DigitCon(const char *filename);
	virtual bool is2DigitCon(const char *filename);
	virtual bool isCon(const char *filename);
	virtual bool isOldRing(const char *filename);
	virtual bool isRing(const char *filename);
	virtual bool isRin(const char *filename);
	virtual bool isTorusO(const char *filename);
	virtual bool isTorusI(const char *filename);
	virtual int getNumCircleSegments(float fraction = 0.0f);
	virtual void finishPart(TREModel *treModel, TRESubModel *subModel = NULL);
	virtual bool shouldFlipWinding(bool invert, bool windingCCW);

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
		bool twoSidedLighting:1;
		bool bfc:1;
		bool compileParts:1;
		bool compileAll:1;
		bool lighting:1;
	} m_flags;
};

#endif // __LDMODELPARSER_H__
