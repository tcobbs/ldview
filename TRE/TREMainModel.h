#ifndef __TREMAINMODEL_H__
#define __TREMAINMODEL_H__

#include <TRE/TREModel.h>
#include <TCFoundation/TCImage.h>

class TCDictionary;
class TREVertexStore;
class TREColoredShapeGroup;
class TRETransShapeGroup;
class TCImage;

extern const float POLYGON_OFFSET_FACTOR;
extern const float POLYGON_OFFSET_UNITS;

class TREMainModel : public TREModel
{
public:
	TREMainModel(void);
	TREMainModel(const TREMainModel &other);
	virtual TCObject *copy(void);
	virtual TCDictionary* getLoadedModels(bool bfc);
	virtual void draw(void);
	virtual TREVertexStore *getVertexStore(void) { return m_vertexStore; }
	virtual TREVertexStore *getStudVertexStore(void)
	{
		return m_studVertexStore;
	}
	virtual TREVertexStore *getColoredStudVertexStore(void)
	{
		return m_coloredStudVertexStore;
	}
	virtual TREModel *modelNamed(const char *name, bool bfc);
	virtual void registerModel(TREModel *model, bool bfc);
	void setCompilePartsFlag(bool value) { m_mainFlags.compileParts = value; }
	bool getCompilePartsFlag(void) { return m_mainFlags.compileParts != false; }
	void setCompileAllFlag(bool value) { m_mainFlags.compileAll = value; }
	bool getCompileAllFlag(void) { return m_mainFlags.compileAll != false; }
	void setEdgeLinesFlag(bool value) { m_mainFlags.edgeLines = value; }
	bool getEdgeLinesFlag(void) { return m_mainFlags.edgeLines != false; }
	void setEdgesOnlyFlag(bool value) { m_mainFlags.edgesOnly = value; }
	bool getEdgesOnlyFlag(void)
	{
		return m_mainFlags.edgesOnly != false && getEdgeLinesFlag();
	}
	void setTwoSidedLightingFlag(bool value);
	bool getTwoSidedLightingFlag(void)
	{
		return m_mainFlags.twoSidedLighting != false;
	}
	void setLightingFlag(bool value);
	bool getLightingFlag(void) { return m_mainFlags.lighting != false; }
	void setUseStripsFlag(bool value) { m_mainFlags.useStrips = value; }
	bool getUseStripsFlag(void) { return m_mainFlags.useStrips != false; }
	void setUseFlatStripsFlag(bool value) { m_mainFlags.useFlatStrips = value; }
	bool getUseFlatStripsFlag(void)
	{
		return m_mainFlags.useFlatStrips != false;
	}
	void setBFCFlag(bool value) { m_mainFlags.bfc = value; }
	bool getBFCFlag(void) { return m_mainFlags.bfc != false; }
	void setAALinesFlag(bool value) { m_mainFlags.aaLines = value; }
	bool getAALinesFlag(void) { return m_mainFlags.aaLines != false; }
	void setSortTransparentFlag(bool value)
	{
		m_mainFlags.sortTransparent = value;
	}
	bool getSortTransparentFlag(void)
	{
		return m_mainFlags.sortTransparent != false;
	}
	void setStippleFlag(bool value) { m_mainFlags.stipple = value; }
	bool getStippleFlag(void) { return m_mainFlags.stipple != false; }
	void setWireframeFlag(bool value) { m_mainFlags.wireframe = value; }
	bool getWireframeFlag(void) { return m_mainFlags.wireframe != false; }
	void setConditionalLinesFlag(bool value)
	{
		m_mainFlags.conditionalLines = value;
	}
	bool getConditionalLinesFlag(void)
	{
		return m_mainFlags.conditionalLines != false && getEdgeLinesFlag();
	}
	void setSmoothCurvesFlag(bool value) { m_mainFlags.smoothCurves = value; }
	bool getSmoothCurvesFlag(void) { return m_mainFlags.smoothCurves != false; }
	void setShowAllConditionalFlag(bool value);
	bool getShowAllConditionalFlag(void)
	{
		return m_mainFlags.showAllConditional != false &&
			getConditionalLinesFlag();
	}
	void setConditionalControlPointsFlag(bool value);
	bool getConditionalControlPointsFlag(void)
	{
		return m_mainFlags.conditionalControlPoints != false &&
			getConditionalLinesFlag();
	}
	void setPolygonOffsetFlag(bool value)
	{
		m_mainFlags.polygonOffset = value;
	}
	bool getPolygonOffsetFlag(void)
	{
		return m_mainFlags.polygonOffset != false;
	}
	void setStudLogoFlag(bool value) { m_mainFlags.studLogo = value; }
	bool getStudLogoFlag(void) { return m_mainFlags.studLogo != false; }
	void setRemovingHiddenLines(bool value)
	{
		m_mainFlags.removingHiddenLines = value;
	}
	bool getRemovingHiddenLines(void)
	{
		return m_mainFlags.removingHiddenLines != false;
	}
	void setCutawayDrawFlag(bool value) { m_mainFlags.cutawayDraw = value; }
	bool getCutawayDrawFlag(void) { return m_mainFlags.cutawayDraw != false; }
	void setEdgeLineWidth(float value) { m_edgeLineWidth = value; }
	float getEdgeLineWidth(void) { return m_edgeLineWidth; }
	void setStudTextureFilter(int value) { m_studTextureFilter = value; }
	int getStudTextureFilter(void) { return m_studTextureFilter; }
	virtual bool getCompiled(void) { return m_mainFlags.compiled != false; }
	virtual bool getCompiling(void) { return m_mainFlags.compiling != false; }
	virtual float getMaxRadiusSquared(const TCVector &center);
	virtual float getMaxRadius(const TCVector &center);
	TREVertexStore *getColoredVertexStore(void)
	{
		return m_coloredVertexStore;
	}
	TREVertexStore *getTransVertexStore(void)
	{
		return m_transVertexStore;
	}
	void setColor(TCULong color, TCULong edgeColor);
	TCULong getColor(void);
	TCULong getEdgeColor(void);
	bool postProcess(void);
	void compile(void);
	void recompile(void);
	virtual void addTransparentTriangle(TCULong color,
		const TCVector vertices[], const TCVector normals[]);
	virtual bool shouldLoadConditionalLines(void);
	bool isStudSection(TREMSection section)
	{
		return section == TREMStud || section == TREMStudBFC;
	}

