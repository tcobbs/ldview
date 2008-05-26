// LGEOTables.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tinyxml.h>
#include <map>
#include <string>

struct Element
{
	std::string lgeoName;
	std::string lgeoFilename;
	unsigned int flags;
};

struct Color
{
	std::string lgeoName;
};

typedef std::map<std::string, Element> ElementMap;
typedef std::map<int, Color> ColorMap;
typedef std::map<std::string, std::string> StringStringMap;
typedef std::map<std::string, StringStringMap> PatternMap;

bool readString(FILE *tableFile, std::string &string)
{
	for (;;)
	{
		int ch =  fgetc(tableFile);

		if (ch == EOF)
		{
			return false;
		}
		else if (ch == 0)
		{
			return true;
		}
		else
		{
			string += (char)ch;
		}
	}
}

bool readColorsFile(const char *filename, ColorMap &colors)
{
	FILE *colorsFile = fopen(filename, "rb");

	try
	{
		if (colorsFile != NULL)
		{
			for (;;)
			{
				unsigned char numBuf[2];
				size_t readSize = fread(numBuf, 1, 2, colorsFile);
				Color color;
				unsigned int ldrawNum;

				if (readSize == 0)
				{
					// done
					break;
				}
				else if (readSize != 2)
				{
					throw "Error parsing %s.\n";
				}
				ldrawNum = ((unsigned int)numBuf[0] << 8) |
					(unsigned int)numBuf[1];
				color.lgeoName.reserve(10);
				if (!readString(colorsFile, color.lgeoName))
				{
					throw "Error parsing %s.\n";
				}
				colors[ldrawNum] = color;
			}
			fclose(colorsFile);
			return true;
		}
		else
		{
			throw "Error reading %s.\n";
		}
	}
	catch (const char *error)
	{
		printf(error, filename);
		if (colorsFile != NULL)
		{
			fclose(colorsFile);
		}
		return false;
	}
}

bool readElementsFile(
	const char *filename,
	ElementMap &table,
	const PatternMap &patterns)
{
	FILE *tableFile = fopen(filename, "rb");

	try
	{
		if (tableFile != NULL)
		{
			for (;;)
			{
				unsigned char numBuf[4];
				unsigned int ldrawNum;
				int flags;
				char ldrawBase[128];
				std::string ldrawFilename;
				Element element;
				size_t readSize = fread(numBuf, 1, 4, tableFile);
				PatternMap::const_iterator it1;

				if (readSize == 0)
				{
					// done
					break;
				}
				else if (readSize != 4)
				{
					throw "Error parsing %s.\n";
				}
				ldrawNum = ((unsigned int)numBuf[0] << 24) |
					((unsigned int)numBuf[1] << 16) |
					((unsigned int)numBuf[2] << 8) |
					(unsigned int)numBuf[3];
				flags = fgetc(tableFile);
				if (flags == EOF)
				{
					throw "Error parsing %s.\n";
				}
				element.flags = (unsigned int)flags;
				element.lgeoName.reserve(10);
				if (!readString(tableFile, element.lgeoName))
				{
					throw "Error parsing %s.\n";
				}
				element.lgeoFilename = element.lgeoName + ".inc";
				sprintf(ldrawBase, "%d", ldrawNum);
				ldrawFilename = ldrawBase;
				ldrawFilename += ".dat";
				table[ldrawFilename] = element;
				it1 = patterns.find(element.lgeoName.substr(3));
				if (it1 != patterns.end())
				{
					const StringStringMap &strings = it1->second;
					for (StringStringMap::const_iterator it2 = strings.begin();
						it2 != strings.end(); it2++)
					{
						element.lgeoFilename = "patterns/";
						element.lgeoFilename += it2->second;
						element.lgeoFilename += ".inc";
						element.lgeoName = "lg_";
						element.lgeoName += it1->first;
						element.lgeoName += 'p';
						element.lgeoName += it2->first;
						ldrawFilename = ldrawBase;
						ldrawFilename += 'p';
						ldrawFilename += it2->first;
						ldrawFilename += ".dat";
						table[ldrawFilename] = element;
					}
				}
			}
			fclose(tableFile);
			return true;
		}
		else
		{
			throw "Error reading %s.\n";
		}
	}
	catch (const char *error)
	{
		printf(error, filename);
		if (tableFile != NULL)
		{
			fclose(tableFile);
		}
		return false;
	}
}

