#ifndef __TREMAINMODEL_H__
#define __TREMAINMODEL_H__

#include <TRE/TREModel.h>
#include <TCFoundation/TCImage.h>
#include <TCFoundation/TCStlIncludes.h>

#if defined(_MSC_VER) && _MSC_VER <= 1200	// VS 6
#define _NO_TRE_THREADS
#else  // VS 6
#ifdef _NO_BOOST
#define _NO_TRE_THREADS
#endif // _NO_BOOST
#endif // VS 6
#ifndef _NO_TRE_THREADS
#ifdef WIN32
#define MutexType recursive_mutex
#else
#define MutexType mutex
#endif // WIN32
namespace boost
{
	class thread_group;
	class MutexType;
	class condition;
}
#endif // !_NO_TRE_THREADS

class TCDictionary;
class TREVertexStore;
class TREColoredShapeGroup;
class TRETransShapeGroup;
class TCImage;

extern const GLfloat POLYGON_OFFSET_FACTOR;
extern const GLfloat POLYGON_OFFSET_UNITS;

typedef std::list<TCVector> TCVectorList;
typedef std::list<TCULong> TCULongList;
typedef std::list<TREMSection> SectionList;

class TREMainModel : public TREModel
{
public:
	TREMainModel(void);
	//TREMainModel(const TREMainModel &other);
	virtual TCObject *copy(void);
	virtual TCDictionary* getLoadedModels(bool bfc);
	virtual void draw(void);
	virtual TREVertexStore *getVertexStore(void) { return m_vertexStore; }
	virtual TREVertexStore *getStudVertexStore(void)
	{
		return m_studVertexStore;
	}
	virtual TREVertexStore *getColoredStudVertexStore(void)
	{
		return m_coloredStudVertexStore;
	}
	virtual TREModel *modelNamed(const char *name, bool bfc);
	virtual void registerModel(TREModel *model, bool bfc);
	void setCompilePartsFlag(bool value) { m_mainFlags.compileParts = value; }
	bool getCompilePartsFlag(void) const
	{
		return m_mainFlags.compileParts != false;
	}
	void setCompileAllFlag(bool value) { m_mainFlags.compileAll = value; }
	bool getCompileAllFlag(void) const
	{
		return m_mainFlags.compileAll != false;
	}
	void setFlattenConditionalsFlag(bool value)
	{
		m_mainFlags.flattenConditionals = value;
	}
	bool getFlattenConditionalsFlag(void) const
	{
		return m_mainFlags.flattenConditionals != false;
	}
	void setEdgeLinesFlag(bool value) { m_mainFlags.edgeLines = value; }
	bool getEdgeLinesFlag(void) { return m_mainFlags.edgeLines != false; }
	void setEdgesOnlyFlag(bool value) { m_mainFlags.edgesOnly = value; }
	bool getEdgesOnlyFlag(void)
	{
		return m_mainFlags.edgesOnly != false && getEdgeLinesFlag();
	}
	void setTwoSidedLightingFlag(bool value);
	bool getTwoSidedLightingFlag(void)
	{
		return m_mainFlags.twoSidedLighting != false;
	}
	void setLightingFlag(bool value);
	bool getLightingFlag(void) { return m_mainFlags.lighting != false; }
	void setUseStripsFlag(bool value) { m_mainFlags.useStrips = value; }
	bool getUseStripsFlag(void) { return m_mainFlags.useStrips != false; }
	void setUseFlatStripsFlag(bool value) { m_mainFlags.useFlatStrips = value; }
	bool getUseFlatStripsFlag(void)
	{
		return m_mainFlags.useFlatStrips != false;
	}
	void setBFCFlag(bool value) { m_mainFlags.bfc = value; }
	bool getBFCFlag(void) { return m_mainFlags.bfc != false; }
	void setRedBackFacesFlag(bool value) { m_mainFlags.redBackFaces = value; }
	bool getRedBackFacesFlag(void) { return m_mainFlags.redBackFaces != false; }
	void setGreenFrontFacesFlag(bool value)
	{
		m_mainFlags.greenFrontFaces = value;
	}
	bool getGreenFrontFacesFlag(void)
	{
		return m_mainFlags.greenFrontFaces != false;
	}
	void setDrawNormalsFlag(bool value) { m_mainFlags.drawNormals = value; }
	bool getDrawNormalsFlag(void) { return m_mainFlags.drawNormals != false; }
	void setStencilConditionalsFlag(bool value)
	{
		m_mainFlags.stencilConditionals = value;
	}
	bool getStencilConditionalsFlag(void);
	void setVertexArrayEdgeFlagsFlag(bool value)
	{
		m_mainFlags.vertexArrayEdgeFlags = value;
	}
	bool getVertexArrayEdgeFlagsFlag(void)
	{
		return m_mainFlags.vertexArrayEdgeFlags != false;
	}
	void setMultiThreadedFlag(bool value) { m_mainFlags.multiThreaded = value; }
	bool getMultiThreadedFlag(void) const
	{
		return m_mainFlags.multiThreaded != false;
	}
	void setSaveAlphaFlag(bool value) { m_mainFlags.saveAlpha = value; }
	bool getSaveAlphaFlag(void) { return m_mainFlags.saveAlpha != false; }
	void setLineJoinsFlag(bool value) { m_mainFlags.lineJoins = value; }
	bool getLineJoinsFlag(void) { return m_mainFlags.lineJoins != false; }
	bool getActiveLineJoinsFlag(void)
	{
		return m_mainFlags.activeLineJoins != false;
	}
	void setAALinesFlag(bool value) { m_mainFlags.aaLines = value; }
	bool getAALinesFlag(void) { return m_mainFlags.aaLines != false; }
	void setSortTransparentFlag(bool value)
	{
		m_mainFlags.sortTransparent = value;
	}
	bool getSortTransparentFlag(void)
	{
		return m_mainFlags.sortTransparent != false;
	}
	void setStippleFlag(bool value) { m_mainFlags.stipple = value; }
	bool getStippleFlag(void) { return m_mainFlags.stipple != false; }
	void setWireframeFlag(bool value) { m_mainFlags.wireframe = value; }
	bool getWireframeFlag(void) { return m_mainFlags.wireframe != false; }
	void setConditionalLinesFlag(bool value)
	{
		m_mainFlags.conditionalLines = value;
	}
	bool getConditionalLinesFlag(void)
	{
		return m_mainFlags.conditionalLines != false && getEdgeLinesFlag();
	}
	void setSmoothCurvesFlag(bool value) { m_mainFlags.smoothCurves = value; }
	bool getSmoothCurvesFlag(void) { return m_mainFlags.smoothCurves != false; }
	void setShowAllConditionalFlag(bool value);
	bool getShowAllConditionalFlag(void)
	{
		return m_mainFlags.showAllConditional != false &&
			getConditionalLinesFlag();
	}
	void setConditionalControlPointsFlag(bool value);
	bool getConditionalControlPointsFlag(void)
	{
		return m_mainFlags.conditionalControlPoints != false &&
			getConditionalLinesFlag();
	}
	void setPolygonOffsetFlag(bool value)
	{
		m_mainFlags.polygonOffset = value;
	}
	bool getPolygonOffsetFlag(void)
	{
		return m_mainFlags.polygonOffset != false;
	}
	void setStudLogoFlag(bool value) { m_mainFlags.studLogo = value; }
	bool getStudLogoFlag(void) { return m_mainFlags.studLogo != false; }
	void setRemovingHiddenLines(bool value)
	{
		m_mainFlags.removingHiddenLines = value;
	}
	bool getRemovingHiddenLines(void)
	{
		return m_mainFlags.removingHiddenLines != false;
	}
	void setCutawayDrawFlag(bool value) { m_mainFlags.cutawayDraw = value; }
	bool getCutawayDrawFlag(void) { return m_mainFlags.cutawayDraw != false; }
	void setEdgeLineWidth(GLfloat value) { m_edgeLineWidth = value; }
	TCFloat getEdgeLineWidth(void) { return m_edgeLineWidth; }
	void setStudAnisoLevel(GLfloat value) { m_studAnisoLevel = value; }
	TCFloat getStudAnisoLevel(void) { return m_studAnisoLevel; }
	void setStudTextureFilter(int value) { m_studTextureFilter = value; }
	int getStudTextureFilter(void) { return m_studTextureFilter; }
	virtual bool getCompiled(void) const
	{
		return m_mainFlags.compiled != false;
	}
	virtual bool getCompiling(void) { return m_mainFlags.compiling != false; }
	virtual TCFloat getMaxRadiusSquared(const TCVector &center);
	virtual TCFloat getMaxRadius(const TCVector &center);
	TREVertexStore *getColoredVertexStore(void)
	{
		return m_coloredVertexStore;
	}
	TREVertexStore *getTransVertexStore(void)
	{
		return m_transVertexStore;
	}
	void setColor(TCULong color, TCULong edgeColor);
	TCULong getColor(void);
	TCULong getEdgeColor(void);
	bool postProcess(void);
	void compile(void);
	void recompile(void);
	virtual void addTransparentTriangle(TCULong color,
		const TCVector vertices[], const TCVector normals[],
		const TCVector *textureCoords = NULL);
	virtual bool shouldLoadConditionalLines(void);
	bool isStudSection(TREMSection section)
	{
		return section == TREMStud || section == TREMStudBFC;
	}
	virtual void openGlWillEnd(void);
	virtual void finish(void);
	virtual void addLight(const TCVector &location, TCULong color);
	virtual const TCVectorList &getLightLocations(void) const
	{
		return m_lightLocations;
	}
	virtual const TCULongList &getLightColors(void) const
	{
		return m_lightColors;
	}
	const TCFloat *getCurrentModelViewMatrix(void) const
	{
		return m_currentModelViewMatrix;
	}
	const TCFloat *getCurrentProjectionMatrix(void) const
	{
		return m_currentProjectionMatrix;
	}
	bool hasWorkerThreads(void);
	void waitForSort(void);
	void waitForConditionals(int step);
	const TCULongArray *getActiveConditionals(int step) const
	{
		return m_activeConditionals[step];
	}
	const TCULongArray *getActiveColorConditionals(int step) const
	{
		return m_activeColorConditionals[step];
	}
	bool doingBackgroundConditionals(void);
	virtual TCObject *getAlertSender(void) { return m_alertSender; }
	virtual void setAlertSender(TCObject *value) { m_alertSender = value; }
	int getStep(void) const { return m_step; }
	void setStep(int value);
	int getNumSteps(void) const { return m_numSteps; }
	void transferPrep(void);
	void updateModelTransferStep(int subModelIndex,
		bool isConditionals = false);
	bool onLastStep(void);

