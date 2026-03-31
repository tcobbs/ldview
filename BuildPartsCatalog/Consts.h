//
//  LargeConsts.hpp
//  BuildPartsCatalog
//
//  Created by Travis Cobbs on 9/6/25.
//

#ifndef LargeConsts_h
#define LargeConsts_h

#include <string>
#include <vector>
#include <set>

typedef std::vector<std::string> StringVector;
typedef std::set<std::string> StringSet;

struct Consts {
	static const StringSet largeParts;
	static const StringVector panelPrefixes1;
	static const StringVector panelPrefixes2;
	static const std::string styleSheet;
	static const std::string htmlHeader;
	static const std::string mainPrefix;
	static const std::string htmlSuffix;
	static const std::string categoryRow;
	static const std::string catPrefix;
	static const std::string catSuffix;
	static const std::string partRow;
	static const std::string keywordsRow;
};

#endif /* LargeConsts_h */
