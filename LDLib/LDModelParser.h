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
class LDLConditionalLineLine;

class LDModelParser : public TCObject
{
public:
	LDModelParser(void);
	virtual bool parseMainModel(LDLMainModel *mainLDLModel);
	TREMainModel *getMainTREModel(void) { return m_mainTREModel; }
	void setFileIsPartFlag(bool value) { m_flags.fileIsPart = value; }
	bool getFileIsPartFlag(void) { return m_flags.fileIsPart != false; }
	void setFlattenPartsFlag(bool value) { m_flags.flattenParts = value; }
	bool getFlattenPartsFlag(void) { return m_flags.flattenParts != false; }
	void setPrimitiveSubstitutionFlag(bool value)
	{
		m_flags.primitiveSubstitution = value;
	}
	bool getPrimitiveSubstitutionFlag(void)
	{
		return m_flags.primitiveSubstitution != false;
	}
	bool getSeamsFlag(void) { return m_flags.seams != false; }
	void setEdgeLinesFlag(bool value) { m_flags.edgeLines = value; }
	bool getEdgeLinesFlag(void) { return m_flags.edgeLines != false; }
	void setEdgesOnlyFlag(bool value) { m_flags.edgesOnly = value; }
	bool getEdgesOnlyFlag(void) { return m_flags.edgesOnly != false; }
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
	void setRedBackFacesFlag(bool value) { m_flags.redBackFaces = value; }
	bool getRedBackFacesFlag(void) { return m_flags.redBackFaces != false; }
	void setGreenFrontFacesFlag(bool value) { m_flags.greenFrontFaces = value; }
	bool getGreenFrontFacesFlag(void)
	{
		return m_flags.greenFrontFaces != false;
	}
	void setAALinesFlag(bool value) { m_flags.aaLines = value; }
	bool getAALinesFlag(void) { return m_flags.aaLines != false; }
	void setSortTransparentFlag(bool value) { m_flags.sortTransparent = value; }
	bool getSortTransparentFlag(void)
	{
		return m_flags.sortTransparent != false;
	}
	void setStippleFlag(bool value) { m_flags.stipple = value; }
	bool getStippleFlag(void) { return m_flags.stipple != false; }
	void setWireframeFlag(bool value) { m_flags.wireframe = value; }
	bool getWireframeFlag(void) { return m_flags.wireframe != false; }
	void setConditionalLinesFlag(bool value)
	{
		m_flags.conditionalLines = value;
	}
	bool getConditionalLinesFlag(void)
	{
		return m_flags.conditionalLines != false;
	}
	void setSmoothCurvesFlag(bool value) { m_flags.smoothCurves = value; }
	bool getSmoothCurvesFlag(void) { return m_flags.smoothCurves != false; }
	void setShowAllConditionalFlag(bool value)
	{
		m_flags.showAllConditional = value;
	}
	bool getShowAllConditionalFlag(void)
	{
		return m_flags.showAllConditional != false;
	}
	void setConditionalControlPointsFlag(bool value)
	{
		m_flags.conditionalControlPoints = value;
	}
	bool getConditionalControlPointsFlag(void)
	{
		return m_flags.conditionalControlPoints != false;
	}
	void setCompilePartsFlag(bool value) { m_flags.compileParts = value; }
	bool getCompilePartsFlag(void) { return m_flags.compileParts != false; }
	void setCompileAllFlag(bool value) { m_flags.compileAll = value; }
	bool getCompileAllFlag(void) { return m_flags.compileAll != false; }
	void setPolygonOffsetFlag(bool value) { m_flags.polygonOffset = value; }
	bool getPolygonOffsetFlag(void) { return m_flags.polygonOffset != false; }
	void setStudLogoFlag(bool value) { m_flags.studLogo = value; }
	bool getStudLogoFlag(void) { return m_flags.studLogo != false; }
	void setCurveQuality(int value) { m_curveQuality = value; }
	int getCurveQuality(void) { return m_curveQuality; }
	void setEdgeLineWidth(float value) { m_edgeLineWidth = value; }
	float getEdgeLineWidth(void) { return m_edgeLineWidth; }
	void setSeamWidth(float seamWidth);
	float getSeamWidth(void);
	void setDefaultRGB(TCByte r, TCByte g, TCByte b, bool transparent);
	void setDefaultColorNumber(int colorNumber);
	void setStudTextureFilter(int value) { m_studTextureFilter = value; }
	int getStudTextureFilter(void) { return m_studTextureFilter; }
	virtual bool shouldLoadConditionalLines(void);
protected:
	virtual ~LDModelParser(void);
	virtual void dealloc(void);
	void setSeamsFlag(bool value) { m_flags.seams = value; }
	void setDefaultColorSetFlag(bool value) { m_flags.defaultColorSet = value; }
	bool getDefaultColorSetFlag(void)
	{
		return m_flags.defaultColorSet != false;
	}
	void setDefaultColorNumberSetFlag(bool value)
	{
		m_flags.defaultColorNumberSet = value;
	}
	bool getDefaultColorNumberSetFlag(void)
	{
		return m_flags.defaultColorNumberSet != false;
	}
	virtual bool parseModel(LDLModel *ldlModel, TREModel *treModel, bool bfc);
	virtual bool parseModel(LDLModelLine *modelLine, TREModel *treModel,
		bool bfc);
	virtual void parseLine(LDLShapeLine *shapeLine, TREModel *treModel);
	virtual void parseTriangle(LDLShapeLine *shapeLine, TREModel *treModel,
		bool bfc, bool invert);
	virtual void parseQuad(LDLShapeLine *shapeLine, TREModel *treModel,
		bool bfc, bool invert);
	virtual void parseConditionalLine(LDLConditionalLineLine *conditionalLine,
		TREModel *treModel);
	virtual bool addSubModel(LDLModelLine *modelLine, TREModel *treParentModel,
		TREModel *treModel, bool invert);
	virtual bool performPrimitiveSubstitution(LDLModel *ldlModel,
		TREModel *treModel, bool bfc);
	virtual float getTorusFraction(int size);
	virtual bool substituteStud(TREModel *treModel, int numSegments);
	virtual bool substituteStud(TREModel *treModel);
	virtual bool substituteStu2(TREModel *treModel);
	virtual bool substituteStu22(TREModel *treModel, bool isA, bool bfc);
	virtual bool substituteStu23(TREModel *treModel, bool isA, bool bfc);
	virtual bool substituteStu24(TREModel *treModel, bool isA, bool bfc);
	virtual bool substituteTorusIO(TREModel *treModel, bool inner, bool bfc,
		bool is48 = false);
	virtual bool substituteTorusQ(TREModel *treModel, bool bfc,
		bool is48 = false);
	virtual bool substituteEighthSphere(TREModel *treModel, bool bfc,
		bool is48 = false);
	virtual bool substituteCylinder(TREModel *treModel, float fraction,
		bool bfc, bool is48 = false);
	virtual bool substituteSlopedCylinder(TREModel *treModel, float fraction,
		bool bfc, bool is48 = false);
	virtual bool substituteSlopedCylinder2(TREModel *treModel, float fraction,
		bool bfc, bool is48 = false);
	virtual bool substituteChrd(TREModel *treModel, float fraction, bool bfc,
		bool is48 = false);
	virtual bool substituteDisc(TREModel *treModel, float fraction, bool bfc,
		bool is48 = false);
	virtual bool substituteNotDisc(TREModel *treModel, float fraction,
		bool bfc, bool is48 = false);
	virtual bool substituteCircularEdge(TREModel *treModel, float fraction,
		bool is48 = false);
	virtual bool substituteCone(TREModel *treModel, float fraction, int size,
		bool bfc, bool is48 = false);
	virtual bool substituteRing(TREModel *treModel, float fraction, int size,
		bool bfc, bool is48 = false);
	virtual float startingFraction(const char *filename);
	virtual bool startsWithFraction(const char *filename);
	virtual bool startsWithFraction2(const char *filename);
	virtual bool isPrimitive(const char *filename, const char *suffix,
		bool *is48 = NULL);
	virtual bool isCyli(const char *filename, bool *is48 = NULL);
	virtual bool isCyls(const char *filename, bool *is48 = NULL);
	virtual bool isCyls2(const char *filename, bool *is48 = NULL);
	virtual bool isChrd(const char *filename, bool *is48 = NULL);
	virtual bool isDisc(const char *filename, bool *is48 = NULL);
	virtual bool isNdis(const char *filename, bool *is48 = NULL);
	virtual bool isEdge(const char *filename, bool *is48 = NULL);
//	virtual bool isCcyli(const char *filename, bool *is48 = NULL);
	virtual bool is1DigitCon(const char *filename, bool *is48 = NULL);
	virtual bool is2DigitCon(const char *filename, bool *is48 = NULL);
	virtual bool isCon(const char *filename, bool *is48 = NULL);
	virtual bool isOldRing(const char *filename, bool *is48 = NULL);
	virtual bool isRing(const char *filename, bool *is48 = NULL);
	virtual bool isRin(const char *filename, bool *is48 = NULL);
	virtual bool isTorus(const char *filename, bool *is48 = NULL);
	virtual bool isTorusO(const char *filename, bool *is48 = NULL);
	virtual bool isTorusI(const char *filename, bool *is48 = NULL);
	virtual bool isTorusQ(const char *filename, bool *is48 = NULL);
	virtual int getNumCircleSegments(float fraction = 0.0f, bool is48 = false);
	virtual void finishPart(TREModel *treModel, TRESubModel *subModel = NULL);
	virtual bool shouldFlipWinding(bool invert, bool windingCCW);

