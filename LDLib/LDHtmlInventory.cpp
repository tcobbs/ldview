#include "LDHtmlInventory.h"
#include "LDPartsList.h"
#include "LDPartCount.h"
#include <LDLoader/LDLMainModel.h>
#include <TCFoundation/TCLocalStrings.h>
#include "LDPreferences.h"
#include <string>

const char *LDHtmlInventory::sm_style = "\
body\n\
{\n\
	font-family: tahoma, sans-serif;\n\
}\n\
\n\
table\n\
{\n\
	border-collapse: collapse;\n\
	border: 2px solid #000000;\n\
	background-color: #FFFFDD;\n\
	padding: 0px;\n\
}\n\
\n\
th.titleImage\n\
{\n\
	background-color: #EEEEEE;\n\
	color: #000000;\n\
	font-size: 150%;\n\
	border-bottom-style: none;\n\
	padding-bottom: 0px;\n\
}\n\
\n\
th\n\
{\n\
	background-color: #0080D0;\n\
	color: #FFFFFF;\n\
	border-bottom: 1px solid #000000;\n\
	border-right: 1px solid #00558A;\n\
	padding: 4px 8px;\n\
}\n\
\n\
th.title\n\
{\n\
	background-color: #EEEEEE;\n\
	color: #000000;\n\
	font-size: 150%;\n\
}\n\
\n\
td\n\
{\n\
	vertical-align: top;\n\
	background-color: #FFFFDD;\n\
	border-bottom: 1px solid #BFBFA5;\n\
	border-right: 1px solid #BFBFA5;\n\
	padding: 2px 8px;\n\
}\n\
\n\
td.quantity\n\
{\n\
	text-align: right;\n\
}\n\
\n\
td.colorNumber\n\
{\n\
	text-align: right;\n\
	border-right-style: none;\n\
}\n\
\n\
td.image\n\
{\n\
	border-right-style: none;\n\
}\n\
\n\
table.color\n\
{\n\
	border-style: none;\n\
	width: 100%;\n\
}\n\
\n\
table.color td\n\
{\n\
	border-style: none;\n\
	padding: 0px 8px 0px 0px;\n\
	width: 100%\n\
}\n\
\n\
table.color td.colorBox\n\
{\n\
	padding: 1px 0px;\n\
	width: auto\n\
}\n\
\n\
table.colorBox\n\
{\n\
	border: 1px solid black;\n\
	width: 2em;\n\
	height: 1em;\n\
	padding: 0px;\n\
}\n\
\n\
table.colorBox td\n\
{\n\
	padding: 0px;\n\
}\n\
\n\
table.credits\n\
{\n\
	border-collapse: collapse;\n\
	border-style: none;\n\
	background-color: transparent;\n\
	margin: 0px;\n\
	width: 100%;\n\
}\n\
\n\
td.credits\n\
{\n\
	background-color: #EEEEEE;\n\
	border-style: none;\n\
	padding: 0px;\n\
}\n\
\n\
table.credits td\n\
{\n\
	background-color: transparent;\n\
	color: #808080;\n\
	border-style: none;\n\
	font-size: 65%;\n\
}\n\
\n\
a\n\
{\n\
	text-decoration: none;\n\
	border-style: none;\n\
}\n\
\n\
a:link\n\
{\n\
	color: #000080;\n\
}\n\
\n\
a:visited\n\
{\n\
	color: #0000FF;\n\
}\n\
\n\
a:hover\n\
{\n\
	text-decoration: underline;\n\
	color: #000080;\n\
}\n\
\n\
img\n\
{\n\
	float: right;\n\
}\n\
\n\
th img\n\
{\n\
	float: none;\n\
}\n\
\n\
a img\n\
{\n\
	border-style: none;\n\
}\n\
\n\
:link:hover img\n\
{\n\
	background-color: #D0E8FF;\n\
}\n\
\n\
:visited:hover img\n\
{\n\
	background-color: #A0C0FF;\n\
}\n\
";

const char *LDHtmlInventory::sm_cssHeader = "\
/* CSS Style Sheet auto-generated by LDView */\n\
\n\
";

const char *LDHtmlInventory::sm_cssFilename = "LDViewPartsList.css";