	static void loadStudTexture(const char *filename);
	static void setStudTextureData(TCByte *data, long length);
	static void setRawStudTextureData(TCByte *data, long length);
	static TCImageArray *getStudTextures(void) { return sm_studTextures; }
	static unsigned getStudTextureID(void) { return sm_studTextureID; }
protected:
	virtual ~TREMainModel(void);
	virtual void dealloc(void);
	void scanMaxRadiusSquaredPoint(const TCVector &point);
	virtual void activateBFC(void);
	virtual void deactivateBFC(void);
	void transferTransparent(void);
	virtual void drawTransparent(void);
	virtual void drawLines(void);
	virtual void drawSolid(void);
	virtual void enableLineSmooth(void);
	virtual void bindStudTexture(void);
	virtual void configureStudTexture(void);
	virtual bool shouldCompileSection(TREMSection section);

	static void loadStudMipTextures(TCImage *mainImage);

	TCDictionary *m_loadedModels;
	TCDictionary *m_loadedBFCModels;
	TREVertexStore *m_vertexStore;
	TREVertexStore *m_studVertexStore;
	TREVertexStore *m_coloredVertexStore;
	TREVertexStore *m_coloredStudVertexStore;
	TREVertexStore *m_transVertexStore;
	TCULong m_color;
	TCULong m_edgeColor;
	float m_maxRadiusSquared;
	TCVector m_center;
	float m_edgeLineWidth;
	bool m_abort;				// Easier not to be a bit field.
	int m_studTextureFilter;
	struct
	{
		// The following are temporal
		bool compiling:1;
		bool compiled:1;
		bool removingHiddenLines:1;	// This one is changed externally
		bool cutawayDraw:1;			// This one is changed externally
		// The following aren't temporal
		bool compileParts:1;
		bool compileAll:1;
		bool edgeLines:1;
		bool edgesOnly:1;
		bool twoSidedLighting:1;
		bool lighting:1;
		bool useStrips:1;
		bool useFlatStrips:1;
		bool bfc:1;
		bool aaLines:1;
		bool sortTransparent:1;
		bool stipple:1;
		bool wireframe:1;
		bool conditionalLines:1;
		bool smoothCurves:1;
		bool showAllConditional:1;
		bool conditionalControlPoints:1;
		bool polygonOffset:1;
		bool studLogo:1;
	} m_mainFlags;

	static TCImageArray *sm_studTextures;
	static unsigned sm_studTextureID;
	static class TREMainModelCleanup
	{
	public:
		~TREMainModelCleanup(void);
	} sm_mainModelCleanup;
	friend class TREMainModelCleanup;
};

#endif // __TREMAINMODEL_H__
