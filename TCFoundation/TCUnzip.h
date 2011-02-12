#ifndef __TCUNZIP_H__
#define __TCUNZIP_H__

#include <TCFoundation/TCObject.h>

#ifdef HAVE_MINIZIP
#include <minizip/unzip.h>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <map>
#endif // HAVE_MINIZIP

#ifdef COCOA
#define UNZIP_CMD
#endif // COCOA

#if defined (_QT) || defined (__APPLE__) || defined(_OSMESA)
#include <stdlib.h>
#endif // _QT || __APPLE__ || _OSMESA

class TCUnzip : public TCObject
{
public:
	TCUnzip(void);
	int unzip(const char *filename, const char *outputDir = NULL);
	static bool supported(void);
#ifdef HAVE_MINIZIP
	typedef std::vector<std::string> StringVector;

	bool open(const char *filename);
	void close();
	bool getPaths(StringVector &paths);
#endif // HAVE_MINIZIP
protected:
	~TCUnzip(void);
	virtual void dealloc(void);
#ifdef WIN32
	int unzipWin32(const char *filename, const char *outputDir);
#else // WIN32
#ifdef UNZIP_CMD
	int unzipExec(const char *filename, const char *outputDir);
	void unzipChildExec(const char *filename, const char *outputDir);
#endif // UNZIP_CMD
#endif // WIN32
	
#ifdef HAVE_MINIZIP
	struct Entry
	{
		uLong offset;
		unz_file_info fileInfo;
	};
	typedef std::map<std::string, Entry> EntryMap;
	typedef std::list<std::string> StringList;
	typedef std::map<std::string, StringList> StringStringListMap;
	typedef std::map<std::string, tm_unz> StringTimeMap;

	unzFile m_unzFile;
	EntryMap m_entryMap;
	StringStringListMap m_pathsMap;
	bool m_scanned;

	bool scan(void);
	int unzipMinizip(const char *filename, const char *outputDir = NULL);
	bool extractFile(const std::string &path, Entry &entry,
		const char *outputDir, StringTimeMap &dirs);
	bool setFileDate(const std::string &path, const tm_unz &unzTime);
#endif // HAVE_MINIZIP
};

#endif // __TCUNZIP_H__
