#ifndef __LDPOVEXPORTER_H__
#define __LDPOVEXPORTER_H__

#include "LDExporter.h"
#include <map>
#include <list>
#include <TCFoundation/TCDefines.h>
#include <TCFoundation/TCVector.h>

class LDLModel;
class LDLFileLine;
class LDLModelLine;
class LDLTriangleLine;
class LDLQuadLine;
class LDLLineLine;
class LDLCommentLine;
class LDLShapeLine;

typedef std::map<std::string, bool> StringBoolMap;
typedef std::map<std::string, std::string> StringStringMap;
typedef std::list<LDLShapeLine *> ShapeLineList;
typedef std::map<int, ShapeLineList> IntShapeLineListMap;
typedef std::map<int, bool> IntBoolMap;
typedef std::list<std::string> StringList;
typedef std::list<TCVector> VectorList;

class LDPovExporter : public LDExporter
{
public:
	LDPovExporter(void);
	int doExport(LDLModel *pTopModel);
protected:
	~LDPovExporter(void);
	void dealloc(void);
	bool writeHeader(void);
	bool writeModel(LDLModel *pModel);
	bool writeCamera(void);
	bool writeLights(void);
	void writeLight(TCFloat lat, TCFloat lon, TCFloat radius);
	bool writeModelObject(LDLModel *pModel);
	void writeGeometry(const IntShapeLineListMap &colorGeometryMap);
	bool scanModelColors(LDLModel *pModel);
	bool writeModelColors(void);
	bool writeEdges(void);
	void writeMatrix(TCFloat *matrix);
	void writeSeamMatrix(LDLModelLine *pModelLine);
	void writeColor(int colorNumber, bool preSpace = true);
	void writeColorDeclaration(int colorNumber);
	void writeRGBA(int r, int g, int b, int a);
	bool writeModelLine(LDLModelLine *pModelLine, bool &studsStarted);
	void writeCommentLine(LDLCommentLine *pCommentLine, bool &ifStarted,
		bool &elseStarted, bool &povMode);
	void writeTriangleLine(LDLTriangleLine *pTriangleLine);
	void writeQuadLine(LDLQuadLine *pQuadLine);
	void writeEdgeLineMacro(void);
	void writeEdgeColor(void);
	void endMesh(void);
	void startMesh(void);
	void startStuds(bool &started);
	void endStuds(bool &started);
	void writePoints(const TCVector *points, int count, int size = -1,
		int start = 0);
	void writeTriangle(const TCVector *points, int size = -1, int start = 0);
	void writePoint(const TCVector &point);
	std::string getDeclareName(LDLModel *pModel);
	std::string getDeclareName(const std::string &modelFilename);
	std::string getModelFilename(LDLModel *pModel);
	bool findInclude(const std::string &modelFilename);
	void writeDescriptionComment(LDLModel *pModel);
	bool findModelGeometry(LDLModel *pModel,
		IntShapeLineListMap &colorGeometryMap);
	bool isStud(LDLModel *pModel);
	void getCameraString(char *&povCamera);
	void scanEdgePoint(const TCVector &point, const LDLFileLine *pFileLine);

	bool writeRoundClipRegion(TCFloat fraction, bool closeOff = true);
	virtual bool substituteEighthSphere(bool bfc, bool is48 = false);
	virtual bool substituteEighthSphereCorner(bool bfc, bool is48 = false);
	virtual bool substituteCylinder(TCFloat fraction, bool bfc,
		bool is48 = false);
	virtual bool substituteSlopedCylinder(TCFloat fraction, bool bfc,
		bool is48 = false);
	virtual bool substituteSlopedCylinder2(TCFloat fraction, bool bfc,
		bool is48 = false);
	virtual bool substituteDisc(TCFloat fraction, bool bfc, bool is48 = false);
	virtual bool substituteNotDisc(TCFloat fraction, bool bfc,
		bool is48 = false);
	virtual bool substituteCone(TCFloat fraction, int size, bool bfc,
		bool is48 = false);
	virtual bool substituteRing(TCFloat fraction, int size, bool bfc,
		bool is48 = false, bool isOld = false);
	virtual bool substituteTorusQ(TCFloat fraction, int size, bool bfc,
		bool is48 = false);
	virtual bool substituteTorusIO(bool inner, TCFloat fraction, int size,
		bool bfc, bool is48 = false);
	virtual bool substituteChrd(TCFloat fraction, bool bfc, bool is48 = false);
	virtual bool substituteStud(void);

	static double alphaMod(int color);
	static void cleanupFloats(TCFloat *array, int count = 16);
	static void cleanupDoubles(double *array, int count = 16);
	static const char *get48Prefix(bool is48);
	static std::string getSizeSeamString(TCFloat size);
	static std::string getOfsSeamString(TCFloat ofs, TCFloat size);

	StringBoolMap m_processedModels;
	StringBoolMap m_emptyModels;
	IntBoolMap m_colorsUsed;
	LDLModel *m_pTopModel;
	StringStringMap m_declareNames;
	FILE *m_pPovFile;
	StringList m_searchPath;
	bool m_findReplacements;
	bool m_inlinePov;
	bool m_hideStuds;
	long m_quality;
	TCFloat m_edgeRadius;
	VectorList m_edgePoints;
};

#endif // __LDPOVEXPORTER_H__
