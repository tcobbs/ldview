//
//  PartsCatalog.hpp
//  BuildPartsCatalog
//
//  Created by Travis Cobbs on 9/2/25.
//

#ifndef PartsCatalog_h
#define PartsCatalog_h

#include <string>
#include <vector>
#include <set>
#include <regex>
#include <map>
#include <fstream>
#include "Consts.h"

typedef std::map<std::string, std::string> StringMap;
typedef std::map<std::string, StringSet> CategoryMap;

class LDLMainModel;
class LDLModel;

class PartsCatalog {
public:
	bool build();
private:
	static std::string commonOptionsRaw;
	static std::string imageSize;
	static std::string edgeThickness;
	static std::string cssFilename;
	static std::string indexFilename;
	std::string ldrawDir;
	std::string catalogDir;
	std::string lastDir;
	std::string imgDir;
	std::string catDir;
	std::string ldview;
	StringVector partNames;
	StringVector baseplateNames;
	StringVector panelNames;
	std::regex panelRegex1;
	std::regex panelRegex2;
	StringMap variables;
	std::string commonOptions;
	CategoryMap categories;

	void initDirs();
	void initRegexes();
	void initVariables();
	std::string getEnv(const std::string& name);
	bool verifyDir(const std::string& dirString, bool create = true);
	std::string createDummyModel(const StringVector& parts);
	StringVector scanParts();
	StringVector scanParts(const std::string ldrawDir);
	void classifyParts(LDLMainModel *mainModel);
	void classifyPart(LDLModel *model);
	bool isBaseplate(LDLModel *model, const char* category);
	bool isPanel(LDLModel *model);
	LDLMainModel* loadMainModel(const std::string& path);
	void generateImages();
	bool generateImages(const std::string& classification, StringVector& parts, const std::string& options);
	std::string joinStrings(const StringVector& input, const char* delim);
	std::string replaceVariables(const std::string& input, const StringMap& variables);
	static std::vector<StringVector> splitStrings(const StringVector& input, size_t chunkSize = 47);
	void generateHtml(LDLMainModel *mainModel);
	void generateStyleSheet();
	void writeStyleSheet(std::fstream& file);
	void generateHtmlPages(LDLMainModel* mainModel);
	void generateIndex(LDLMainModel* mainModel);
	void writeHeader(std::fstream& file, const std::string& title);
	void openOutputFile(std::fstream& file, const std::string& filename, std::ios_base::openmode openMode = std::ofstream::out);
	void generateCategoryPages(LDLMainModel *mainModel);
	void generateCategoryPage(const std::string& category, const StringSet& parts, LDLMainModel *mainModel);
#ifdef __APPLE__
	void initDirsApple();
#endif // __APPLE__
};

#endif /* PartsCatalog_h */
