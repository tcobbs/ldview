#ifndef __TREMAINMODEL_H__
#define __TREMAINMODEL_H__

#include <TRE/TREModel.h>

class TCDictionary;

class TREMainModel : public TREModel
{
public:
	TREMainModel(void);
	TREMainModel(const TREMainModel &other);
	TCObject *copy(void);
	virtual TCDictionary* getLoadedModels(void);
	virtual void draw(void);
protected:
	virtual ~TREMainModel(void);
	virtual void dealloc(void);

	TCDictionary *m_loadedModels;
	struct
	{
	} m_mainFlags;
};

#endif // __TREMAINMODEL_H__
