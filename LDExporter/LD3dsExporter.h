#ifndef __LD3DSEXPORTER_H__
#define __LD3DSEXPORTER_H__

#ifdef EXPORT_3DS

#include "LDExporter.h"
#include <TCFoundation/TCTypedValueArray.h>

struct Lib3dsFile;
struct Lib3dsMesh;
struct Lib3dsNode;
struct Lib3dsFace;

class LDLShapeLine;

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
	struct Vertex
	{
		float v[3];
	};
	typedef std::vector<Vertex> VertexVector;
	typedef std::vector<Lib3dsFace> FaceVector;
	typedef std::map<int, int> IntIntMap;
	typedef std::map<std::string, int> StringIntMap;
	typedef std::map<std::string, Lib3dsMesh *> StringMeshMap;
	~LD3dsExporter(void);
	void dealloc(void);
	virtual void initSettings(void) const;
	void doExport(LDLModel *pModel, Lib3dsNode *pParentNode,
		const TCFloat *matrix, int colorNumber, bool inPart, bool bfc,
		bool invert);
	int getMaterial(int colorNumber);
	void writeShapeLine(VertexVector &vecVertices, FaceVector &vecFaces,
		LDLShapeLine *pShapeLine, const TCFloat *matrix, int colorNumber,
		bool bfc, bool invert);
	void writeTriangle(VertexVector &vecVertices, FaceVector &vecFaces,
		const TCVector *points, int i0, int i1, int i2, int colorNumber,
		const TCFloat *matrix);
	std::string getMeshName(LDLModel *model, Lib3dsMesh *&pMesh);
	bool shouldFlipWinding(bool bfc, bool invert, LDLShapeLine *pShapeLine);

	// NOTE: loadSettings is NOT virtual: it's called from the constructor.
	void loadSettings(void);

	StringIntMap m_names;
	StringMeshMap m_meshes;
	IntIntMap m_colorNumbers;
	Lib3dsFile *m_file;
	LDLModel *m_topModel;
	bool m_seams;
	float m_seamWidth;
	int m_meshCount;
	//int m_meshNameCount;
	//bool m_includeCamera;
};

#endif // EXPORT_3DS
#endif // __LD3DSEXPORTER_H__