LDHtmlInventory::LDHtmlInventory(void) :
	m_prefs(new LDPreferences)
{
	int i;

	m_prefs->loadInventorySettings();
	m_showModel = m_prefs->getInvShowModel();
	m_externalCss = m_prefs->getInvExternalCss();
	m_partImages = m_prefs->getInvPartImages();
	m_showFile = m_prefs->getInvShowFile();
	m_lastSavePath = m_prefs->getInvLastSavePath();
	const LongVector &columnOrder = m_prefs->getInvColumnOrder();
	for (i = 0; i < (int)columnOrder.size(); i++)
	{
		m_columnOrder.push_back((LDPartListColumn)columnOrder[i]);
	}
}

LDHtmlInventory::~LDHtmlInventory(void)
{
}

void LDHtmlInventory::dealloc(void)
{
	m_prefs->release();
	TCObject::dealloc();
}

void LDHtmlInventory::populateColumnMap(void)
{
	int i;

	m_columns = m_columnOrder.size();
	m_columnMap.clear();
	for (i = 0; i < (int)m_columnOrder.size(); i++)
	{
		LDPartListColumn column = m_columnOrder[i];

		m_columnMap[column] = true;
		switch (column)
		{
		case LDPLCPart:
			if (m_partImages)
			{
				m_columns++;
			}
			break;
		case LDPLCColor:
			m_columns++;
			break;
		default:
			// Get rid of gcc warning
			break;
		}
	}
}

bool LDHtmlInventory::generateHtml(
	const char *filename,
	LDPartsList *partsList,
	const char *modelName)
{
	FILE *file = fopen(filename, "w");
	size_t nSlashSpot;

	m_lastSavePath = filename;
	populateColumnMap();
	nSlashSpot = m_lastSavePath.find_last_of("/\\");
	if (nSlashSpot < m_lastSavePath.size())
	{
		m_lastSavePath = m_lastSavePath.substr(0, nSlashSpot);
	}
	m_prefs->setInvLastSavePath(m_lastSavePath.c_str());
	m_prefs->commitInventorySettings();
	m_modelName = modelName;
	nSlashSpot = m_modelName.find_last_of("/\\");
	if (nSlashSpot < m_modelName.size())
	{
		m_modelName = m_modelName.substr(nSlashSpot + 1);
	}
	if (file)
	{
		const LDPartCountVector &partCounts = partsList->getPartCounts();
		int i, j;

		writeHeader(file);
		writeTableHeader(file);
		for (i = 0; i < (int)partCounts.size(); i++)
		{
			const LDPartCount &partCount = partCounts[i];
			const IntVector &colors = partCount.getColors();
			LDLModel *model = const_cast<LDLModel *>(partCount.getModel());
			LDLPalette *palette = model->getMainModel()->getPalette();
			//int partTotal = partCount.getTotalCount();

			for (j = 0; j < (int)colors.size(); j++)
			{
				int colorNumber = colors[j];
				LDLColorInfo colorInfo = palette->getAnyColorInfo(colorNumber);

				writePartRow(file, partCount, palette, colorInfo, colorNumber);
			}
		}
		writeTableFooter(file);
		writeFooter(file);
		fclose(file);
		return true;
	}
	else
	{
		return false;
	}
}

FILE *LDHtmlInventory::safeOpenCssFile(const std::string &cssFilename,
									   bool &match)
{
	FILE *cssFile = fopen(cssFilename.c_str(), "r");

	match = false;
	if (cssFile)
	{
		size_t headerSize = strlen(sm_cssHeader);
		size_t contentSize = strlen(sm_style);
		long lFileSize;

		fseek(cssFile, 0, SEEK_END);
		lFileSize = ftell(cssFile);
		fseek(cssFile, 0, SEEK_SET);
		//if ((size_t)lFileSize == headerSize + contentSize)
		{
			std::string fileHeader;
			std::string fileContents;

			fileHeader.resize(headerSize);
			fileContents.resize(contentSize);
			fread(&fileHeader[0], headerSize, 1, cssFile);
			fread(&fileContents[0], contentSize, 1, cssFile);
			if (fileHeader == sm_cssHeader &&
				fileContents == sm_style)
			{
				TCByte test;

				if (fread(&test, 1, 1, cssFile) == 0)
				{
					match = true;
				}
			}
		}
		fclose(cssFile);
		return NULL;
	}
	return fopen(cssFilename.c_str(), "w");
}

