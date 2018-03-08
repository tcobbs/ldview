//
//  TransLoader.cpp
//  MacTrans
//
//  Created by Travis Cobbs on 3/7/18.
//  Copyright © 2018 Travis Cobbs. All rights reserved.
//

#include "TransLoader.h"
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/mystring.h>
#include <iostream>
#include <stdio.h>
#include <sstream>

TransLoader::TransLoader(std::string const& filename)
	: m_filename(filename)
{
}

bool TransLoader::load()
{
	FILE *transFile = fopen(m_filename.c_str(), "rb");
	if (transFile == NULL)
	{
		return false;
	}
	fseek(transFile, 0, SEEK_END);
	long fileSize = ftell(transFile);
	fseek(transFile, 0, SEEK_SET);
	std::vector<TCByte> buffer;
	buffer.resize(fileSize + 1);
	if (fread(&buffer[0], fileSize, 1, transFile) != 1)
	{
		fclose(transFile);
		return false;
	}
	fclose(transFile);
	if (fileSize < 2)
	{
		return false;
	}
	bool bigEndian;
	if (buffer[0] == 0xFE && buffer[1] == 0xFF)
	{
		bigEndian = true;
	}
	else if (buffer[0] == 0xFF && buffer[1] == 0xFE)
	{
		bigEndian = false;
	}
	else
	{
		return false;
	}
	std::wstring wstringData;
	long count = fileSize / 2;
	
	wstringData.reserve(count + 1);
	// Note: skip first 2 bytes, which are the Byte Order Mark.
	for (long i = 2; i < fileSize; i += 2)
	{
		int uByte;
		int lByte;
		
		if (bigEndian)
		{
			uByte = buffer[i];
			lByte = buffer[i + 1];
		}
		else
		{
			uByte = buffer[i + 1];
			lByte = buffer[i];
		}
		wchar_t wc = (wchar_t)((uByte << 8) | lByte);
		wstringData.append(&wc, 1);
	}
	std::wistringstream wss;
	wss.str(wstringData);
	std::wstring line;
	std::vector<std::wstring> lines;
	while (std::getline(wss, line))
	{
		line.erase(line.find_last_not_of(L"\r\n") + 1);
		lines.push_back(line);
	}
	for (size_t i = 1; i + 1 < lines.size(); ++i)
	{
		std::wstring const& line1 = lines[i];
		std::wstring const& line2 = lines[i + 1];
		if ((line1.substr(0, 3) == L"DO:" && line2.substr(0, 3) == L"DT:") ||
			(line1.substr(0, 3) == L"MO:" && line2.substr(0, 3) == L"MT:"))
		{
			size_t colonSpot1 = line1.find(L":", 3);
			size_t colonSpot2 = line2.find(L":", 3);
			if (colonSpot1 < line1.size() && colonSpot2 < line2.size())
			{
				std::wstring key = line1.substr(colonSpot1 + 1);
				std::wstring value = line2.substr(colonSpot2 + 1);
				char *keyUtf8 = ucstringtoutf8(key.c_str(), (int)key.size());
				char *valueUtf8 = ucstringtoutf8(value.c_str(), (int)value.size());
				m_transMap[keyUtf8] = valueUtf8;
				delete[] keyUtf8;
				delete[] valueUtf8;
			}
		}
	}
	return true;
}

bool TransLoader::findString(std::string const& src, std::string& dst) const
{
	auto it = m_transMap.find(src);
	if (it == m_transMap.end())
	{
		return false;
	}
	dst = it->second;
	return true;
}
