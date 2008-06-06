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
class TiXmlElement;

typedef std::map<std::string, bool> StringBoolMap;
typedef std::map<std::string, std::string> StringStringMap;
typedef std::list<LDLShapeLine *> ShapeLineList;
typedef std::map<int, ShapeLineList> IntShapeLineListMap;
typedef std::map<int, bool> IntBoolMap;
typedef std::list<std::string> StringList;
typedef std::list<TCVector> VectorList;
typedef std::set<std::string> StringSet;
typedef std::map<std::string, const TCFloat *> MatrixMap;

struct PovName
{
	std::string name;
	StringStringMap attributes;
};

typedef std::list<PovName> PovNameList;

struct PovMapping
{
	PovNameList names;
	StringList povFilenames;
};

struct PovElement : public PovMapping
{
	float matrix[16];
};

typedef std::map<TCULong, PovMapping> PovColorMap;
typedef std::map<std::string, PovElement> PovElementMap;

class LDPovExporter : public LDExporter
{
public:
	LDPovExporter(void);
	int doExport(LDLModel *pTopModel);
	virtual std::string getExtension(void) const { return "pov"; }
	virtual ucstring getTypeDescription(void) const;
protected:
	~LDPovExporter(void);
	void dealloc(void);
	bool writeHeader(void);
	bool writeModel(LDLModel *pModel, const TCFloat *matrix);
	bool writeCamera(void);
	bool writeLights(void);
	void writeLight(TCFloat lat, TCFloat lon, TCFloat radius);
	bool writeModelObject(LDLModel *pModel, bool mirrored,
		const TCFloat *matrix);
	void writeGeometry(const IntShapeLineListMap &colorGeometryMap);
	bool scanModelColors(LDLModel *pModel);
	bool writeModelColors(void);
	bool writeEdges(void);
	void writeMatrix(TCFloat *matrix, const char *filename = NULL);
	void writeSeamMatrix(LDLModelLine *pModelLine);
	bool writeColor(int colorNumber, bool slope = false);
	void writeColorDeclaration(int colorNumber);
	void writeInnerColorDeclaration(int colorNumber, bool slope);
	void writeRGBA(int r, int g, int b, int a);
	bool writeModelLine(LDLModelLine *pModelLine, bool &studsStarted,
		bool mirrored, const TCFloat *matrix);
	void indentStud(bool studsStarted);
	void writeInnerModelLine(const std::string &declareName,
		LDLModelLine *pModelLine, bool mirrored, bool slope, bool studsStarted);
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
	std::string getDeclareName(LDLModel *pModel, bool mirrored);
	std::string getDeclareName(const std::string &modelFilename, bool mirrored);
	std::string getModelFilename(const LDLModel *pModel);
	std::string findInclude(const std::string &filename);
	bool findModelInclude(const LDLModel *pModel);
	std::string findMainPovName(const PovMapping &mapping);
	const PovName *findPovName(const PovMapping &mapping, const char *attrName,
		const char *attrValue);
	bool findXmlModelInclude(const LDLModel *pModel);
	void writeDescriptionComment(const LDLModel *pModel);
	bool findModelGeometry(LDLModel *pModel,
		IntShapeLineListMap &colorGeometryMap, bool mirrored);
	bool isStud(LDLModel *pModel);
	void getCameraString(char *&povCamera);
	void scanEdgePoint(const TCVector &point, const LDLFileLine *pFileLine);
	void loadLDrawPovXml(void);
	void loadXmlColors(TiXmlElement *matrices);
	std::string loadPovMapping(TiXmlElement *element,
		const char *ldrawElementName, PovMapping &mapping);
	void loadXmlMatrices(TiXmlElement *matrices);
	void loadXmlElements(TiXmlElement *elements);
	bool writeInclude(const std::string &filename, bool lineFeed = true,
		const LDLModel *pModel = NULL);
	void writeLogo(void);
	virtual void initSettings(void) const;

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

	// NOTE: loadDefaults is NOT virtual: it's called from the constructor.
	void loadDefaults(void);

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
	bool m_xmlMap;
	bool m_inlinePov;
	bool m_hideStuds;
	bool m_unmirrorStuds;
	long m_quality;
	TCFloat m_edgeRadius;
	VectorList m_edgePoints;
	PovColorMap m_xmlColors;
	PovElementMap m_xmlElements;
	StringStringMap m_includeVersions;
	StringStringMap m_xmlMatrices;
	MatrixMap m_matrices;
	std::string m_ldrawDir;
	StringSet m_includes;
};

#endif // __LDPOVEXPORTER_H__
