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
	bool getCompileParts(void) { return m_mainFlags.compileParts; }
	bool getCompileAll(void) { return m_mainFlags.compileAll; }
	TREVertexStore *getColoredVertexStore(void)
	{
		return m_coloredVertexStore;
	}
protected:
	virtual ~TREMainModel(void);
	virtual void dealloc(void);

	TCDictionary *m_loadedModels;
	TREVertexStore *m_vertexStore;
	TREVertexStore *m_coloredVertexStore;
	struct
	{
		bool compileParts:1;
		bool compileAll:1;
		bool compiled:1;
	} m_mainFlags;
};

#endif // __TREMAINMODEL_H__
