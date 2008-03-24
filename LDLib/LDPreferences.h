#ifndef __LDPREFERENCES_H__
#define __LDPREFERENCES_H__

#include <TCFoundation/TCAlertSender.h>
#include <LDLib/LDrawModelViewer.h>
#include <TCFoundation/TCStlIncludes.h>
#include <TCFoundation/TCUserDefaults.h>

typedef std::map<std::string, bool> StringBoolMap;
typedef std::vector<std::string> StringVector;

class LDPreferences : public TCAlertSender
{
public:
	enum LightDirection
	{
		CustomDirection	= 0,
		UpperLeft		= 1,
		UpperMiddle		= 2,
		UpperRight		= 3,
		MiddleLeft		= 4,
		Middle			= 5,
		MiddleMiddle	= Middle,
		MiddleRight		= 6,
		LowerLeft		= 7,
		LowerMiddle		= 8,
		LowerRight		= 9,
	};
	LDPreferences(LDrawModelViewer* modelViewer = NULL);
	static const char *lightVectorChangedAlertClass(void)
	{
		return "LDLightVectorChanged";
	}

	void setModelViewer(LDrawModelViewer *value);

	// These are called from the constructor, and cannot be properly made into
	// virtual functions.
	// *************************************************************************
	void applySettings(void);
	void applyGeneralSettings(void);
	void applyLDrawSettings(void);
	void applyGeometrySettings(void);
	void applyEffectsSettings(void);
	void applyPrimitivesSettings(void);
	void applyUpdatesSettings();
	void loadSettings(void);
	void loadDefaultGeneralSettings(bool initializing = true);
	void loadDefaultLDrawSettings(bool initializing = true);
	void loadDefaultGeometrySettings(bool initializing = true);
	void loadDefaultEffectsSettings(bool initializing = true);
	void loadDefaultPrimitivesSettings(bool initializing = true);
	void loadDefaultUpdatesSettings(bool initializing = true);
	void loadDefaultInventorySettings(bool initializing = true);
	void loadInventorySettings(void);
	// *************************************************************************

	void commitSettings(void);
	void commitGeneralSettings(bool flush = true);
	void commitLDrawSettings(bool flush = true);
	void commitGeometrySettings(bool flush = true);
	void commitEffectsSettings(bool flush = true);
	void commitPrimitivesSettings(bool flush = true);
	void commitUpdatesSettings(bool flush = true);
	void commitInventorySettings(bool flush = true);

	virtual void saveDefaultView(void);
	virtual void resetDefaultView(void);

	// General settings
	int getFsaaMode(void) { return m_fsaaMode; }
	bool getLineSmoothing(void) { return m_lineSmoothing; }
	void getBackgroundColor(int &r, int &g, int &b);
	void getDefaultColor(int &r, int &g, int &b);
	bool getTransDefaultColor(void) { return m_transDefaultColor; }
	int getDefaultColorNumber(void) { return m_defaultColorNumber; }
	bool getProcessLdConfig(void) { return m_processLdConfig; }
	bool getShowFps(void) { return m_showFps; }
	bool getShowErrors(void) { return m_showErrors; }
	int getFullScreenRefresh(void) { return m_fullScreenRefresh; }
	TCFloat getFov(void) { return m_fov; }
	int getMemoryUsage(void) { return m_memoryUsage; }
	void getCustomColor(int index, int &r, int &g, int &b);
	
	// LDraw settings
	const char *getLDrawDir(void) { return m_ldrawDir.c_str(); }
	const StringVector &getExtraDirs(void) { return m_extraDirs; }

	// Geometry settings
	bool getUseSeams(void) { return m_useSeams; }
	int getSeamWidth(void) { return m_seamWidth; }
	bool getDrawWireframe(void) { return m_drawWireframe; }
	bool getUseWireframeFog(void) { return m_useWireframeFog; }
	bool getRemoveHiddenLines(void) { return m_removeHiddenLines; }
	int getWireframeThickness(void) { return m_wireframeThickness; }
	bool getBfc(void) { return m_bfc; }
	bool getRedBackFaces(void) { return m_redBackFaces; }
	bool getGreenFrontFaces(void) { return m_greenFrontFaces; }
	bool getShowHighlightLines(void) { return m_showHighlightLines; }
	bool getDrawConditionalHighlights(void)
	{
		return m_drawConditionalHighlights;
	}
	bool getShowAllConditionalLines(void) { return m_showAllConditionalLines; }
	bool getShowConditionalControlPoints(void)
	{
		return m_showConditionalControlPoints;
	}
	bool getEdgesOnly(void) { return m_edgesOnly; }
	bool getUsePolygonOffset(void) { return m_usePolygonOffset; }
	bool getBlackHighlights(void) { return m_blackHighlights; }
	int getEdgeThickness(void) { return m_edgeThickness; }

