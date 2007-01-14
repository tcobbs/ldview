#ifndef __LDHTMLINVENTORY_H__
#define __LDHTMLINVENTORY_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCStlIncludes.h>
#include <LDLoader/LDLPalette.h>
#include <stdio.h>

class LDPartsList;
class LDPartCount;
class LDLPalette;
class LDPreferences;

class LDHtmlInventory : public TCObject
{
public:
	LDHtmlInventory(void);

	void setShowModelFlag(bool value);
	bool getShowModelFlag(void) { return m_showModel; }
	void setExternalCssFlag(bool value);
	bool getExternalCssFlag(void) { return m_externalCss; }
	const char *getLastSavePath(void) { return m_lastSavePath.c_str(); }

	bool generateHtml(const char *filename, LDPartsList *partsList,
		const char *modelName);
protected:
	virtual ~LDHtmlInventory(void);
	virtual void dealloc(void);
	void writeHeader(FILE *file);
	void writeFooter(FILE *file);
	void writeTableHeader(FILE *file);
	void writeTableFooter(FILE *file);
	void writePartRow(FILE *file, const LDPartCount &partCount,
		LDLPalette *palette, const LDLColorInfo &colorInfo, int colorNumber);
	bool writeExternalCss(void);
	FILE *safeOpenCssFile(const std::string &cssFilename, bool &match);

	std::string m_modelName;
	LDPreferences *m_prefs;

	bool m_showModel;
	bool m_externalCss;
	std::string m_lastSavePath;

	static const char *sm_style;
	static const char *sm_cssHeader;
};

#endif // __LDHTMLINVENTORY_H__