	static void loadStudTexture(const char *filename);
	static void setStudTextureData(TCByte *data, long length);
	static void setRawStudTextureData(TCByte *data, long length);
	static TCImageArray *getStudTextures(void) { return sm_studTextures; }
	static unsigned getStudTextureID(void) { return sm_studTextureID; }
protected:
	virtual ~TREMainModel(void);
	virtual void dealloc(void);
	void scanMaxRadiusSquaredPoint(const TCVector &point);
	virtual void activateBFC(void);
	virtual void deactivateBFC(void);
	void transferTransparent(void);
	virtual void transferTransparent(const SectionList &sectionList);
	virtual void drawTransparent(int pass = -1);
	virtual void drawLines(int pass = -1);
	virtual void drawSolid(void);
	virtual void enableLineSmooth(int pass = -1);
	virtual void bindStudTexture(void);
	virtual void configureStudTexture(bool allowMipMap = true);
	virtual bool shouldCompileSection(TREMSection section);
	virtual void passOnePrep(void);
	virtual void passTwoPrep(void);
	virtual void passThreePrep(void);
#ifndef _NO_TRE_THREADS
	template <class _ScopedLock> bool workerThreadDoWork(_ScopedLock &lock);
	template <class _ScopedLock> void nextConditionalsStep(_ScopedLock &lock);
	void workerThreadProc(void);
#endif // !_NO_TRE_THREADS
	void launchWorkerThreads(void);
	int getNumWorkerThreads(void);
	int getNumBackgroundTasks(void);
	void triggerWorkerThreads(void);
	bool backgroundSortNeeded(void);
	bool backgroundConditionalsNeeded(void);
	void flattenConditionals(void);
	void backgroundConditionals(int step);
	TCULongArray *backgroundConditionals(TREShapeGroup *shapes, int step);