	// Effects settings
	bool getUseLighting(void) { return m_useLighting; }
	bool getQualityLighting(void) { return m_qualityLighting; }
	bool getSubduedLighting(void) { return m_subduedLighting; }
	bool getUseSpecular(void) { return m_useSpecular; }
	bool getOneLight(void) { return m_oneLight; }
	LightDirection getLightDirection(void);
	const TCVector &getLightVector(void) { return m_lightVector; }
	bool getDrawLightDats(void) { return m_drawLightDats; }
	bool getOptionalStandardLight(void) { return m_optionalStandardLight; }
	bool getNoLightGeom(void) { return m_noLightGeom; }
	LDVStereoMode getStereoMode(void) { return m_stereoMode; }
	int getStereoEyeSpacing(void) { return m_stereoEyeSpacing; }
	LDVCutawayMode getCutawayMode(void) { return m_cutawayMode; }
	int getCutawayAlpha(void) { return m_cutawayAlpha; }
	int getCutawayThickness(void) { return m_cutawayThickness; }
	bool getSortTransparent(void) { return m_sortTransparent; }
	bool getPerformSmoothing(void) { return m_performSmoothing; }
	bool getUseStipple(void) { return m_useStipple; }
	bool getUseFlatShading(void) { return m_useFlatShading; }

	// Primitives settings
	bool getAllowPrimitiveSubstitution(void)
	{
		return m_allowPrimitiveSubstitution;
	}
	bool getTextureStuds(void) { return m_textureStuds; }
	int getTextureFilterType(void) { return m_textureFilterType; }
	TCFloat getAnisoLevel(void) { return m_anisoLevel; }
	int getCurveQuality(void) { return m_curveQuality; }
	bool getQualityStuds(void) { return m_qualityStuds; }
	bool getHiResPrimitives(void) { return m_hiResPrimitives; }

	// Update settings
	int getProxyType(void) { return m_proxyType; }
	const char *getProxyServer(void) { return m_proxyServer.c_str(); }
	int getProxyPort(void) { return m_proxyPort; }
	bool getCheckPartTracker(void) { return m_checkPartTracker; }
	int getMissingPartWait(void) { return m_missingPartWait; }
	int getUpdatedPartWait(void) { return m_updatedPartWait; }

	// Inventory settings
	bool getInvShowModel(void) { return m_invShowModel; }
	bool getInvExternalCss(void) { return m_invExternalCss; }
	bool getInvPartImages(void) { return m_invPartImages; }
	bool getInvShowFile(void) { return m_invShowFile; }
	bool getInvShowTotal(void) { return m_invShowTotal; }
	const LongVector &getInvColumnOrder(void) { return m_invColumnOrder; }
	const char *getInvLastSavePath(void) { return m_invLastSavePath.c_str(); }

	// No UI
	bool getMultiThreaded(void) { return m_multiThreaded; }

	// General settings
	void setFsaaMode(int value, bool commit = false);
	void setLineSmoothing(bool value, bool commit = false);
	void setBackgroundColor(int r, int g, int b, bool commit = false);
	void setDefaultColor(int r, int g, int b, bool commit = false);
	void setTransDefaultColor(bool value, bool commit = false);
	void setDefaultColorNumber(int value, bool commit = false);
	void setProcessLdConfig(bool value, bool commit = false);
	void setShowFps(bool value, bool commit = false);
	void setShowErrors(bool value, bool commit = false);
	void setFullScreenRefresh(int value, bool commit = false);
	void setFov(TCFloat value, bool commit = false);
	void setMemoryUsage(int value, bool commit = false);
	void setCustomColor(int index, int r, int g, int b, bool commit = false);
	
	// LDraw settings
	void setLDrawDir(const char *value, bool commit = false);
	void setExtraDirs(const StringVector &value, bool commit = false);

