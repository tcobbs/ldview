//
//  XliffUpdater.h
//  MacTrans
//
//  Created by Travis Cobbs on 3/7/18.
//  Copyright © 2018 Travis Cobbs. All rights reserved.
//

#ifndef XliffUpdater_h
#define XliffUpdater_h

#include <string>
#include <tinyxml/tinyxml.h>
#include "TransLoader.h"

class XliffUpdater
{
public:
	XliffUpdater(TransLoader const &transLoader, std::string const& srcFilename, std::string const& dstFilename);
	bool update();
private:
	bool updateFileEl(TiXmlElement *fileEl);
	TransLoader m_transLoader;
	std::string m_srcFilename;
	std::string m_dstFilename;
	std::string m_dstLanguageCode;
	TiXmlDocument m_xmlDoc;
};

#endif /* XliffUpdater_h */
