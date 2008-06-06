#ifndef __LDSTLEXPORTER_H__
#define __LDSTLEXPORTER_H__

#include "LDExporter.h"

class LDStlExporter : public LDExporter
{
public:
	LDStlExporter(void);
	int doExport(TREModel *pTopModel);
	virtual bool usesLDLModel(void) { return false; }
	virtual bool usesTREModel(void) { return true; }
	virtual std::string getExtension(void) const { return "stl"; }
	virtual ucstring getTypeDescription(void) const;
protected:
	~LDStlExporter(void);
	void dealloc(void);
	virtual void initSettings(void) const;

	// NOTE: loadDefaults is NOT virtual: it's called from the constructor.
	void loadDefaults(void);
};

#endif // __LDSTLEXPORTER_H__
