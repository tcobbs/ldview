#ifndef __LDPREFERENCES_H__
#define __LDPREFERENCES_H__

#include <TCFoundation/TCObject.h>
#include <LDLib/LDrawModelViewer.h>
#ifdef WIN32
// In Windows, we have to disable a number of warnings in order to use any STL
// classes without getting tons of warnings.  The following warning is shut off
// completely; it's just the warning that identifiers longer than 255 characters
// will be truncated in the debug info.  I really don't care about this.  Note
// that the other warnings are only disabled during the #include of the STL
// headers due to the warning(push) and warning(pop).
#pragma warning(push, 1)	// Minimum warnings during STL includes
#endif // WIN32
#include <string>
#include <map>
#ifdef WIN32
#pragma warning(pop)
#endif // WIN32

typedef std::map<std::string, bool> StringBoolMap;

class LDPreferences : public TCObject
{
public:
	LDPreferences(LDrawModelViewer* modelViewer);

	// These are called from the constructor, and cannot be properly made into
	// virtual functions.
	// *************************************************************************
	void applySettings(void);
	void applyGeneralSettings(void);
	void applyGeometrySettings(void);
	void applyEffectsSettings(void);
	void applyPrimitivesSettings(void);
	void applyUpdatesSettings();
	void loadSettings(void);
	void loadDefaultGeneralSettings(void);
	void loadDefaultGeometrySettings(void);
	void loadDefaultEffectsSettings(void);
	void loadDefaultPrimitivesSettings(void);
	void loadDefaultUpdatesSettings(void);
	void loadGeneralSettings(void);
	void loadGeometrySettings(void);
	void loadEffectsSettings(void);
	void loadPrimitivesSettings(void);
	void loadUpdatesSettings(void);
	// *************************************************************************

	void commitSettings(void);
	void commitGeneralSettings(void);
	void commitGeometrySettings(void);
	void commitEffectsSettings(void);
	void commitPrimitivesSettings(void);
	void commitUpdatesSettings(void);

	// General settings
	int getFsaaMode(void) { return m_fsaaMode; }
	bool getLineSmoothing(void) { return m_lineSmoothing; }
	TCULong getBackgroundColor(void) { return m_backgroundColor; }
	TCULong getDefaultColor(void) { return m_defaultColor; }
	bool getTransDefaultColor(void) { return m_transDefaultColor; }
	int getDefaultColorNumber(void) { return m_defaultColorNumber; }
	bool getProcessLdConfig(void) { return m_processLdConfig; }
	bool getShowFps(void) { return m_showFps; }
	bool getShowErrors(void) { return m_showErrors; }
	int getFullScreenRefresh(void) { return m_fullScreenRefresh; }
	TCFloat getFov(void) { return m_fov; }
	int getMemoryUsage(void) { return m_memoryUsage; }
	TCULong getCustomColor(int index) { return m_customColors[index]; }

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
	int getCurveQuality(void) { return m_curveQuality; }
	bool getQualityStuds(void) { return m_qualityStuds; }
	bool getHiResPrimitives(void) { return m_hiResPrimitives; }

	// Update settings
	int getProxyType(void) { return m_proxyType; }
	const char *getProxyServer(void) { return m_proxyServer.c_str(); }
	int getProxyPort(void) { return m_proxyPort; }
	bool getCheckPartTracker(void) { return m_checkPartTracker; }


	// General settings
	void setFsaaMode(int value, bool commit = false);
	void setLineSmoothing(bool value, bool commit = false);
	void setBackgroundColor(TCULong value, bool commit = false);
	void setDefaultColor(TCULong value, bool commit = false);
	void setTransDefaultColor(bool value, bool commit = false);
	void setDefaultColorNumber(int value, bool commit = false);
	void setProcessLdConfig(bool value, bool commit = false);
	void setShowFps(bool value, bool commit = false);
	void setShowErrors(bool value, bool commit = false);
	void setFullScreenRefresh(int value, bool commit = false);
	void setFov(TCFloat value, bool commit = false);
	void setMemoryUsage(int value, bool commit = false);
	void setCustomColor(int index, TCULong value, bool commit = false);

