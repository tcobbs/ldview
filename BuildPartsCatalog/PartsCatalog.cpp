//
//  PartsCatalog.cpp
//  BuildPartsCatalog
//
//  Created by Travis Cobbs on 9/2/25.
//

#include "PartsCatalog.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iterator>
#include <LDLoader/LDLMainModel.h>
#include <LDLoader/LDLModelLine.h>

namespace fs = std::filesystem;

StringSet PartsCatalog::largeParts = {
	"44556",
	"93541",
	"54093",
	"6181",
	"53452",
	"2372c01",
	"2372c01d01",
	"92709",
	"92710",
	"92711",
	"92709c01",
	"92709c02",
	"92709c04",
	"92709c03",
	"65417",
	"65418",
	"20033",
	"18913",
	"18912",
	"20033c01",
	"20033c02",
	"54779",
	"54100",
	"54101",
	"54779c02",
	"54779c01",
	"30215",
	"57786",
	"57915",
	"2372",
	"2373",
	"30072",
	"47116",
	"6161",
	"30477",
	"30401",
	"35199",
	"86501",
	"6490",
	"31012",
	"2842c02",
	"2842c01",
	"2842",
	"2840",
	"23221",
	"74781-f2",
	"74781-f1",
	"74780-f2",
	"74780-f1",
	"2677",
	"2678",
	"2671",
	"2886",
	"2885",
	"2881a",
	"2882",
	"2880a",
	"353",
	"92710c01",
	"92710c02",
	"92710c03",
	"4196",
	"18601",
	"33086",
	"u572p02",
	"u572p01",
	"u9328",
	"u9494",
	"u9494p01",
	"u9499",
	"u9499c01",
	"u9499p01",
	"u9499p01c01",
	"64991",
	"u9495",
	"u9495p01",
	"94318",
	"66645b",
	"66645bp01",
	"61898e",
	"36069b",
	"36069bp01",
	"66645a",
	"66645ap01",
	"36069a",
	"36069ap01",
	"61898d",
	"61898dc01",
	"572c02",
	"99013",
	"99013p01",
	"6391",
	"2972",
	"87058",
	"2869c02",
	"2869c01",
	"866c04",
	"3241ac04",
	"73696c04",
	"73696c03",
	"73696c02",
	"73696c01",
	"73697c04",
	"73697c03",
	"73697c02",
	"73697c01",
	"u9232c02",
	"u9232c01",
	"u9231c02",
	"u9231c01",
	"u9234c02",
	"u9234c01",
	"u9233c02",
	"u9233c01",
	"3229bc04",
	"3229ac04",
	"948ac02",
	"948ac01",
	"949ac02",
	"949ac01",
	"53400c04",
	"2869",
	"u9266",
	"73696c00",
	"73696",
	"864",
	"73697c00",
	"73697",
	"865",
	"u9232c00",
	"u9232",
	"u9220",
	"u9231c00",
	"u9231",
	"u9234c00",
	"u9234",
	"u9221",
	"u9233c00",
	"u9233",
	"948a",
	"949a",
	"948c",
	"948b",
	"949b",
	"35011",
	"35011c01",
};

StringVector PartsCatalog::panelPrefixes1 = {
	"4864",
	"6268",
	"4215",
	"2362",
	"4865",
};
StringVector PartsCatalog::panelPrefixes2 = {
	"4345ap",
	"4345bp",
};

std::string PartsCatalog::commonOptionsRaw = "'${LDVIEW}' '-LDrawDir=${LDRAW_DIR}' -FOV=0.1 -SaveSnapshots=1 '-SaveDir=${IMG_DIR}' -SaveWidth=${image_size} -SaveHeight=${image_size} -EdgeThickness=${edge_thickness} -SaveZoomToFit=1 -AutoCrop=1 -SaveAlpha=1 -LineSmoothing=1 -BFC=0 ";
std::string PartsCatalog::imageSize = "512";
std::string PartsCatalog::edgeThickness = "1";

