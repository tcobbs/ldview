#ifndef __LDMODELPARSER_H__
#define __LDMODELPARSER_H__

#include <TCFoundation/TCObject.h>
#include <LDLoader/LDLPrimitiveCheck.h>

#include <string.h>

class LDLMainModel;
class LDLModel;
class TREMainModel;
class TREModel;
class TRESubModel;
class LDLShapeLine;
class LDLModelLine;
class LDLConditionalLineLine;

class LDModelParser : public LDLPrimitiveCheck
{
public:
	LDModelParser(void);
	virtual bool parseMainModel(LDLMainModel *mainLDLModel);
	TREMainModel *getMainTREModel(void) { return m_mainTREModel; }
	void setFileIsPartFlag(bool value) { m_flags.fileIsPart = value; }
	bool getFileIsPartFlag(void) { return m_flags.fileIsPart != false; }
	void setFlattenPartsFlag(bool value) { m_flags.flattenParts = value; }
	bool getFlattenPartsFlag(void) { return m_flags.flattenParts != false; }
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
	void setEdgeLineWidth(TCFloat32 value) { m_edgeLineWidth = value; }
	TCFloat32 getEdgeLineWidth(void) { return m_edgeLineWidth; }
	void setStudAnisoLevel(TCFloat32 value) { m_studAnisoLevel = value; }
	TCFloat32 getStudAnisoLevel(void) { return m_studAnisoLevel; }
	void setSeamWidth(TCFloat seamWidth);
	TCFloat getSeamWidth(void);
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
	virtual TCFloat getTorusFraction(int size);
	virtual bool substituteStud(int numSegments);
	virtual bool substituteStud(void);
	virtual bool substituteStu2(void);
	virtual bool substituteStu22(bool isA, bool bfc);
	virtual bool substituteStu23(bool isA, bool bfc);
	virtual bool substituteStu24(bool isA, bool bfc);
	virtual bool substituteTorusIO(bool inner, bool bfc,
		bool is48 = false);
	virtual bool substituteTorusQ(bool bfc,
		bool is48 = false);
	virtual bool substituteEighthSphere(bool bfc,
		bool is48 = false);
	virtual bool substituteCylinder(TCFloat fraction,
		bool bfc, bool is48 = false);
	virtual bool substituteSlopedCylinder(TCFloat fraction,
		bool bfc, bool is48 = false);
	virtual bool substituteSlopedCylinder2(TCFloat fraction,
		bool bfc, bool is48 = false);
	virtual bool substituteChrd(TCFloat fraction, bool bfc,
		bool is48 = false);
	virtual bool substituteDisc(TCFloat fraction, bool bfc,
		bool is48 = false);
	virtual bool substituteNotDisc(TCFloat fraction,
		bool bfc, bool is48 = false);
	virtual bool substituteCircularEdge(TCFloat fraction,
		bool is48 = false);
	virtual bool substituteCone(TCFloat fraction, int size,
		bool bfc, bool is48 = false);
	virtual bool substituteRing(TCFloat fraction, int size,
		bool bfc, bool is48 = false);
	virtual void finishPart(TREModel *treModel, TRESubModel *subModel = NULL);
	virtual bool shouldFlipWinding(bool invert, bool windingCCW);

	LDLMainModel *m_mainLDLModel;
	TREMainModel *m_mainTREModel;
	TREModel *m_currentTREModel;
	TCFloat m_seamWidth;
	TCFloat32 m_edgeLineWidth;
	TCFloat32 m_studAnisoLevel;
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
