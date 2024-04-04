//
//  TCUnzipStream.cpp
//  TCFoundation
//
//  Created by Travis Cobbs on 3/24/24.
//

#include "TCUnzipStream.h"
#include <TCFoundation/mystring.h>

TCUnzipStream::TCUnzipStream(void)
	: m_isValid(false)
{
}

TCUnzipStream::~TCUnzipStream(void)
{
}

#ifdef HAVE_MINIZIP

std::map<std::string, TCUnzipStream::ZipIndex> TCUnzipStream::sm_zipIndices;

void TCUnzipStream::deindex(const std::string& zipFilename)
{
	std::string lfilename = lowerCaseString(zipFilename);
	sm_zipIndices.erase(lfilename);
}

bool TCUnzipStream::load(const std::string& zipFilename, unzFile zipFile, const std::string& filename)
{
	if (zipFile == NULL)
	{
		return false;
	}
	if (sm_zipIndices.empty())
	{
		return false;
	}
	std::string lZipFilename = lowerCaseString(zipFilename);
	const ZipIndices::const_iterator zipIt = sm_zipIndices.find(lZipFilename);
	if (zipIt == sm_zipIndices.end())
	{
		return false;
	}
	const ZipIndex& zipIndex = zipIt->second;
	std::string lfilename = lowerCaseString(filename);
	ZipIndex::const_iterator fileIt = zipIndex.find(lfilename);
	if (fileIt == zipIndex.end())
	{
		return false;
	}
	if (unzGoToFilePos64(zipFile, &(fileIt->second)) != UNZ_OK)
	{
		return false;
	}
	if (unzOpenCurrentFile(zipFile) != UNZ_OK)
	{
		return false;
	}
	unz_file_info info;
	if (unzGetCurrentFileInfo(zipFile, &info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK)
	{
		return false;
	}
	std::string buf;
	buf.resize(info.uncompressed_size);
	if (unzReadCurrentFile(zipFile, &buf[0], (unsigned)info.uncompressed_size) != (int)buf.size())
	{
		return false;
	}
	unzCloseCurrentFile(zipFile);
	// TODO: See if there is a way to do this without having to copy the entire
	// contents of the file from one string to another.
	str(buf);
	m_isValid = true;
	return true;
}

bool TCUnzipStream::index(unzFile zipFile, ZipIndex& zipIndex)
{
	// It turns out that every call to unzLocateFile simply walks
	// through the list of all files in the zip looking for the one you
	// asked for. Given that the parts library has thousands of files,
	// this is VERY SLOW, especially since all failed lookups (of which
	// there are many, due to LDraw library path search order) will end
	// up checking every single file in the zip. This code loops through
	// all the files in the zip one time and then stores them in a map
	// keyed off of the filename.
	if (unzGoToFirstFile(zipFile) != UNZ_OK)
	{
		unzClose(zipFile);
		return false;
	}
	while (true)
	{
		unz64_file_pos filePos;
		unz_file_info info;
		std::string subFilename;
		subFilename.resize(1024);
		if (unzGetCurrentFileInfo(zipFile, &info, &subFilename[0], subFilename.size(), NULL, 0, NULL, 0) != UNZ_OK)
		{
			unzClose(zipFile);
			return false;
		}
		subFilename.resize(strlen(subFilename.c_str()));
		convertStringToLower(subFilename);
		if (unzGetFilePos64(zipFile, &filePos) != UNZ_OK)
		{
			unzClose(zipFile);
			return false;
		}
		zipIndex[subFilename] = filePos;
		int nextResult = unzGoToNextFile(zipFile);
		if (nextResult == UNZ_END_OF_LIST_OF_FILE)
		{
			return true;
		}
		if (nextResult != UNZ_OK)
		{
			unzClose(zipFile);
			return false;
		}
	}
}

const TCUnzipStream::ZipIndex& TCUnzipStream::findIndex(const std::string& zipFilename)
{
	std::string lzipFilename = lowerCaseString(zipFilename);
	ZipIndices::const_iterator it = sm_zipIndices.find(lzipFilename);
	if (it == sm_zipIndices.end())
	{
		throw 0;
	}
	return it->second;
}

// Note: The reason TCUnzipStream doesn't have the unzFile as a member variable
// is that each instance of TCUnzipStream is designed to represent a single file
// inside the zip file. So the load function takes the unzFile as its first
// argument.
unzFile TCUnzipStream::open(const std::string& zipFilename)
{
	unzFile zipFile = unzOpen(zipFilename.c_str());
	if (zipFile != NULL)
	{
		std::string lzipFilename = lowerCaseString(zipFilename);
		if (sm_zipIndices.find(lzipFilename) == sm_zipIndices.end())
		{
			if (!index(zipFile, sm_zipIndices[lzipFilename]))
			{
				return NULL;
			}
		}
	}
	return zipFile;
}

bool TCUnzipStream::close(unzFile zipFile)
{
	if (zipFile == NULL)
	{
		return false;
	}
	unzClose(zipFile);
	return true;
}

#endif // HAVE_MINIZIP

bool TCUnzipStream::is_valid(void)
{
	return m_isValid;
}
