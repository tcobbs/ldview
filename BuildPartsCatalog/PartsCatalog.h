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

typedef std::vector<std::string> StringVector;
typedef std::set<std::string> StringSet;
typedef std::map<std::string, std::string> StringMap;

class LDLMainModel;
class LDLModel;

class PartsCatalog {
public:
	bool build();
private:
	static StringSet largeParts;
	static StringVector panelPrefixes1;
	static StringVector panelPrefixes2;
	static std::string commonOptionsRaw;
	static std::string imageSize;
	static std::string edgeThickness;
	std::string ldrawDir;
	std::string catalogDir;
	std::string lastDir;
	std::string imgDir;
	std::string catDir;
	std::string fullDir;
	std::string ldview;
	StringVector partNames;
	StringVector baseplateNames;
	StringVector panelNames;
	std::regex panelRegex1;
	std::regex panelRegex2;
	StringMap variables;
	std::string commonOptions;

	bool initDirs();
	void initRegexes();
	bool initVariables();
	std::string getEnv(const std::string& name);
	bool verifyDir(const std::string& dirString, bool create = true);
	StringVector scanParts(const std::string ldrawDir);
	bool classifyParts(LDLMainModel *mainModel);
	bool classifyPart(LDLModel *model);
	bool isBaseplate(LDLModel *model, const char* category);
	bool isPanel(LDLModel *model);
	bool generateImages(const StringVector& parts, const std::string& options);
	std::string joinStrings(const StringVector& input, const char* delim);
	std::string replaceVariables(const std::string& input);
	static std::vector<StringVector> splitStrings(const StringVector& input, size_t chunkSize = 50);
};

#endif /* PartsCatalog_h */
