//
//  TransLoader.h
//  MacTrans
//
//  Created by Travis Cobbs on 3/7/18.
//  Copyright © 2018 Travis Cobbs. All rights reserved.
//

#ifndef TransLoader_h
#define TransLoader_h

#include <string>
#include <vector>
#include <map>
#include <fstream>

class TransLoader
{
public:
	TransLoader(std::string const& filename);
	bool load();
	bool findString(std::string const& src, std::string& dst) const;
private:
	void fixMenuString(std::wstring& menuString);
	void fixAmpersands(std::wstring& rcString);
	std::string m_filename;
	std::map<std::string, std::string> m_transMap;
};

#endif /* TransLoader_h */