bool LDHtmlInventory::writeExternalCss(void)
{
	if (m_externalCss)
	{
		std::string cssFilename = m_lastSavePath + "/" + sm_cssFilename;
		bool match;
		FILE *cssFile = safeOpenCssFile(cssFilename, match);

		if (cssFile)
		{
			fprintf(cssFile, "%s", sm_cssHeader);
			fprintf(cssFile, "%s", sm_style);
			fclose(cssFile);
		}
		if (cssFile || match)
		{
			return true;
		}
	}
	return false;
}

void LDHtmlInventory::writeHeader(FILE *file)
{
	fprintf(file, "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" "
		"\"http://www.w3.org/TR/html4/strict.dtd\">\n");
	fprintf(file, "<html>\n");
	fprintf(file, "<head>\n");
	fprintf(file, "<meta http-equiv=\"content-type\" "
		"content=\"text/html;charset=utf-8\">\n");
	char title[1024];
	sprintf(title, TCLocalStrings::get("PLTitle"), m_modelName.c_str());
	fprintf(file, "<title>%s</title>\n", title);
	if (writeExternalCss())
	{
		fprintf(file, "<link href=\"%s\" title=\"%s\" "
			"rel=\"stylesheet\" type=\"text/css\">\n", sm_cssFilename,
			TCLocalStrings::get("PLStyleSheetTitle"));
	}
	else
	{
		fprintf(file, "<style type=\"text/css\" title=\"%s\">\n",
			TCLocalStrings::get("PLStyleSheetTitle"));
		fprintf(file, "%s", sm_style);
		fprintf(file, "</style>\n");
	}
	fprintf(file, "</head>\n");
	fprintf(file, "<body>\n");
}

void LDHtmlInventory::writeFooter(FILE *file)
{
	fprintf(file, "</body>\n");
	fprintf(file, "</html>\n");
}

void LDHtmlInventory::writePartHeaderCell(FILE *file)
{
	int colSpan = 2;

	if (!m_partImages)
	{
		colSpan = 1;
	}
	writeHeaderCell(file, LDPLCPart, colSpan);
}

void LDHtmlInventory::writeHeaderCell(
	FILE *file,
	LDPartListColumn column,
	int colSpan)
{
	if (colSpan == 1)
	{
		fprintf(file, "			<th>%s</th>\n", getColumnName(column));
	}
	else
	{
		fprintf(file, "			<th colspan=\"%d\">%s</th>\n", colSpan,
			getColumnName(column));
	}
}

void LDHtmlInventory::writeHeaderCell(FILE *file, LDPartListColumn column)
{
	switch (column)
	{
	case LDPLCPart:
		writePartHeaderCell(file);
		break;
	case LDPLCDescription:
		writeHeaderCell(file, LDPLCDescription, 1);
		break;
	case LDPLCColor:
		writeHeaderCell(file, LDPLCColor, 2);
		break;
	case LDPLCQuantity:
		writeHeaderCell(file, LDPLCQuantity, 1);
		break;
	}
}

void LDHtmlInventory::writePartCell(
	FILE *file,
	const LDPartCount &partCount,
	LDLPalette *palette,
	const LDLColorInfo &colorInfo,
	int colorNumber)
{
	int peeronColorNumber = colorNumber;
	std::string imgStyle;
	std::string partName = partCount.getFilename();
	size_t nDotSpot = partName.find('.');
	int r, g, b, a;

	palette->getRGBA(colorInfo, r, g, b, a);
	if (nDotSpot < partName.size())
	{
		partName = partName.substr(0, nDotSpot);
	}
	if (peeronColorNumber >= 512)
	{
		char bgColor[10];

		sprintf(bgColor, "%02X%02X%02X", r, g, b);
		peeronColorNumber = 7;
		imgStyle = (std::string)"style = \"padding: 4px; "
			"background-color: #" + bgColor + "\" ";
	}
	if (m_partImages)
	{
		std::string className;
		bool official = partCount.getModel()->isOfficial();

		//if (official)
		{
			className = " class=\"image\"";
		}
		fprintf(file, "			<td%s>"
			"<a href=\"http://peeron.com/inv/parts/%s\">",
			className.c_str(), partName.c_str());
		if (official)
		{
			fprintf(file, "<img %salt=\"%s\" title=\"%s\" "
			"src=\"http://media.peeron.com/ldraw/images/%d/100/%s.png\">",
			imgStyle.c_str(), TCLocalStrings::get("PLViewOnPeeron"),
			TCLocalStrings::get("PLViewOnPeeron"), peeronColorNumber,
			partName.c_str());
		}
		else
		{
			fprintf(file, "%s", TCLocalStrings::get("PLViewOnPeeron"));
		}
		fprintf(file, "</a></td>\n");
	}
	fprintf(file, "			<td>%s</td>\n", partName.c_str());
}