	// Geometry settings
	void setUseSeams(bool value, bool commit = false, bool apply = false);
	void setSeamWidth(int value, bool commit = false);
	void setDrawWireframe(bool value, bool commit = false, bool apply = false);
	void setUseWireframeFog(bool value, bool commit = false,
		bool apply = false);
	void setRemoveHiddenLines(bool value, bool commit = false,
		bool apply = false);
	void setWireframeThickness(int value, bool commit = false);
	void setBfc(bool value, bool commit = false, bool apply = false);
	void setRedBackFaces(bool value, bool commit = false, bool apply = false);
	void setGreenFrontFaces(bool value, bool commit = false,
		bool apply = false);
	void setShowHighlightLines(bool value, bool commit = false,
		bool apply = false);
	void setDrawConditionalHighlights(bool value, bool commit = false,
		bool apply = false);
	void setShowAllConditionalLines(bool value, bool commit = false);
	void setShowConditionalControlPoints(bool value, bool commit = false);
	void setEdgesOnly(bool value, bool commit = false, bool apply = false);
	void setUsePolygonOffset(bool value, bool commit = false,
		bool apply = false);
	void setBlackHighlights(bool value, bool commit = false,
		bool apply = false);
	void setEdgeThickness(int value, bool commit = false);

	// Effects settings
	void setUseLighting(bool value, bool commit = false, bool apply = false);
	void setQualityLighting(bool value, bool commit = false, bool apply = false);
	void setSubduedLighting(bool value, bool commit = false, bool apply = false);
	void setUseSpecular(bool value, bool commit = false, bool apply = false);
	void setOneLight(bool value, bool commit = false, bool apply = false);
	void setLightDirection(LightDirection value, bool commit = false,
		bool apply = false);
	void setLightVector(const TCVector &value, bool commit = false,
		bool apply = false);
	void setDrawLightDats(bool value, bool commit = false, bool apply = false);
	void setOptionalStandardLight(bool value, bool commit = false,
		bool apply = false);
	void setNoLightGeom(bool value, bool commit = false, bool apply = false);
	void setStereoMode(LDVStereoMode value, bool commit = false);
	void setStereoEyeSpacing(int value, bool commit = false);
	void setCutawayMode(LDVCutawayMode value, bool commit = false);
	void setCutawayAlpha(int value, bool commit = false);
	void setCutawayThickness(int value, bool commit = false);
	void setSortTransparent(bool value, bool commit = false);
	void setPerformSmoothing(bool value, bool commit = false);
	void setUseStipple(bool value, bool commit = false);
	void setUseFlatShading(bool value, bool commit = false);

	// Primitives settings
	void setAllowPrimitiveSubstitution(bool value, bool commit = false,
		bool apply = false);
	void setTextureStuds(bool value, bool commit = false, bool apply = false);
	void setTextureFilterType(int value, bool commit = false);
	void setAnisoLevel(TCFloat value, bool commit = false);
	void setCurveQuality(int value, bool commit = false);
	void setQualityStuds(bool value, bool commit = false);
	void setHiResPrimitives(bool value, bool commit = false);

	// Update settings
	void setProxyType(int value, bool commit = false);
	void setProxyServer(const char *value, bool commit = false);
	void setProxyPort(int value, bool commit = false);
	void setCheckPartTracker(bool value, bool commit = false);
	void setMissingPartWait(int value, bool commit = false);
	void setUpdatedPartWait(int value, bool commit = false);

	// Inventory settings
	void setInvShowModel(bool value, bool commit = false);
	void setInvExternalCss(bool value, bool commit = false);
	void setInvPartImages(bool value, bool commit = false);
	void setInvShowFile(bool value, bool commit = false);
	void setInvShowTotal(bool value, bool commit = false);
	void setInvColumnOrder(const LongVector &value, bool commit = false);
	void setInvLastSavePath(const char *value, bool commit = false);

