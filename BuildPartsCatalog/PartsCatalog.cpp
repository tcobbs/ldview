//
//  PartsCatalog.cpp
//  BuildPartsCatalog
//
//  Created by Travis Cobbs on 9/2/25.
//

#include "PartsCatalog.h"
#include "AlertHandler.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <iterator>
#include <LDLoader/LDLMainModel.h>
#include <LDLoader/LDLModelLine.h>
#include <TCFoundation/TCPngImageFormat.h>
#include <TCFoundation/TCDictionary.h>
#include <TCFoundation/TCAlertManager.h>
#include <LDLoader/LDLError.h>

namespace fs = std::filesystem;

std::string PartsCatalog::commonOptionsRaw = "${ldview} '-LDrawDir=${ldraw_dir}' -FOV=0.1 -SaveSnapshots=1 -SnapshotSuffix=.png '-SaveDir=${img_dir}' -SaveWidth=${image_size} -SaveHeight=${image_size} -EdgeThickness=${edge_thickness} -SaveZoomToFit=1 -AutoCrop=1 -SaveAlpha=1 -LineSmoothing=1 -BFC=0 -PreferenceSet=LDPartsCatalog ";
std::string PartsCatalog::imageSize = "512";
std::string PartsCatalog::edgeThickness = "1";
std::string PartsCatalog::cssFilename = "LDrawPartsCatalog.css";
std::string PartsCatalog::indexFilename = "LDrawPartsCatalog.html";

bool PartsCatalog::build() {
	try {
		initDirs();
		initVariables();
		initRegexes();
		StringVector outputPaths = {
			catalogDir,
			lastDir,
			imgDir,
			catDir,
		};
		if (!verifyDir(ldrawDir, false)) {
			std::cerr << "LDraw Dir not found!\n";
			return false;
		}
		for (const auto& dir : outputPaths) {
			if (!verifyDir(dir)) {
				std::cerr << "Error creating output dir: " << dir << "!\n";
				return false;
			}
		}
		LDLMainModel *mainModel = NULL;
		bool retValue = true;
		try {
			auto parts = scanParts();
			auto omniPath = createOmniModel(parts);
			mainModel = loadMainModel(omniPath);
			classifyParts(mainModel);
			generateImages();
			generateHtml(mainModel);
		} catch (...) {
			retValue = false;
		}
		std::cout << "Cleaning up...\n";
		TCObject::release(mainModel);
		std::cout << "Done!\n";
		return retValue;
	} catch (...) {
		return false;
	}
}

LDLMainModel* PartsCatalog::loadMainModel(const std::string& path) {
	std::cout << "Loading omni model...\n";
	auto mainModel = new LDLMainModel();
	AlertHandler *alertHandler = new AlertHandler();
	if (!mainModel->load(path.c_str())) {
		TCObject::release(alertHandler);
		std::cerr << "Error loading omni model!\n";
		throw "Load failed";
	}
	TCObject::release(alertHandler);
	std::cout << "\nDone loading omni model!\n";
	return mainModel;
}

std::string PartsCatalog::createOmniModel(const StringVector& parts) {
	std::fstream omniModel;
	auto omniPath = catalogDir + "/omni.ldr";
	std::cout << "Creating omni model " << omniPath << "...\n";
	try {
		openOutputFile(omniModel, omniPath, std::ofstream::binary);
	} catch (...) {
		std::cerr << "Error creating omni model!\n";
		throw "Error creating omni model";
	}
	for (const auto& part : parts) {
		omniModel << "1 16 0 0 0 1 0 0 0 1 0 0 0 1 " << part << "\n";
	}
	std::cout << "Done creating omni model!\n";
	return omniPath;
}

std::string PartsCatalog::replaceVariables(const std::string& input, const StringMap& variables) {
	std::string result = input;
	for (const auto& pair : variables) {
		std::string var = std::string("${") + pair.first + "}";
		size_t spot;
		while ((spot = result.find(var)) < result.size()) {
			result.replace(spot, var.size(), pair.second);
		}
	}
	return result;
}

