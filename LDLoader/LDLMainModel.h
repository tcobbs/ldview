#ifndef __LDLMAINMODEL_H__
#define __LDLMAINMODEL_H__

#include <LDLoader/LDLModel.h>

class LDLMainModel : public LDLModel
{
public:
	LDLMainModel(void);
	LDLMainModel(const LDLMainModel &other);
	TCObject *copy(void);
	virtual bool load(const char *filename);
	virtual TCDictionary* getLoadedModels(void);
	virtual void print(void);

	// Flags
	void setLowResStuds(bool value) { m_mainFlags.lowResStuds = value; }
	bool getLowResStuds(void) const { return m_mainFlags.lowResStuds; }
protected:
	virtual void dealloc(void);

	TCDictionary *m_loadedModels;
	struct
	{
		// Public flags
		bool lowResStuds:1;
	} m_mainFlags;
};

#endif // __LDLMAINMODEL_H__
