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

std::map<std::string, TCUnzipStream::ZipDirectory> TCUnzipStream::sm_dirMaps;

bool TCUnzipStream::load(const std::string& zipFilename, unzFile zipFile, const std::string& filename)
{
	if (zipFile == NULL)
	{
		return false;
	}
	if (sm_dirMaps.empty())
	{
		return false;
	}
	std::string lZipFilename = lowerCaseString(zipFilename);
	const ZipDirectories::const_iterator dirIt = sm_dirMaps.find(lZipFilename);
	if (dirIt == sm_dirMaps.end())
	{
		return false;
	}
	const ZipDirectory& zipDir = dirIt->second;
	std::string lfilename = lowerCaseString(filename);
	ZipDirectory::const_iterator fileIt = zipDir.find(lfilename);
	if (fileIt == zipDir.end())
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

unzFile TCUnzipStream::open(const std::string& zipFilename)
{
	unzFile zipFile = unzOpen(zipFilename.c_str());
	if (zipFile != NULL)
	{
		std::string dirZip = lowerCaseString(zipFilename);
		if (sm_dirMaps.find(dirZip) == sm_dirMaps.end())
		{
			// It turns out that every call to unzLocateFile simply walks
			// through the list of all files in the zip looking for the one you
			// asked for. Given that the parts library has thousands of files,
			// this is VERY SLOW, especially since all failed lookups (of which
			// there are many, due to LDraw library path search order) will end
			// up checking every single file in the zip. This code loops through
			// all the files in the zip one time and then stores them in a map
			// keyed off of the filename.
			ZipDirectory& zipDir = sm_dirMaps[dirZip];
			if (unzGoToFirstFile(zipFile) != UNZ_OK)
			{
				unzClose(zipFile);
				return NULL;
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
					return NULL;
				}
				subFilename.resize(strlen(subFilename.c_str()));
				convertStringToLower(subFilename);
				if (unzGetFilePos64(zipFile, &filePos) != UNZ_OK)
				{
					unzClose(zipFile);
					return NULL;
				}
				zipDir[subFilename] = filePos;
				int nextResult = unzGoToNextFile(zipFile);
				if (nextResult == UNZ_END_OF_LIST_OF_FILE)
				{
					return zipFile;
				}
				if (nextResult != UNZ_OK)
				{
					unzClose(zipFile);
					return NULL;
				}
			}
		}
	}
	return zipFile;
}

#endif // HAVE_MINIZIP

bool TCUnzipStream::is_valid(void)
{
	return m_isValid;
}