void LDHtmlInventory::writeDescriptionCell(
	FILE *file,
	const LDPartCount &partCount)
{
	if (m_columnMap[LDPLCDescription])
	{
		const char *description = partCount.getModel()->getDescription();
		if (description)
		{
			fprintf(file, "			<td>%s</td>\n", description);
		}
		else
		{
			fprintf(file, "			<td>&lt;%s&gt;</td>\n",
				TCLocalStrings::get("PLNoDescription"));
		}
	}
}

void LDHtmlInventory::writeColorCell(
	FILE *file,
	LDLPalette *palette,
	const LDLColorInfo &colorInfo,
	int colorNumber)
{
	int r, g, b, a;

	palette->getRGBA(colorInfo, r, g, b, a);
	if (strlen(colorInfo.name))
	{
		fprintf(file, "			<td class=\"colorNumber\">%d:</td>\n",
			colorNumber);
	}
	else
	{
		if (colorNumber < 512)
		{
			fprintf(file, "			<td class=\"colorNumber\">%d"
				"</td>\n", colorNumber);
		}
		else
		{
			fprintf(file, "			<td class=\"colorNumber\">#%X"
				"</td>\n", colorNumber);
		}
	}
	fprintf(file, "			<td>\n");
	fprintf(file, "				<table class=\"color\">\n");
	fprintf(file, "					<tr>\n");
	fprintf(file, "						<td rowspan=\"2\">"
		"%s<td>\n", colorInfo.name);
	fprintf(file, "						<td class=\"colorBox\"><table "
		"class=\"colorBox\"><tr><td style=\"background-color: #%02X%02X%02X\">"
		"&nbsp;</td></tr></table></td>\n", r, g, b);
	fprintf(file, "					</tr>\n");
	fprintf(file, "				</table>\n");
	fprintf(file, "			</td>\n");
}

void LDHtmlInventory::writeQuantityCell(
	FILE *file,
	const LDPartCount &partCount,
	int colorNumber)
{
	fprintf(file, "			<td class=\"quantity\">%d</td>\n",
		partCount.getColorCount(colorNumber));
}

void LDHtmlInventory::writeCell(
	FILE *file, LDPartListColumn column,
	const LDPartCount &partCount,
	LDLPalette *palette,
	const LDLColorInfo &colorInfo,
	int colorNumber)
{
	switch (column)
	{
	case LDPLCPart:
		writePartCell(file, partCount, palette, colorInfo, colorNumber);
		break;
	case LDPLCDescription:
		writeDescriptionCell(file, partCount);
		break;
	case LDPLCColor:
		writeColorCell(file, palette, colorInfo, colorNumber);
		break;
	case LDPLCQuantity:
		writeQuantityCell(file, partCount, colorNumber);
		break;
	}
}

