//
//  XliffUpdater.cpp
//  MacTrans
//
//  Created by Travis Cobbs on 3/7/18.
//  Copyright © 2018 Travis Cobbs. All rights reserved.
//

#include "XliffUpdater.h"
#include <TCFoundation/mystring.h>

XliffUpdater::XliffUpdater(
TransLoader const &transLoader,
std::string const& srcFilename,
std::string const& dstFilename)
	: m_transLoader(transLoader)
	, m_srcFilename(srcFilename)
	, m_dstFilename(dstFilename)
	, m_xmlDoc(srcFilename)
{
	char *dstBaseName = filenameFromPath(m_dstFilename.c_str());
	char *dot = strchr(dstBaseName, '.');
	if (dot)
	{
		*dot = 0;
	}
	m_dstLanguageCode = dstBaseName;
	delete[] dstBaseName;
}

bool XliffUpdater::update()
{
	if (!m_xmlDoc.LoadFile())
	{
		return false;
	}
	TiXmlHandle hDoc(&m_xmlDoc);
	TiXmlElement *root = hDoc.FirstChildElement("xliff").Element();
	
	if (root == nullptr)
	{
		return false;
	}
	TiXmlElement *fileEl;
	for (fileEl = root->FirstChildElement("file"); fileEl != nullptr;
		 fileEl = fileEl->NextSiblingElement("file"))
	{
		if (!updateFileEl(fileEl))
		{
			return false;
		}
	}
	return m_xmlDoc.SaveFile(m_dstFilename);
}

bool XliffUpdater::updateFileEl(TiXmlElement *fileEl)
{
	TiXmlElement *bodyEl = fileEl->FirstChildElement("body");
	if (bodyEl == nullptr)
	{
		return false;
	}
	fileEl->SetAttribute("target-language", m_dstLanguageCode.c_str());
	TiXmlElement *unitEl;
	for (unitEl = bodyEl->FirstChildElement("trans-unit"); unitEl != nullptr;
		 unitEl = unitEl->NextSiblingElement("trans-unit"))
	{
		TiXmlElement *sourceEl = unitEl->FirstChildElement("source");
		if (sourceEl != nullptr)
		{
			const char *sourceText = sourceEl->GetText();
			std::string targetText;
			if (!m_transLoader.findString(sourceText, targetText))
			{
				std::cout << "No translation found for <<" << sourceText << ">>\n";
				continue;
			}
			TiXmlElement targetEl("target");
			TiXmlText text(targetText);
			targetEl.InsertEndChild(text);
			TiXmlElement *oldTargetEl = unitEl->FirstChildElement("target");
			if (oldTargetEl != nullptr)
			{
				if (unitEl->ReplaceChild(oldTargetEl, targetEl) == nullptr)
				{
					std::cout << "Error replacing target element.";
				}
			}
			else
			{
				if (unitEl->InsertAfterChild(sourceEl, targetEl) == nullptr)
				{
					std::cout << "Error adding target element.";
				}
			}
		}
	}
	return true;
}
