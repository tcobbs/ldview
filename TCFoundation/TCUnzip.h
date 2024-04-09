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
	bool open(const char *filename);
	void close();
#endif // HAVE_MINIZIP
protected:
	~TCUnzip(void);
	virtual void dealloc(void);
#ifdef UNZIP_CMD
	int unzipExec(const char *filename, const char *outputDir);
	void unzipChildExec(const char *filename, const char *outputDir);
#endif // UNZIP_CMD
	
#ifdef HAVE_MINIZIP
	typedef std::map<std::string, tm_unz> StringTimeMap;

	unzFile m_unzFile;

	int unzipMinizip(const char *filename, const char *outputDir = NULL);
	bool extractCurrentFile(const std::string &path, const unz_file_info &info,
		const char *outputDir, StringTimeMap &dirs);
#ifdef WIN32
	void timetToFileTime(time_t t, LPFILETIME pft);
#endif // WIN32
	time_t convertTime(const tm_unz& unzTime);
	bool setFileDate(const std::string &path, const tm_unz &unzTime);
#endif // HAVE_MINIZIP
};

#endif // __TCUNZIP_H__