	// No UI
	void setDefaultZoom(TCFloat value, bool commit = false);
protected:
	~LDPreferences(void);
	void dealloc(void);
	void setSetting(bool &setting, bool value, const char *key, bool commit);
	void setSetting(int &setting, int value, const char *key, bool commit);
	void setSetting(TCULong &setting, TCULong value, const char *key,
		bool commit);
	void setSetting(TCFloat &setting, TCFloat value, const char *key,
		bool commit);
	void setSetting(std::string &setting, const std::string &value,
		const char *key, bool commit, bool isPath = false);
	void setSetting(LongVector &setting, const LongVector &value,
		const char *key, bool commit);
	void setSetting(StringVector &setting, const StringVector &value,
		const char *key, bool commit, bool isPath = false, int keyDigits = 2);
	bool setSetting(TCVector &setting, const TCVector &value,
		const char *key, bool commit);
	void setColorSetting(TCULong &setting, int r, int g, int b, const char *key,
		bool commit);
	bool getBoolSetting(const char *key, bool defaultValue = false);
	LongVector getLongVectorSetting(const char *key,
		const LongVector &defaultValue = LongVector());
	StringVector getStringVectorSetting(const char *key,
		const StringVector &defaultValue = StringVector(), bool isPath = false,
		int keyDigits = 2);
	TCVector getTCVectorSetting(const char *key,
		const TCVector &defaultValue = TCVector());
	long getLongSetting(const char *key, long defaultValue = 0);
	int getIntSetting(const char *key, int defaultValue = 0);
	float getFloatSetting(const char *key, float defaultValue = 0.0f);
	std::string getStringSetting(const char *key,
		const char *defaultValue = NULL, bool isPath = false);
	void getColorSetting(const char *key, int &r, int &g, int &b,
		TCULong defaultColor = 0);

	virtual void getRGB(int color, int &r, int &g, int &b);
	virtual int getColor(int r, int g, int b);

	// These are called from the constructor, and cannot be properly made into
	// virtual functions.
	// *************************************************************************
	void setupDefaultRotationMatrix(void);
	void setupModelCenter(void);
	void setupModelSize(void);
	void loadGeneralSettings(void);
	void loadLDrawSettings(void);
	void loadGeometrySettings(void);
	void loadEffectsSettings(void);
	void loadPrimitivesSettings(void);
	void loadUpdatesSettings(void);
	// *************************************************************************

	LDrawModelViewer* modelViewer;

	// General settings
	int m_fsaaMode;
	bool m_lineSmoothing;
	TCULong m_backgroundColor;
	TCULong m_defaultColor;
	bool m_transDefaultColor;
	int m_defaultColorNumber;
	bool m_processLdConfig;
	bool m_showFps;
	bool m_showErrors;
	int m_fullScreenRefresh;
	TCFloat m_fov;
	int m_memoryUsage;
	TCULong m_customColors[16];
	
	// LDraw settings
	std::string m_ldrawDir;
	StringVector m_extraDirs;

	// Geometry settings
	bool m_useSeams;
	int m_seamWidth;
	bool m_drawWireframe;
	bool m_useWireframeFog;
	bool m_removeHiddenLines;
	int m_wireframeThickness;
	bool m_bfc;
	bool m_redBackFaces;
	bool m_greenFrontFaces;
	bool m_showHighlightLines;
	bool m_drawConditionalHighlights;
	bool m_showAllConditionalLines;
	bool m_showConditionalControlPoints;
	bool m_edgesOnly;
	bool m_usePolygonOffset;
	bool m_blackHighlights;
	int m_edgeThickness;

	// Effects settings
	bool m_useLighting;
	bool m_qualityLighting;
	bool m_subduedLighting;
	bool m_useSpecular;
	bool m_oneLight;
	LDVStereoMode m_stereoMode;
	int m_stereoEyeSpacing;
	LDVCutawayMode m_cutawayMode;
	int m_cutawayAlpha;
	int m_cutawayThickness;
	bool m_sortTransparent;
	bool m_performSmoothing;
	bool m_useStipple;
	bool m_useFlatShading;

	// Primitives settings
	bool m_allowPrimitiveSubstitution;
	bool m_textureStuds;
	int m_textureFilterType;
	TCFloat m_anisoLevel;
	int m_curveQuality;
	bool m_qualityStuds;
	bool m_hiResPrimitives;

	// Update settings
	int m_proxyType;
	std::string m_proxyServer;
	int m_proxyPort;
	bool m_checkPartTracker;
	int m_missingPartWait;
	int m_updatedPartWait;

	// Inventory settings
	bool m_invShowModel;
	bool m_invExternalCss;
	bool m_invPartImages;
	bool m_invShowFile;
	bool m_invShowTotal;
	LongVector m_invColumnOrder;
	std::string m_invLastSavePath;

	// Settings with no UI
	bool m_skipValidation;
	TCFloat m_defaultZoom;
	TCFloat m_zoomMax;
	TCVector m_lightVector;
	bool m_drawLightDats;
	bool m_optionalStandardLight;
	bool m_noLightGeom;
	bool m_multiThreaded;

	StringBoolMap m_changedSettings;
	StringBoolMap m_globalSettings;

	bool m_initializing;
};

#endif // __LDPREFERENCES_H__