void PartsCatalog::generateImages() {
	std::cout << "Generating part images...\n";
	std::cout << "  > Standard...\n";
	if (!generateImages("Parts", partNames, "-cg30,45,275000 ")) {
		throw "Standard images failed";
	}
	std::cout << "  > Standard done!\n";
	std::cout << "  > Panel...\n";
	if (!generateImages("Panels", panelNames, "-cg30,225,275000 ")) {
		throw "Panel images failed";
	}
	std::cout << "  > Panel done!\n";
	std::cout << "  > Baseplate...\n";
	if (!generateImages("Baseplates", baseplateNames, "-cg30,45,550000 ")) {
		throw "Baseplate images failed";
	}
	std::cout << "  > Baseplate done!\n";
	std::cout << "Done generating part images!\n";
}

bool PartsCatalog::generateImages(const std::string& classification, StringVector& parts, const std::string& options) {
	fs::path origPath = fs::current_path();
	try {
		std::fstream lastFile;
		std::string lastPath = lastDir + "/" + "LDraw" + classification + ".txt";
		lastFile.open(lastPath, std::fstream::in);
		std::string line;
		StringSet lastParts;
		while (std::getline(lastFile, line)) {
			lastParts.insert(line);
		}
		if (!lastParts.empty()) {
			for (size_t i = parts.size(); i > 0; --i) {
#if __cplusplus < 202002L
				if (lastParts.find(parts[i - 1]) != lastParts.end()) {
#else // C++20
				if (lastParts.contains(parts[i - 1])) {
#endif // C++20
					parts.erase(parts.begin() + (i - 1));
				}
			}
		}
		if (parts.empty()) {
			std::cout << "  > All images already generated on past run!\n";
			return true;
		}
		lastFile.close();
		lastFile.open(lastPath, std::fstream::out | std::fstream::app);
		fs::current_path(ldrawDir + "/parts");
		const std::vector<StringVector> partsChunks = splitStrings(parts);
		size_t i = 0;
		for (const auto& chunk : partsChunks) {
			std::cout << "  > " << ++i << "/" << partsChunks.size() << " (" << std::fixed << std::setprecision(2) << (i - 1.0f) / partsChunks.size() * 100.0 << "%)" << std::flush;
			auto partsList = joinStrings(chunk, " ");
			auto command = commonOptions + options + partsList;
			int result;
			bool failed = false;
			for (int i = 0; (result = std::system(command.c_str())) != 0 && i < 10; ++i) {
				std::cerr << "Non-zero exit code from LDView: " << result << "; will try 10 times.\n";
				failed = true;
			}
			if (failed) {
				if (result == 0) {
					std::cerr << "  > Succeeded!\n";
				} else {
					std::cerr << "10 LDView failures in a row; exiting!\n";
					return false;
				}
			}
			std::cout << "\r";
			for (const auto& part : chunk) {
				lastFile << part << "\n";
			}
			lastFile << std::flush;
		}
		std::cout << "\n";
		fs::current_path(origPath);
		return true;
	} catch (...) {
		return false;
	}
}

#ifdef __APPLE__
void PartsCatalog::initDirsApple() {
	std::string homeDir;
	try {
		homeDir = getEnv("HOME");
	} catch (...) {
		std::cerr << "HOME environment varible not set!\n";
		throw "HOME not set";
	}
	if (ldrawDir.empty()) {
		ldrawDir = homeDir + "/Library/LDraw";
	}
	catalogDir = homeDir + "/tmp/LDrawCatalog-Native";
	ldview = "'/Applications/LDView-4.5.app/Contents/MacOS/LDView'";
}
#endif // __APPLE__

#ifdef WIN32
void PartsCatalog::initDirsWindows() {
	std::string userProfileDir;
	try {
		userProfileDir = getEnv("USERPROFILE");
		replaceStringCharacter(userProfileDir, '\\', '/');
	}
	catch (...) {
		std::cerr << "USERPROFILE environment varible not set!\n";
		throw "USERPROFILE not set";
	}
	if (ldrawDir.empty()) {
		ldrawDir = userProfileDir + "/LDRAW";
	}
	catalogDir = userProfileDir + "/LDrawCatalog";
	ldview = "C:/Program Files/LDView/LDView64.exe";
	replaceStringCharacter(ldview, '/', '\\');
	replaceStringCharacter(ldrawDir, '/', '\\');
	char shortPath[MAX_PATH];
	DWORD result = GetShortPathNameA(ldview.c_str(), shortPath, sizeof(shortPath));
	if (result == 0 >> result > MAX_PATH) {
		throw "Error getting LDView.exe short path!";
	}
	ldview = shortPath;
}
#endif // WIN32

#if !defined (WIN32) && !defined (__APPLE__)
void PartsCatalog::initDirsLinux() {
	if (ldrawDir.empty()) {
		ldrawDir = "/usr/share/ldraw";
	}
	catalogDir = "/tmp/LDrawCatalog";
	ldview = "/usr/bin/LDView";
}
#endif

void PartsCatalog::initDirs() {
	try {
		ldrawDir = getEnv("LDRAW_DIR");
	} catch (...) {}
#ifdef __APPLE__
	initDirsApple();
#endif // __APPLE__
#ifdef WIN32
	initDirsWindows();
#endif // WIN32
#if !defined (WIN32) && !defined (__APPLE__)
	initDirsLinux();
#endif
	if (ldrawDir.empty()) {
		std::cerr << "Cannot determine LDraw directory!\n";
		throw "No LDraw dir";
	}
	LDLModel::setLDrawDir(ldrawDir.c_str());
	lastDir = catalogDir + "/last";
	imgDir = catalogDir + "/img";
	catDir = catalogDir + "/cat";
}

std::string PartsCatalog::joinStrings(const StringVector& input, const char* delim) {
	std::stringstream ss;
	std::copy(input.begin(), input.end(), std::ostream_iterator<std::string>(ss, delim));
	std::string result = ss.str();
	if (result.empty()) {
		return result;
	} else {
		return result.substr(0, result.size() - strlen(delim));
	}
}

void PartsCatalog::initVariables() {
	variables = {
		{ "ldview", ldview },
		{ "ldraw_dir", ldrawDir },
		{ "img_dir", imgDir },
		{ "image_size", imageSize },
		{ "edge_thickness", edgeThickness },
	};
	commonOptions = replaceVariables(commonOptionsRaw, variables);
#ifdef WIN32
	replaceStringCharacter(commonOptions, '\'', '"');
#endif // WIN32
}

void PartsCatalog::initRegexes() {
	std::string prefixes1 = joinStrings(Consts::panelPrefixes1, "|");
	std::string pattern1 = "^(" + prefixes1 + ")[^0-9].*";
	panelRegex1 = pattern1;
	std::string prefixes2 = joinStrings(Consts::panelPrefixes2, "|");
	std::string pattern2 = "^(" + prefixes2 + ")[0-9]+\\..*";
	panelRegex2 = pattern2;
}

bool PartsCatalog::verifyDir(const std::string& dirString, bool create) {
	fs::path cwd = fs::current_path();
	fs::path dir(dirString);
	try {
		fs::current_path(dir);
		fs::current_path(cwd);
		return true;
	} catch (...) {
	}
	if (!create) {
		return false;
	}
	try {
		fs::create_directories(dir);
		return true;
	} catch (...) {
		return false;
	}
}

std::string PartsCatalog::getEnv(const std::string& name) {
#ifdef WIN32
	char* buffer = NULL;
	size_t count = 0;
	if (_dupenv_s(&buffer, &count, name.c_str()) == 0 && buffer != NULL) {
		std::string result = buffer;
		free(buffer);
		return result;
	}
	else {
		throw "Not Found";
	}
#else // WIN32
	const char* value = std::getenv(name.c_str());
	if (value == NULL) {
		throw "Not Found";
	}
	return value;
#endif // !WIN32
}

StringVector PartsCatalog::scanParts() {
	std::cout << "Scanning for parts...\n";
	auto parts = scanParts(ldrawDir);
	if (parts.empty()) {
		std::cerr << "No parts found!\n";
		throw "No parts found";
	}
	std::cout << "Done scanning for parts!\n";
	return parts;
}

StringVector PartsCatalog::scanParts(const std::string ldrawDir) {
	StringVector result;
	fs::path partsDir(ldrawDir + "/parts");
	for (const auto& entry : fs::directory_iterator(partsDir)) {
		if (entry.is_regular_file()) {
			result.push_back(entry.path().filename().string());
		}
	}
	return result;
}

void PartsCatalog::classifyParts(LDLMainModel *mainModel) {
	std::cout << "Classifying parts...\n";
	LDLFileLineArray* fileLines = mainModel->getFileLines();
	for (size_t i = 0; i < fileLines->getCount(); ++i) {
		LDLFileLine* fileLine = (*fileLines)[i];
		if (fileLine->getLineType() == LDLLineType::LDLLineTypeModel) {
			LDLModel* model = ((LDLModelLine*)fileLine)->getModel();
			try {
				classifyPart(model);
			} catch (...) {
			}
		}
	}
	std::cout << "Done classifying parts!\n";
}

bool PartsCatalog::isBaseplate(LDLModel *model, const char* category) {
	if (strcasecmp(category, "baseplate") == 0) {
		return true;
	}
	std::string name = lowerCaseString(model->getName());
	removeExtenstion(name);
#if __cplusplus < 202002L
	return Consts::largeParts.find(name) != Consts::largeParts.end();
#else // C++20
	return Consts::largeParts.contains(name);
#endif // C++20
}

bool PartsCatalog::isPanel(LDLModel *model) {
	std::string name = lowerCaseString(model->getName());
	std::smatch match;
	if (std::regex_match(name, match, panelRegex1) || std::regex_match(name, match, panelRegex1)) {
		return true;
	}
	return false;
}

void PartsCatalog::classifyPart(LDLModel *model) {
	const char* category = model->getCategory();
	if (category == NULL) {
		std::cerr << "Part without category (ignoring): " << model->getName() << "\n";
		throw "Part without category";
	}
	categories[category].insert(model->getName());
	if (isBaseplate(model, category)) {
		baseplateNames.push_back(model->getName());
	} else if (isPanel(model)) {
		panelNames.push_back(model->getName());
	} else {
		partNames.push_back(model->getName());
	}
}

std::vector<StringVector> PartsCatalog::splitStrings(const StringVector& input, size_t chunkSize)
{
	std::vector<StringVector> chunks;
	for (size_t i = 0; i < input.size(); i += chunkSize) {
		auto last = std::min(input.size(), i + chunkSize);
		chunks.emplace_back(input.begin() + i, input.begin() + last);
	}
	return chunks;
}

void PartsCatalog::generateHtml(LDLMainModel* mainModel) {
	std::cout << "Generating HTML...\n";
	generateStyleSheet();
	generateHtmlPages(mainModel);
	std::cout << "Done generating HTML!\n";
}

void PartsCatalog::generateStyleSheet() {
	std::fstream cssFile;
	auto cssPath = catalogDir + "/" + cssFilename;
	std::cout << "Creating style sheet " << cssPath << "...\n";
	try {
		openOutputFile(cssFile, cssPath);
		writeStyleSheet(cssFile);
	} catch (...) {
		std::cerr << "Error creating style sheet!\n";
		throw "Error creating style sheet";
	}
	std::cout << "Done creating style sheet!\n";
}

void PartsCatalog::writeStyleSheet(std::fstream& file) {
	file << Consts::styleSheet;
}

void PartsCatalog::generateHtmlPages(LDLMainModel* mainModel) {
	std::cout << "Generating HTML pages...\n";
	generateIndex(mainModel);
	generateCategoryPages(mainModel);
	std::cout << "Done generating HTML pages!\n";
}

void PartsCatalog::generateIndex(LDLMainModel* mainModel) {
	std::fstream indexFile;
	auto indexPath = catalogDir + "/" + indexFilename;
	std::cout << "Creating index " << indexPath << "...\n";
	try {
		openOutputFile(indexFile, indexPath);
		writeHeader(indexFile, "LDraw Parts");
		indexFile << Consts::mainPrefix;
		for (const auto& pair : categories) {
			const std::string& category = pair.first;
			const char* categoryFile = stringByReplacingSubstring(category.c_str(), " ", "%20");
			StringMap variables = {
				{ "category_file", categoryFile },
				{ "category", category },
			};
			delete[] categoryFile;
			indexFile << replaceVariables(Consts::categoryRow, variables);
		}
		indexFile << Consts::htmlSuffix;
	} catch (...) {
		std::cerr << "Error creating index!\n";
		throw "Error creating index";
	}
}

void PartsCatalog::writeHeader(std::fstream& file, const std::string& title) {
	StringMap variables = {
		{ "title", title },
		{ "style_sheet", Consts::styleSheet },
	};
	file << replaceVariables(Consts::htmlHeader, variables);
}

void PartsCatalog::openOutputFile(std::fstream& file, const std::string& filename, std::ios_base::openmode openMode) {
	file.open(filename, std::ofstream::out | std::ofstream::trunc | openMode);
}

void PartsCatalog::generateCategoryPages(LDLMainModel *mainModel) {
	for (const auto& pair : categories) {
		generateCategoryPage(pair.first, pair.second, mainModel);
	}
}

void PartsCatalog::generateCategoryPage(const std::string& category, const StringSet& parts, LDLMainModel *mainModel) {
	std::fstream catFile;
	auto catPath = catalogDir + "/cat/" + "LDraw" + category + ".html";
	std::cout << "Creating category page " << catPath << "...\n";
	auto loadedModels = mainModel->getLoadedModels();
	try {
		openOutputFile(catFile, catPath);
		writeHeader(catFile, category);
		catFile << replaceVariables(Consts::catPrefix, {{ "category", category}});
		for (const auto& part : parts) {
			LDLModel* model = (LDLModel*)loadedModels->objectForKey(part.c_str());
			auto keywords = model->getKeywords();
			if (model == NULL) {
				std::cerr << "Model not found for part " << part << "!\n";
				throw "Model not found";
			}
			const char* description = model->getDescription();
			if (description == NULL) {
				std::cerr << "No description for part (skipping): " << part << "!\n";
				continue;
			}
			std::string partName(part);
			removeExtenstion(partName);
			auto pngPath = imgDir + "/" + partName + ".png";
#ifdef WIN32
			FILE* pngFile = NULL;
			if (fopen_s(&pngFile, pngPath.c_str(), "rb") != 0) {
				if (pngFile != NULL) {
					fclose(pngFile);
				}
				pngFile = NULL;
			}
#else // WIN32
			FILE* pngFile = fopen(pngPath.c_str(), "rb");
#endif // !WIN32
			if (pngFile == NULL) {
				std::cerr << "Error opening part image: " << pngPath << "!\n";
				throw "Error opening part png";
			}
			int width, height, bpp;
			if (!TCPngImageFormat::getInfo(pngFile, width, height, bpp)) {
				fclose(pngFile);
				std::cerr << "Error getting part image dimentions: " << pngPath << "!\n";
				throw "Error getting part png dimensions";
			}
			fclose(pngFile);
			long rowSpan = 2;
			if (!keywords.empty()) {
				rowSpan = 3;
			}
			StringMap variables = {
				{ "png_width", ltostr(width) },
				{ "png_height", ltostr(height) },
				{ "png_filename", partName + ".png" },
				{ "part_filename", part },
				{ "rowspan", ltostr(rowSpan) },
				{ "desc", description },
			};
			catFile << replaceVariables(Consts::partRow, variables);
			if (!keywords.empty()) {
				catFile << replaceVariables(Consts::keywordsRow, {{ "keywords", joinStrings(keywords, ", ")}});
			}
		}
		catFile << Consts::htmlSuffix;
	} catch (...) {
		std::cerr << "Error creating category page!\n";
		throw "Error creating category page";
	}

}
