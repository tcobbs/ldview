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
typedef std::map<char, std::string> CharStringMap;
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
	StringList povCodes;
	StringList povFilenames;
	std::string ior;
};

struct PovElement : public PovMapping
{
	float matrix[16];
};

typedef std::map<TCULong, PovMapping> PovColorMap;
typedef std::map<std::string, PovElement> PovElementMap;

class LDPovExporter : public LDExporter
{
	enum ColorType
	{
		CTOpaque,
		CTTransparent,
		CTRubber,
		CTChrome,
	};
public:
	LDPovExporter(void);
	int doExport(LDLModel *pTopModel);
	virtual std::string getExtension(void) const { return "pov"; }
	virtual ucstring getTypeDescription(void) const;
protected:
	~LDPovExporter(void);
	void dealloc(void);
	bool writeHeader(void);
	void writeMainModel(void);
	void writeFloor(void);
	bool writeModel(LDLModel *pModel, const TCFloat *matrix);
	bool writeCamera(void);
	bool writeLights(void);
	void writeLight(TCFloat lat, TCFloat lon, int num);
	bool writeModelObject(LDLModel *pModel, bool mirrored,
		const TCFloat *matrix);
	void writeGeometry(const IntShapeLineListMap &colorGeometryMap);
	bool scanModelColors(LDLModel *pModel);
	bool writeModelColors(void);
	bool writeEdges(void);
	bool writeXmlMatrix(const char *filename);
	void writeMatrix(const TCFloat *matrix);
	void writeSeamMacro(void);
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
	void getCameraStrings(std::string &locationString,
		std::string &lookAtString, std::string &skyString);
	void scanEdgePoint(const TCVector &point, const LDLFileLine *pFileLine);
	void loadLDrawPovXml(void);
	void loadXmlColors(TiXmlElement *matrices);
	std::string loadPovMapping(TiXmlElement *element,
		const char *ldrawElementName, PovMapping &mapping);
	void loadPovDependency(TiXmlElement *element, PovMapping &mapping);
	void loadPovFilenames(TiXmlElement *element, PovMapping &mapping,
		const std::string &povVersion = std::string());
	void loadPovCodes(TiXmlElement *element, PovMapping &mapping);
	void loadPovDependencies(TiXmlElement *element, PovMapping &mapping);
	void loadXmlMatrices(TiXmlElement *matrices);
	void loadXmlElements(TiXmlElement *elements);
	bool writeCode(const std::string &code, bool lineFeed = true);
	bool writeInclude(const std::string &filename, bool lineFeed = true,
		const LDLModel *pModel = NULL);
	void writeLogo(void);
	virtual void initSettings(void) const;
	virtual void addEdgesSettings(void) const;
	virtual void addGeometrySettings(void) const;
	virtual int getNumEdgesSettings(void) const { return 2; }
	virtual int getNumGeometrySettings(void) const { return 4; }
	std::string getAspectRatio(void);
	std::string replaceSpecialChacters(const char *string);
	void writeLDXOpaqueColor(void);
	void writeLDXTransColor(void);
	void writeLDXChromeColor(void);
	void writeLDXRubberColor(void);
	ColorType getColorType(int colorNumber);
	void writeDeclare(const char *name, const std::string &value,
		const char *commentName = NULL);
	void writeDeclare(const char *name, const char *value,
		const char *commentName = NULL);
	void writeDeclare(const char *name, double value,
		const char *commentName = NULL);
	void writeDeclare(const char *name, float value,
		const char *commentName = NULL);
	void writeDeclare(const char *name, long value,
		const char *commentName = NULL);
	void writeDeclare(const char *name, bool value,
		const char *commentName = NULL);
	bool shouldDrawConditional(const TCVector &p1, const TCVector &p2,
		const TCVector &p3, const TCVector &p4, const TCFloat *matrix);

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
	
	// NOTE: loadSettings is NOT virtual: it's called from the constructor.
	void loadSettings(void);

	static double alphaMod(int color);
	static void cleanupFloats(TCFloat *array, int count = 16);
	static void cleanupDoubles(double *array, int count = 16);
	static const char *get48Prefix(bool is48);

	StringBoolMap m_processedModels;
	StringBoolMap m_emptyModels;
	IntBoolMap m_colorsUsed;
	LDLModel *m_pTopModel;
	StringStringMap m_declareNames;
	FILE *m_pPovFile;
	StringList m_searchPath;
	bool m_findReplacements;
	bool m_xmlMap;
	std::string m_xmlMapPath;
	std::string m_topInclude;
	std::string m_bottomInclude;
	bool m_inlinePov;
	bool m_hideStuds;
	bool m_unmirrorStuds;
	long m_quality;
	bool m_floor;
	long m_floorAxis;
	bool m_refls;
	bool m_shads;
	long m_selectedAspectRatio;
	float m_customAspectRatio;
	TCFloat m_edgeRadius;
	TCFloat m_ambient;
	TCFloat m_diffuse;
	TCFloat m_refl;
	TCFloat m_phong;
	TCFloat m_phongSize;
	TCFloat m_transRefl;
	TCFloat m_transFilter;
	TCFloat m_transIoR;
	TCFloat m_rubberRefl;
	TCFloat m_rubberPhong;
	TCFloat m_rubberPhongSize;
	TCFloat m_chromeRefl;
	TCFloat m_chromeBril;
	TCFloat m_chromeSpec;
	TCFloat m_chromeRough;
	VectorList m_edgePoints;
	VectorList m_condEdgePoints;
	PovColorMap m_xmlColors;
	PovElementMap m_xmlElements;
	StringStringMap m_includeVersions;
	StringStringMap m_xmlMatrices;
	TiXmlElement *m_dependenciesElement;
	MatrixMap m_matrices;
	std::string m_ldrawDir;
	StringSet m_includes;
	StringSet m_codes;
	StringSet m_macros;

	static CharStringMap sm_replacementChars;
};

#endif // __LDPOVEXPORTER_H__
