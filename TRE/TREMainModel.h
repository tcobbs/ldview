#ifndef __TREMAINMODEL_H__
#define __TREMAINMODEL_H__

#include <TRE/TREModel.h>

class TCDictionary;
class TREVertexStore;
class TREColoredShapeGroup;
class TRETransShapeGroup;

class TREMainModel : public TREModel
{
public:
	TREMainModel(void);
	TREMainModel(const TREMainModel &other);
	virtual TCObject *copy(void);
	virtual TCDictionary* getLoadedModels(bool bfc);
	virtual void draw(void);
	virtual TREVertexStore *getVertexStore(void) { return m_vertexStore; }
	virtual TREModel *modelNamed(const char *name, bool bfc);
	virtual void registerModel(TREModel *model, bool bfc);
	void setCompilePartsFlag(bool value) { m_mainFlags.compileParts = value; }
	bool getCompilePartsFlag(void) { return m_mainFlags.compileParts != false; }
	void setCompileAllFlag(bool value) { m_mainFlags.compileAll = value; }
	bool getCompileAllFlag(void) { return m_mainFlags.compileAll != false; }
	void setEdgeLinesFlag(bool value) { m_mainFlags.edgeLines = value; }
	bool getEdgeLinesFlag(void) { return m_mainFlags.edgeLines != false; }
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
	void postProcess(void);
	void compile(void);
	void recompile(void);
	virtual void addTransparentTriangle(TCULong color,
		const TCVector vertices[], const TCVector normals[]);
protected:
	virtual ~TREMainModel(void);
	virtual void dealloc(void);
	void scanMaxRadiusSquaredPoint(const TCVector &point);
	virtual void activateBFC(void);
	virtual void deactivateBFC(void);
	void transferTransparent(void);
	virtual void drawTransparent(void);
	virtual void drawLines(void);
	virtual void compileTransparent(void);

	TCDictionary *m_loadedModels;
	TCDictionary *m_loadedBFCModels;
	TREVertexStore *m_vertexStore;
	TREVertexStore *m_coloredVertexStore;
	TREVertexStore *m_transVertexStore;
	TRETransShapeGroup *m_transShapes;
	TCULong m_color;
	TCULong m_edgeColor;
	float m_maxRadiusSquared;
	TCVector m_center;
	int m_transListID;
	struct
	{
		bool compileParts:1;
		bool compileAll:1;
		bool compiled:1;
		bool edgeLines:1;
		bool twoSidedLighting:1;
		bool lighting:1;
		bool useStrips:1;
		bool useFlatStrips:1;
		bool bfc:1;
		bool aaLines:1;
		bool sortTransparent:1;
	} m_mainFlags;
};

#endif // __TREMAINMODEL_H__