void LDHtmlInventory::writeTableHeader(FILE *file)
{
	const char *ldviewCreditAlign = "left";
	size_t i;

	fprintf(file, "<table>\n");
	fprintf(file, "	<thead>\n");
	if (m_showModel)
	{
		size_t nDotSpot = m_modelName.find('.');
		std::string modelName;

		if (nDotSpot < m_modelName.size())
		{
			modelName = m_modelName.substr(0, nDotSpot);
		}
		else
		{
			modelName = m_modelName;
		}
		fprintf(file, "		<tr>\n");
		fprintf(file, "			<th class=\"titleImage\" "
			"colspan=\"%d\">\n", m_columns);
		fprintf(file, "				<img alt=\"&lt;%s&gt;\" "
			"title=\"&lt;%s&gt;\" src=\"%s.png\">\n",
			m_modelName.c_str(), m_modelName.c_str(), modelName.c_str());
		fprintf(file, "			</th>\n");
		fprintf(file, "		</tr>\n");
	}
	fprintf(file, "		<tr>\n");
	fprintf(file, "			<th class=\"title\" colspan=\"%d\">\n", m_columns);
	char title[1024];
	sprintf(title, TCLocalStrings::get("PLTitle"), m_modelName.c_str());
	fprintf(file, "				%s\n", title);
	fprintf(file, "			</th>\n");
	fprintf(file, "		</tr>\n");
	fprintf(file, "		<tr>\n");
	for (i = 0; i < m_columnOrder.size(); i++)
	{
		writeHeaderCell(file, m_columnOrder[i]);
	}
	fprintf(file, "		</tr>\n");
	fprintf(file, "	</thead>\n");
	fprintf(file, "	<tfoot>\n");
	fprintf(file, "		<tr>\n");
	fprintf(file, "			<td colspan=\"%d\" class=\"credits\">\n",
		m_columns);
	fprintf(file, "				<table class=\"credits\">\n");
	fprintf(file, "					<tbody>\n");
	fprintf(file, "						<tr>\n");
	if (!m_partImages)
	{
		ldviewCreditAlign = "center";
	}
	fprintf(file, "							<td align=\"%s\">\n",
		ldviewCreditAlign);
	fprintf(file, "								%s\n",
		TCLocalStrings::get("PLGeneratedBy"));
	fprintf(file, "							</td>\n");
	if (m_partImages)
	{
		fprintf(file, "							<td align=\"right\">\n");
		fprintf(file, "								%s\n",
			TCLocalStrings::get("PLProvidedBy"));
		fprintf(file, "							</td>\n");
	}
	fprintf(file, "						</tr>\n");
	fprintf(file, "					</tbody>\n");
	fprintf(file, "				</table>\n");
	fprintf(file, "			</td>\n");
	fprintf(file, "		</tr>\n");
	fprintf(file, "	</tfoot>\n");
	fprintf(file, "	<tbody>\n");
}

void LDHtmlInventory::writeTableFooter(FILE *file)
{
	fprintf(file, "	</tbody>\n");
	fprintf(file, "</table>\n");
}

void LDHtmlInventory::writePartRow(
	FILE *file,
	const LDPartCount &partCount,
	LDLPalette *palette,
	const LDLColorInfo &colorInfo,
	int colorNumber)
{
	size_t i;

	fprintf(file, "		<tr>\n");
	for (i = 0; i < m_columnOrder.size(); i++)
	{
		writeCell(file, m_columnOrder[i], partCount, palette, colorInfo,
			colorNumber);
	}
	fprintf(file, "		</tr>\n");
}

void LDHtmlInventory::setShowModelFlag(bool value)
{
	m_showModel = value;
	m_prefs->setInvShowModel(value);
}

void LDHtmlInventory::setExternalCssFlag(bool value)
{
	m_externalCss = value;
	m_prefs->setInvExternalCss(value);
}

void LDHtmlInventory::setPartImagesFlag(bool value)
{
	m_partImages = value;
	m_prefs->setInvPartImages(value);
}

void LDHtmlInventory::setShowFileFlag(bool value)
{
	// Note: this class doesn't actually use the flag; it just keeps track of
	// the value and makes sure it gets saved and restored in the user defaults.
	m_showFile = value;
	m_prefs->setInvShowFile(value);
}

void LDHtmlInventory::setColumnOrder(const LDPartListColumnVector &value)
{
	LongVector columnOrder;
	int i;

	m_columnOrder = value;
	for (i = 0; i < (int)value.size(); i++)
	{
		columnOrder.push_back(value[i]);
	}
	m_prefs->setInvColumnOrder(columnOrder);
}

bool LDHtmlInventory::isColumnEnabled(LDPartListColumn column)
{
	populateColumnMap();
	return m_columnMap[column];
}

const char *LDHtmlInventory::getColumnName(LDPartListColumn column)
{
	switch (column)
	{
	case LDPLCPart:
		return TCLocalStrings::get("PLPartColName");
		break;
	case LDPLCDescription:
		return TCLocalStrings::get("PLDescriptionColName");
		break;
	case LDPLCColor:
		return TCLocalStrings::get("PLColorColName");
		break;
	case LDPLCQuantity:
		return TCLocalStrings::get("PLQuantityColName");
		break;
	}
	return "<Unknown Column Name>";
}
