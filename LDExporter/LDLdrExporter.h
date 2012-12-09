#ifndef __LDLDREXPORTER_H__
#define __LDLDREXPORTER_H__

#include "LDExporter.h"
#include <TCFoundation/TCTypedValueArray.h>

class TREShapeGroup;
class TREVertexArray;

class LDLdrExporter : public LDExporter
{
public:
	LDLdrExporter(void);
	int doExport(TREModel *pTopModel);
	virtual bool usesLDLModel(void) const { return false; }
	virtual bool usesTREModel(void) const { return true; }
	virtual std::string getExtension(void) const { return "ldr"; }
	virtual ucstring getTypeDescription(void) const;
protected:
	~LDLdrExporter(void);
	void dealloc(void);
	virtual void initSettings(void) const;
	int exportModel(TREModel *pModel, const TCFloat *pMatrix);
	void exportShapes(TREShapeGroup **ppShapes, const TCFloat *pMatrix);
	void exportTriangle(TREVertexArray *vertices, TCULongArray *indices, int ix,
		int i0, int i1, int i2, const TCFloat *pMatrix);
	void exportQuad(TREVertexArray *vertices, TCULongArray *indices, int ix,
		int i0, int i1, int i2, int i3, const TCFloat *pMatrix);
	void exportConditionalLine(TREVertexArray *vertices, TCULongArray *indices,
		TCULongArray *cpIndices, int ix, int i0, int i1,
		const TCFloat *pMatrix);
	void exportLine(TREVertexArray *vertices, TCULongArray *indices,
		int ix, int i0, int i1, const TCFloat *pMatrix);
	void exportStrips(TREShapeGroup *shapeGroup, int nShapeType,
		const TCFloat *pMatrix);

	// NOTE: loadSettings is NOT virtual: it's called from the constructor.
	void loadSettings(void);
	
	FILE *m_pFile;
};

#endif // __LDLDREXPORTER_H__
