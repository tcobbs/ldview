#ifndef __TREMAINMODEL_H__
#define __TREMAINMODEL_H__

#include <TRE/TREModel.h>

class TCDictionary;
class TREVertexStore;

class TREMainModel : public TREModel
{
public:
	TREMainModel(void);
	TREMainModel(const TREMainModel &other);
	virtual TCObject *copy(void);
	virtual TCDictionary* getLoadedModels(void);
	virtual void draw(void);
	virtual TREVertexStore *getVertexStore(void) { return m_vertexStore; }
	virtual TREModel *modelNamed(const char *name);
	virtual void registerModel(TREModel *model);
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
	virtual float getMaxRadiusSquared(const TCVector &center);
	virtual float getMaxRadius(const TCVector &center);
	TREVertexStore *getColoredVertexStore(void)
	{
		return m_coloredVertexStore;
	}
	void setColor(TCULong color, TCULong edgeColor);
	TCULong getColor(void);
	TCULong getEdgeColor(void);
	void postProcess(void);
	void compile(void);
	void recompile(void);
protected:
	virtual ~TREMainModel(void);
	virtual void dealloc(void);
	void scanMaxRadiusSquaredPoint(const TCVector &point);
	virtual void activateBFC(void);
	virtual void deactivateBFC(void);

	TCDictionary *m_loadedModels;
	TREVertexStore *m_vertexStore;
	TREVertexStore *m_coloredVertexStore;
	TCULong m_color;
	TCULong m_edgeColor;
	float m_maxRadiusSquared;
	TCVector m_center;
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
	} m_mainFlags;
};

#endif // __TREMAINMODEL_H__
