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
	TCObject *copy(void);
	virtual TCDictionary* getLoadedModels(void);
	virtual void draw(void);
	virtual TREVertexStore *getVertexStore(void) { return m_vertexStore; }
	virtual TREModel *modelNamed(const char *name);
	virtual void registerModel(TREModel *model);
	bool getCompilePartsFlag(void) { return m_mainFlags.compileParts; }
	bool getCompileAllFlag(void) { return m_mainFlags.compileAll; }
	void setEdgeLinesFlag(bool value) { m_mainFlags.edgeLines = value; }
	bool getEdgeLinesFlag(void) { return m_mainFlags.edgeLines; }
	void setTwoSidedLightingFlag(bool value);
	bool getTwoSidedLightingFlag(void) { return m_mainFlags.twoSidedLighting; }
	TREVertexStore *getColoredVertexStore(void)
	{
		return m_coloredVertexStore;
	}
	void setColor(TCULong color, TCULong edgeColor);
	TCULong getColor(void);
	TCULong getEdgeColor(void);
protected:
	virtual ~TREMainModel(void);
	virtual void dealloc(void);

	TCDictionary *m_loadedModels;
	TREVertexStore *m_vertexStore;
	TREVertexStore *m_coloredVertexStore;
	TCULong m_color;
	TCULong m_edgeColor;
	struct
	{
		bool compileParts:1;
		bool compileAll:1;
		bool compiled:1;
		bool edgeLines:1;
		bool twoSidedLighting:1;
	} m_mainFlags;
};

#endif // __TREMAINMODEL_H__