	LDLMainModel *m_mainLDLModel;
	TREMainModel *m_mainTREModel;
	int m_curveQuality;
	float m_seamWidth;
	float m_edgeLineWidth;
	TCByte m_defaultR;
	TCByte m_defaultG;
	TCByte m_defaultB;
	int m_defaultColorNumber;
	bool m_abort;	// Easier to not be a bit field.
	int m_studTextureFilter;
	struct
	{
		bool fileIsPart:1;
		bool flattenParts:1;
		bool primitiveSubstitution:1;
		bool seams:1;
		bool edgeLines:1;
		bool edgesOnly:1;
		bool twoSidedLighting:1;
		bool bfc:1;
		bool compileParts:1;
		bool compileAll:1;
		bool lighting:1;
		bool aaLines:1;
		bool sortTransparent:1;
		bool stipple:1;
		bool wireframe:1;
		bool conditionalLines:1;
		bool smoothCurves:1;
		bool showAllConditional:1;
		bool conditionalControlPoints:1;
		bool defaultColorSet:1;
		bool defaultColorNumberSet:1;
		bool polygonOffset:1;
		bool studLogo:1;
		bool defaultTrans:1;
		bool redBackFaces:1;
		bool greenFrontFaces:1;
	} m_flags;
};

#endif // __LDMODELPARSER_H__
