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

typedef enum
{
	LDPLCFirst = 1,
	LDPLCPart = LDPLCFirst,
	LDPLCDescription,
	LDPLCColor,
	LDPLCQuantity,
	LDPLCLast = LDPLCQuantity
} LDPartListColumn;

#include <TCFoundation/TCTypedValueArray.h>

typedef std::vector<LDPartListColumn> LDPartListColumnVector;
typedef std::map<LDPartListColumn, bool> LDPartListColumnBoolMap;

class LDHtmlInventory : public TCObject
{
public:
	LDHtmlInventory(void);

	void setShowModelFlag(bool value);
	bool getShowModelFlag(void) { return m_showModel; }
	void setExternalCssFlag(bool value);
	bool getExternalCssFlag(void) { return m_externalCss; }
	void setPartImagesFlag(bool value);
	bool getPartImagesFlag(void) { return m_partImages; }
	void setShowFileFlag(bool value);
	bool getShowFileFlag(void) { return m_showFile; }
	const LDPartListColumnVector &getColumnOrder(void) const
	{
		return m_columnOrder;
	}
	void setColumnOrder(const LDPartListColumnVector &value); 
	const char *getLastSavePath(void) { return m_lastSavePath.c_str(); }
	bool isColumnEnabled(LDPartListColumn column);

	bool generateHtml(const char *filename, LDPartsList *partsList,
		const char *modelName);

	static const char *getColumnName(LDPartListColumn column);
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
	void writePartHeaderCell(FILE *file);
	void writeHeaderCell(FILE *file, LDPartListColumn column, int colSpan);
	void writeHeaderCell(FILE *file, LDPartListColumn column);
	void writePartCell(FILE *file, const LDPartCount &partCount,
		LDLPalette *palette, const LDLColorInfo &colorInfo, int colorNumber);
	void writeDescriptionCell(FILE *file, const LDPartCount &partCount);
	void writeColorCell(FILE *file, LDLPalette *palette,
		const LDLColorInfo &colorInfo, int colorNumber);
	void writeQuantityCell(FILE *file, const LDPartCount &partCount,
		int colorNumber);
	void writeCell(FILE *file, LDPartListColumn column,
		const LDPartCount &partCount, LDLPalette *palette,
		const LDLColorInfo &colorInfo, int colorNumber);
	void populateColumnMap(void);

	std::string m_modelName;
	LDPreferences *m_prefs;

	bool m_showModel;
	bool m_externalCss;
	bool m_partImages;
	bool m_showFile;
	std::string m_lastSavePath;
	int m_columns;
	LDPartListColumnVector m_columnOrder;
	LDPartListColumnBoolMap m_columnMap;

	static const char *sm_style;
	static const char *sm_cssHeader;
	static const char *sm_cssFilename;
};

#endif // __LDHTMLINVENTORY_H__
