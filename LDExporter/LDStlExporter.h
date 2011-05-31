#ifndef __LDSTLEXPORTER_H__
#define __LDSTLEXPORTER_H__

#include "LDExporter.h"

class LDStlExporter : public LDExporter
{
public:
	LDStlExporter(void);
	int doExport(TREModel *pTopModel);
	virtual bool usesLDLModel(void) const { return false; }
	virtual bool usesTREModel(void) const { return true; }
	virtual std::string getExtension(void) const { return "stl"; }
	virtual ucstring getTypeDescription(void) const;
protected:
	~LDStlExporter(void);
	void dealloc(void);
	virtual void initSettings(void) const;

	// NOTE: loadSettings is NOT virtual: it's called from the constructor.
	void loadSettings(void);

	bool m_binary;
	long m_colorFormat;
	long m_scaleSelection;
};

#endif // __LDSTLEXPORTER_H__