	static void loadStudMipTextures(TCImage *mainImage);

	TCObject *m_alertSender;
	TCDictionary *m_loadedModels;
	TCDictionary *m_loadedBFCModels;
	TREVertexStore *m_vertexStore;
	TREVertexStore *m_studVertexStore;
	TREVertexStore *m_coloredVertexStore;
	TREVertexStore *m_coloredStudVertexStore;
	TREVertexStore *m_transVertexStore;
	TCULong m_color;
	TCULong m_edgeColor;
	TCFloat m_maxRadiusSquared;
	TCVector m_center;
	GLfloat m_edgeLineWidth;
	GLfloat m_studAnisoLevel;
	bool m_abort;	// Easier not to be a bit field.  A pointer to it is passed
					// into other functions, and that doesn't work with a bit
					// field.
	GLint m_studTextureFilter;
	TCVectorList m_lightLocations;
	TCULongList m_lightColors;
	TCFloat m_currentModelViewMatrix[16];
	TCFloat m_currentProjectionMatrix[16];
	TCULong m_conditionalsDone;
	int m_conditionalsStep;
	TCULongArray *m_activeConditionals[32];
	TCULongArray *m_activeColorConditionals[32];
	int m_step;
	int m_numSteps;
	int m_transferStep;
	IntVector m_transStepCounts;
#ifndef _NO_TRE_THREADS
	boost::thread_group *m_threadGroup;
	boost::MutexType *m_workerMutex;
	boost::condition *m_workerCondition;
	boost::condition *m_sortCondition;
	boost::condition *m_conditionalsCondition;
	bool m_exiting;
#endif // !_NO_TRE_THREADS
	struct
	{
		// The following are temporal
		bool compiling:1;
		bool compiled:1;
		bool removingHiddenLines:1;	// This one is changed externally
		bool cutawayDraw:1;			// This one is changed externally
		bool activeLineJoins:1;
		bool frameSorted:1;
		bool frameSortStarted:1;
		bool frameStarted:1;
		// The following aren't temporal
		bool compileParts:1;
		bool compileAll:1;
		bool flattenConditionals:1;
		bool edgeLines:1;
		bool edgesOnly:1;
		bool twoSidedLighting:1;
		bool lighting:1;
		bool useStrips:1;
		bool useFlatStrips:1;
		bool bfc:1;
		bool aaLines:1;
		bool sortTransparent:1;
		bool stipple:1;
		bool wireframe:1;
		bool conditionalLines:1;
		bool smoothCurves:1;
		bool showAllConditional:1;
		bool conditionalControlPoints:1;
		bool polygonOffset:1;
		bool studLogo:1;
		bool redBackFaces:1;
		bool greenFrontFaces:1;
		bool lineJoins:1;
		bool drawNormals:1;
		bool stencilConditionals:1;
		bool vertexArrayEdgeFlags:1;
		bool multiThreaded:1;
		bool saveAlpha:1;
	} m_mainFlags;

	static TCImageArray *sm_studTextures;
	static GLuint sm_studTextureID;
	static class TREMainModelCleanup
	{
	public:
		~TREMainModelCleanup(void);
	} sm_mainModelCleanup;
	friend class TREMainModelCleanup;
};

#endif // __TREMAINMODEL_H__