	// Geometry settings
	void setUseSeams(bool value, bool commit = false);
	void setSeamWidth(int value, bool commit = false);
	void setDrawWireframe(bool value, bool commit = false);
	void setUseWireframeFog(bool value, bool commit = false);
	void setRemoveHiddenLines(bool value, bool commit = false);
	void setWireframeThickness(int value, bool commit = false);
	void setBfc(bool value, bool commit = false);
	void setRedBackFaces(bool value, bool commit = false);
	void setGreenFrontFaces(bool value, bool commit = false);
	void setShowHighlightLines(bool value, bool commit = false);
	void setDrawConditionalHighlights(bool value, bool commit = false);
	void setShowAllConditionalLines(bool value, bool commit = false);
	void setShowConditionalControlPoints(bool value, bool commit = false);
	void setEdgesOnly(bool value, bool commit = false);
	void setUsePolygonOffset(bool value, bool commit = false);
	void setBlackHighlights(bool value, bool commit = false);
	void setEdgeThickness(int value, bool commit = false);

	// Effects settings
	void setUseLighting(bool value, bool commit = false);
	void setQualityLighting(bool value, bool commit = false);
	void setSubduedLighting(bool value, bool commit = false);
	void setUseSpecular(bool value, bool commit = false);
	void setOneLight(bool value, bool commit = false);
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
	void setAllowPrimitiveSubstitution(bool value, bool commit = false);
	void setTextureStuds(bool value, bool commit = false);
	void setTextureFilterType(int value, bool commit = false);
	void setCurveQuality(int value, bool commit = false);
	void setQualityStuds(bool value, bool commit = false);
	void setHiResPrimitives(bool value, bool commit = false);

	// Update settings
	void setProxyType(int value, bool commit = false);
	void setProxyServer(const char *value, bool commit = false);
	void setProxyPort(int value, bool commit = false);
	void setCheckPartTracker(bool value, bool commit = false);
protected:
	~LDPreferences(void);
	void dealloc(void);
	void setSetting(bool &setting, bool value, const char *key, bool commit);
	void setSetting(int &setting, int value, const char *key, bool commit);
	void setSetting(TCULong &setting, TCULong value, const char *key,
		bool commit);
	void setSetting(TCFloat &setting, TCFloat value, const char *key,
		bool commit);
	void setSetting(std::string &setting, const std::string value,
		const char *key, bool commit);
	void setColorSetting(TCULong &setting, TCULong value, const char *key,
		bool commit);
	bool getBoolSetting(const char *key, bool defaultValue = false);
	long getLongSetting(const char *key, long defaultValue = 0);
	int getIntSetting(const char *key, int defaultValue = 0);
	float getFloatSetting(const char *key, float defaultValue = 0.0f);
	std::string getStringSetting(const char *key,
		const char *defaultValue = NULL);
	TCULong getColorSetting(const char *key, TCULong defaultColor = 0);

	virtual void getRGB(int color, int &r, int &g, int &b);

	// These are called from the constructor, and cannot be properly made into
	// virtual functions.
	// *************************************************************************
	virtual void setupDefaultRotationMatrix(void);
	void setupModelCenter(void);
	void setupModelSize(void);
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
	int m_curveQuality;
	bool m_qualityStuds;
	bool m_hiResPrimitives;

	// Update settings
	int m_proxyType;
	std::string m_proxyServer;
	int m_proxyPort;
	bool m_checkPartTracker;

	// Settings with no UI
	bool m_skipValidation;
	TCFloat m_defaultZoom;
	TCFloat m_zoomMax;
	TCVector m_lightVector;

	StringBoolMap changedSettings;
	StringBoolMap globalSettings;
};

#endif __LDPREFERENCES_H__