bool readPatternsFile(const char *filename, PatternMap &patterns)
{
	FILE *tableFile = fopen(filename, "rb");

	try
	{
		if (tableFile != NULL)
		{
			for (;;)
			{
				std::string leftName;
				std::string rightName;
				std::string baseName;
				std::string basePattern;
				size_t index;

				leftName.reserve(10);
				if (!readString(tableFile, leftName))
				{
					if (leftName.size() == 0)
					{
						break;
					}
					else
					{
						throw "Error parsing %s.\n";
					}
				}
				rightName.reserve(10);
				if (!readString(tableFile, rightName))
				{
					throw "Error parsing %s.\n";
				}
				index = leftName.find('p');
				if (index >= leftName.size())
				{
					throw "Error parsing %s.\n";
				}
				baseName = leftName.substr(0, index);
				basePattern = leftName.substr(index + 1);
				StringStringMap &map = patterns[baseName];
				map[basePattern] = rightName;
			}
			fclose(tableFile);
			return true;
		}
		else
		{
			throw "Error reading %s.\n";
		}
	}
	catch (const char *error)
	{
		printf(error, filename);
		if (tableFile != NULL)
		{
			fclose(tableFile);
		}
		return false;
	}
}

TiXmlElement *createXmlRootElement(TiXmlDocument &doc)
{
	TiXmlDeclaration *decl = new TiXmlDeclaration( "1.0", "", "" );
	TiXmlElement *rootElement = new TiXmlElement("LDrawPOV");

	doc.LinkEndChild(decl);
	doc.LinkEndChild(rootElement);
	return rootElement;
}

void addXmlColors(TiXmlElement *rootElement, const ColorMap &colors)
{
	TiXmlElement *colorsElement = new TiXmlElement("Colors");

	for (ColorMap::const_iterator it = colors.begin(); it != colors.end(); it++)
	{
		unsigned int ldrawNum = it->first;
		const Color &color = it->second;
		TiXmlElement *colorElement = new TiXmlElement("Color");
		TiXmlElement *ldrawNumberElement = new TiXmlElement("LDrawNumber");
		TiXmlText *ldrawNumberText;
		TiXmlElement *povNameElement = new TiXmlElement("POVName");
		TiXmlText *povNameText = new TiXmlText(color.lgeoName);
		TiXmlElement *povVersionElement = new TiXmlElement("POVVersion");
		TiXmlText *povVersionText = new TiXmlText("3.0");
		TiXmlElement *povFilenameElement = new TiXmlElement("POVFilename");
		TiXmlText *povFilenameText = new TiXmlText("lg_color.inc");
		char numberBuf[128];

		sprintf(numberBuf, "%d", ldrawNum);
		ldrawNumberText = new TiXmlText(numberBuf);
		ldrawNumberElement->LinkEndChild(ldrawNumberText);
		povNameElement->LinkEndChild(povNameText);
		povVersionElement->LinkEndChild(povVersionText);
		povFilenameElement->LinkEndChild(povFilenameText);
		colorElement->LinkEndChild(ldrawNumberElement);
		colorElement->LinkEndChild(povNameElement);
		colorElement->LinkEndChild(povVersionElement);
		colorElement->LinkEndChild(povFilenameElement);
		colorsElement->LinkEndChild(colorElement);
	}
	rootElement->LinkEndChild(colorsElement);
}

