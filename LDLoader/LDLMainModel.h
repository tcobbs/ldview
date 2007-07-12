#ifndef __LDLMAINMODEL_H__
#define __LDLMAINMODEL_H__

#include <LDLoader/LDLModel.h>
#include <TCFoundation/TCStlIncludes.h>

class LDLPalette;

typedef std::map<std::string, bool> StringBoolMap;

class LDLMainModel : public LDLModel
{
public:
	LDLMainModel(void);
	LDLMainModel(const LDLMainModel &other);
	TCObject *copy(void);
	virtual bool load(const char *filename);
	virtual TCDictionary* getLoadedModels(void);
	virtual void print(void);
	virtual TCULong getEdgeColorNumber(TCULong colorNumber);
	virtual void getRGBA(TCULong colorNumber, int& r, int& g, int& b, int& a);
	virtual bool hasSpecular(TCULong colorNumber);
	virtual bool hasShininess(TCULong colorNumber);
	virtual void getSpecular(TCULong colorNumber, float *specular);
	virtual void getShininess(TCULong colorNumber, float &shininess);
	virtual bool colorNumberIsTransparent(TCULong colorNumber);
	virtual LDLPalette *getPalette(void) { return m_mainPalette; }
	virtual void setExtraSearchDirs(TCStringArray *value);
	TCStringArray *getExtraSearchDirs(void) { return m_extraSearchDirs; }
	virtual bool isMainModel(void) const { return true; }

	// Flags
	void setLowResStuds(bool value) { m_mainFlags.lowResStuds = value; }
	bool getLowResStuds(void) const { return m_mainFlags.lowResStuds; }
	void setBlackEdgeLines(bool value) { m_mainFlags.blackEdgeLines = value; }
	bool getBlackEdgeLines(void) { return m_mainFlags.blackEdgeLines; }
	void setProcessLDConfig(bool value) { m_mainFlags.processLDConfig = value; }
	bool getProcessLDConfig(void)
	{
		return m_mainFlags.processLDConfig != false;
	}
	void setSkipValidation(bool value) { m_mainFlags.skipValidation = value; }
	bool getSkipValidation(void) { return m_mainFlags.skipValidation != false; }
	virtual void cancelLoad(void) { m_mainFlags.loadCanceled = true; }
	virtual bool getLoadCanceled(void)
	{
		return m_mainFlags.loadCanceled != false;
	}
	bool &ancestorCheck(const std::string &name) { return m_ancestorMap[name]; }
	void ancestorClear(const std::string &name) { m_ancestorMap[name] = false; }
protected:
	virtual void dealloc(void);
	virtual void processLDConfig(void);

	TCDictionary *m_loadedModels;
	LDLPalette *m_mainPalette;
	TCStringArray *m_extraSearchDirs;
	struct
	{
		// Public flags
		bool lowResStuds:1;
		bool blackEdgeLines:1;
		bool processLDConfig:1;
		bool skipValidation:1;
		// Semi-public flags
		bool loadCanceled:1;
	} m_mainFlags;
	StringBoolMap m_ancestorMap;
};

#endif // __LDLMAINMODEL_H__