bool PartsCatalog::build() {
	if (!initDirs()) {
		return false;
	}
	if (!initVariables()) {
		return false;
	}
	initRegexes();
	StringVector outputPaths = {
		catalogDir,
		lastDir,
		imgDir,
		catDir,
		fullDir,
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
	auto parts = scanParts(ldrawDir);
	if (parts.empty()) {
		std::cerr << "No parts found!\n";
	}
	std::fstream dummyModel;
	auto dummyPath = catalogDir + "/dummy.ldr";
	try {
		dummyModel.open(dummyPath, std::ofstream::out | std::ofstream::binary);
	} catch (...) {
		std::cerr << "Error creating dummy model!\n";
		return false;
	}
	size_t i = 0;
	for (const auto& part : parts) {
		dummyModel << "1 16 0 0 0 1 0 0 0 1 0 0 0 1 " << part << "\n";
		if (++i >= 1000) {
			break;
		}
	}
	dummyModel.close();
	LDLMainModel *mainModel = new LDLMainModel();
	std::cout << "Loading dummy model...\n";
	if (!mainModel->load(dummyPath.c_str())) {
		std::cerr << "Error loading dummy model!\n";
		return false;
	}
	std::cout << "Done loading dummy model!\n";
	if (!classifyParts(mainModel)) {
		return false;
	}
	std::cout << "Generating standard part images...\n";
	generateImages(partNames, "-cg30,45,275000 ");
	std::cout << "Generating panel part images...\n";
	generateImages(panelNames, "-cg30,225,275000 ");
	std::cout << "Generating baseplate part images...\n";
	generateImages(baseplateNames, "-cg30,45,550000 ");
	TCObject::release(mainModel);
	return true;
}

std::string PartsCatalog::replaceVariables(const std::string& input) {
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

bool PartsCatalog::generateImages(const StringVector& parts, const std::string& options) {
	fs::path origPath = fs::current_path();
	try {
		fs::current_path(ldrawDir + "/parts");
		const std::vector<StringVector> partsChunks = splitStrings(parts);
		for (const auto& chunk : partsChunks) {
			auto partsList = joinStrings(chunk, " ");
			auto command = commonOptions + options + partsList;
			std::system(command.c_str());
		}
		fs::current_path(origPath);
		return true;
	} catch (...) {
		return false;
	}
}

bool PartsCatalog::initDirs() {
	std::string homeDir;
	try {
		homeDir = getEnv("HOME");
	} catch (...) {
		std::cerr << "HOME environment varible not set!\n";
		return false;
	}
	try {
		ldrawDir = getEnv("LDRAW_DIR");
	} catch (...) {
		ldrawDir = homeDir + "/Library/LDraw";
	}
	catalogDir = homeDir + "/tmp/LDrawCatalog-Native";
	lastDir = catalogDir + "/last";
	imgDir = catalogDir + "/img";
	catDir = catalogDir + "/cat";
	fullDir = catalogDir + "/full";
	ldview = "/Applications/LDView-4.5.app/Contents/MacOS/LDView";
	return true;
}

std::string PartsCatalog::joinStrings(const StringVector& input, const char* delim) {
	std::stringstream ss;
	std::copy(input.begin(), input.end(), std::ostream_iterator<std::string>(ss, delim));
	std::string result = ss.str();
	if (result.empty()) {
		return result;
	} else {
		return result.substr(0, result.size() - 1);
	}
}

bool PartsCatalog::initVariables() {
	variables = {
		{ "LDVIEW", ldview },
		{ "LDRAW_DIR", ldrawDir },
		{ "IMG_DIR", imgDir },
		{ "image_size", imageSize },
		{ "edge_thickness", edgeThickness },
	};
	commonOptions = replaceVariables(commonOptionsRaw);
	return true;
}

void PartsCatalog::initRegexes() {
	std::string prefixes1 = joinStrings(panelPrefixes1, "|");
	std::string pattern1 = "^(" + prefixes1 + ")[^0-9].*";
	panelRegex1 = pattern1;
	std::string prefixes2 = joinStrings(panelPrefixes2, "|");
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
	const char* value = std::getenv(name.c_str());
	if (value == NULL) {
		throw "Not Found";
	}
	return value;
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

bool PartsCatalog::classifyParts(LDLMainModel *mainModel) {
	LDLFileLineArray* fileLines = mainModel->getFileLines();
	for (size_t i = 0; i < fileLines->getCount(); ++i) {
		LDLFileLine* fileLine = (*fileLines)[i];
		if (fileLine->getLineType() == LDLLineType::LDLLineTypeModel) {
			if (!classifyPart(((LDLModelLine*)fileLine)->getModel())) {
				return false;
			}
		}
	}
	return true;
}

bool PartsCatalog::isBaseplate(LDLModel *model, const char* category) {
	if (strcasecmp(category, "baseplate") == 0) {
		return true;
	}
	std::string name = lowerCaseString(model->getName());
	removeExtenstion(name);
	return largeParts.contains(name);
}

bool PartsCatalog::isPanel(LDLModel *model) {
	std::string name = lowerCaseString(model->getName());
	std::smatch match;
	if (std::regex_match(name, match, panelRegex1) || std::regex_match(name, match, panelRegex1)) {
		return true;
	}
	return false;
}

bool PartsCatalog::classifyPart(LDLModel *model) {
	const char* category = model->getCategory();
	if (category == NULL) {
		std::cerr << "Part without category: " << model->getName() << "\n";
		return true;
	}
	if (isBaseplate(model, category)) {
		baseplateNames.push_back(model->getName());
	} else if (isPanel(model)) {
		panelNames.push_back(model->getName());
	} else {
		partNames.push_back(model->getName());
	}
//	std::cout << model->getName() << ":" << model->getCategory() << ":" << model->getDescription() << "\n";
	return true;
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