void addXmlElements(TiXmlElement *rootElement, const ElementMap &elementMap)
{
	TiXmlElement *matricesElement = new TiXmlElement("Matrices");
	TiXmlElement *matrixElement = new TiXmlElement("LGEOTransform");
	TiXmlText *matrixText = new TiXmlText("0,0,-25,0,-25,0,0,0,0,-25,0,0,0,0,0,1");
	TiXmlElement *elementsElement = new TiXmlElement("Elements");

	matrixElement->LinkEndChild(matrixText);
	matricesElement->LinkEndChild(matrixElement);
	rootElement->LinkEndChild(matricesElement);
	for (ElementMap::const_iterator it = elementMap.begin();
		it != elementMap.end(); it++)
	{
		const std::string &ldrawFilename = it->first;
		const Element &element = it->second;
		TiXmlElement *elementElement = new TiXmlElement("Element");
		TiXmlElement *ldrawElement = new TiXmlElement("LDrawFilename");
		TiXmlText *ldrawText = new TiXmlText(ldrawFilename);
		TiXmlElement *povNameElement = new TiXmlElement("POVName");
		TiXmlText *povNameText = new TiXmlText(element.lgeoName);
		TiXmlElement *povVersionElement = new TiXmlElement("POVVersion");
		TiXmlText *povVersionText = new TiXmlText("3.0");
		TiXmlElement *povFilenameElement = new TiXmlElement("POVFilename");
		TiXmlText *povFilenameText = new TiXmlText("lg_defs.inc");
		TiXmlElement *povFilename2Element = new TiXmlElement("POVFilename");
		TiXmlText *povFilename2Text = new TiXmlText(element.lgeoFilename);
		TiXmlElement *matrixElement = new TiXmlElement("MatrixRef");
		TiXmlText *matrixText = new TiXmlText("LGEOTransform");

		ldrawElement->LinkEndChild(ldrawText);
		povNameElement->LinkEndChild(povNameText);
		povVersionElement->LinkEndChild(povVersionText);
		povFilenameElement->LinkEndChild(povFilenameText);
		povFilename2Element->LinkEndChild(povFilename2Text);
		matrixElement->LinkEndChild(matrixText);
		elementElement->LinkEndChild(ldrawElement);
		elementElement->LinkEndChild(povNameElement);
		elementElement->LinkEndChild(povVersionElement);
		elementElement->LinkEndChild(povFilenameElement);
		elementElement->LinkEndChild(povFilename2Element);
		elementElement->LinkEndChild(matrixElement);
		elementsElement->LinkEndChild(elementElement);
	}
	rootElement->LinkEndChild(elementsElement);
}

void processFiles(const char *path)
{
	PatternMap patterns;
	ElementMap elements;
	ColorMap colors;
	std::string prefix = path;
	std::string colorsFilename;
	std::string elementsFilename;
	std::string patternsFilename;

	if (prefix.size() > 0)
	{
		char lastChar = prefix[prefix.size() - 1];

		if (lastChar != '/' && lastChar != '\\')
		{
			prefix += '/';
		}
	}
	colorsFilename = prefix + "l2p_colr.tab";
	elementsFilename = prefix + "l2p_elmt.tab";
	patternsFilename = prefix + "l2p_ptrn.tab";
	if (readColorsFile(colorsFilename.c_str(), colors) &&
		readPatternsFile(patternsFilename.c_str(), patterns) &&
		readElementsFile(elementsFilename.c_str(), elements, patterns))
	{
		std::string xmlFilename(prefix + "LDrawPOV.xml");

		TiXmlDocument doc;
		TiXmlElement *rootElement = createXmlRootElement(doc);
		addXmlColors(rootElement, colors);
		addXmlElements(rootElement, elements);
		doc.SaveFile(xmlFilename);
	}
}

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		processFiles("");
	}
	else if (argc == 2)
	{
		processFiles(argv[1]);
	}
	else
	{
		printf("Usage: LGEOTables [LGEO Path]\n");
		return 1;
	}
	return 0;
}

