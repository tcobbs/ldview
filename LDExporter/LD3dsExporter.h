#ifndef __LD3DSEXPORTER_H__
#define __LD3DSEXPORTER_H__

#include "LDExporter.h"
#include <TCFoundation/TCTypedValueArray.h>

struct Lib3dsFile;
struct Lib3dsMesh;
struct Lib3dsNode;

class LDLShapeLine;

class TREVertexArray;
class TREShapeGroup;
enum TREShapeType;

class LD3dsExporter : public LDExporter
{
public:
	LD3dsExporter(void);
	int doExport(LDLModel *pTopModel);
	virtual bool usesLDLModel(void) const { return true; }
	virtual bool usesTREModel(void) const { return false; }
	virtual std::string getExtension(void) const { return "3ds"; }
	virtual ucstring getTypeDescription(void) const;
protected:
	typedef std::map<TCULong, int> ULongIntMap;
	typedef std::map<int, int> IntIntMap;
	~LD3dsExporter(void);
	void dealloc(void);
	virtual void initSettings(void) const;
	void doExport(LDLModel *pModel, const TCFloat *matrix,
		int colorNumber);
	int getMaterial(int colorNumber);
	void writeShapeLine(Lib3dsMesh *mesh, LDLShapeLine *pShapeLine,
		const TCFloat *matrix, int colorNumber);
	void writeTriangle(Lib3dsMesh *mesh, const TCVector *points, int i0,
		int i1, int i2, int colorNumber, const TCFloat *matrix);

	// NOTE: loadSettings is NOT virtual: it's called from the constructor.
	void loadSettings(void);

	ULongIntMap m_colors;
	IntIntMap m_colorNumbers;
	int m_numMeshes;
	Lib3dsFile *m_file;
	LDLModel *m_topModel;
};

#endif // __LD3DSEXPORTER_H__
