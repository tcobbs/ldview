//
//  TCUnzipStream.h
//  TCFoundation
//
//  Created by Travis Cobbs on 3/24/24.
//

#ifndef TCUnzipStream_h
#define TCUnzipStream_h

#include <sstream>

#ifdef HAVE_MINIZIP
#include <minizip/unzip.h>
#include <map>
#ifdef USE_CPP11
#include <unordered_map>
#endif // USE_CPP11
#include <string>
#endif // HAVE_MINIZIP

class TCUnzipStream: public std::istringstream
{
public:
	TCUnzipStream(void);
	~TCUnzipStream(void);
#ifdef HAVE_MINIZIP
	bool load(const std::string& zipFilename, unzFile zipFile, const std::string& filename);
	static unzFile open(const std::string& zipFilename);
#endif // HAVE_MINIZIP
	bool is_valid(void);
private:
	bool m_isValid;
#ifdef HAVE_MINIZIP
#ifdef USE_CPP11
	// This map will have thousands of entries, so we want it to be as fast as
	// possible.
	typedef std::unordered_map<std::string, unz64_file_pos> ZipDirectory;
#else // USE_CPP11
	typedef std::map<std::string, unz64_file_pos> ZipDirectory;
#endif // USE_CPP11
	typedef std::map<std::string, ZipDirectory> ZipDirectories;
	static ZipDirectories sm_dirMaps;
#endif // HAVE_MINIZIP
};

#endif // TCUnzipStream_h
