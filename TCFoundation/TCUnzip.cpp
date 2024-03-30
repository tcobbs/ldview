#include "TCUnzip.h"
#include "mystring.h"

#ifdef WIN32

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

#else // WIN32
#include <sys/time.h>
#ifdef UNZIP_CMD
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#endif // UNZIP_CMD
#endif // WIN32

#define BUFFER_SIZE 4096

TCUnzip::TCUnzip(void)
#ifdef HAVE_MINIZIP
	: m_unzFile(NULL)
	, m_scanned(false)
#endif // HAVE_MINIZIP
{
}

TCUnzip::~TCUnzip(void)
{
#ifdef HAVE_MINIZIP
	close();
#endif // HAVE_MINIZIP
}

void TCUnzip::dealloc(void)
{
	TCObject::dealloc();
}

int TCUnzip::unzip(const char *filename, const char *outputDir)
{
#ifdef HAVE_MINIZIP
	return unzipMinizip(filename, outputDir);
#endif // HAVE_MINIZIP
#ifdef UNZIP_CMD
	return unzipExec(filename, outputDir);
#else
	return -1;
#endif // UNZIP_CMD
}

#ifdef UNZIP_CMD

void TCUnzip::unzipChildExec(const char *filename, const char *outputDir)
{
	char *unzipPath = findExecutable("unzip");

	if (unzipPath)
	{
		char *zipFile = copyString(filename);

		if (chdir(outputDir) == 0)
		{
			char dashO[] = "-o";
			char *const argv[] = {unzipPath, dashO, zipFile, NULL};

			// Squash the console output from the unzip program.
			if (freopen("/dev/null", "w", stdout) == NULL)
			{
				debugPrintf("Error redirecting stdout to /dev/null.\n");
			}
			execv(unzipPath, argv);
		}
		delete[] zipFile;
		delete[] unzipPath;
	}
}

int TCUnzip::unzipExec(const char *filename, const char *outputDir)
{
	pid_t forkResult = fork();
	switch (forkResult)
	{
	case 0:
		// child process
		unzipChildExec(filename, outputDir);
		// If we get here at all, there was an error executing unzip.  Since
		// we've already forked, we have to exit with an error status.
		exit(1);
		break;
	case -1:
		// error
		return -1;
		break;
	default:
		// Parent process
		int status;
		wait(&status);
		if (status == 0)
		{
			return 0;
		}
		else
		{
			return -1;
		}
		break;
	}
	// We'll never get to the following, but we get a warning otherwise.
	return -1;
}

#endif // UNZIP_CMD

bool TCUnzip::supported(void)
{
#if defined(UNZIP_CMD) || defined(HAVE_MINIZIP)
	return true;
#else // UNZIP_CMD || HAVE_MINIZIP
	return false;
#endif // UNZIP_CMD || HAVE_MINIZIP
}


#ifdef HAVE_MINIZIP

bool TCUnzip::open(const char *filename)
{
	close();
	m_unzFile = unzOpen(filename);
	return m_unzFile != NULL;
}

void TCUnzip::close(void)
{
	if (m_unzFile != NULL)
	{
		unzClose(m_unzFile);
		m_unzFile = NULL;
		if (m_scanned)
		{
			m_pathsMap.clear();
			m_entryMap.clear();
			m_scanned = false;
		}
	}
}

bool TCUnzip::scan(void)
{
	if (!m_scanned && m_unzFile != NULL)
	{
		if (unzGoToFirstFile(m_unzFile) == UNZ_OK)
		{
			do
			{
				char path[2048];
				Entry entry;

				entry.offset = unzGetOffset(m_unzFile);
				if (unzGetCurrentFileInfo(m_unzFile, &entry.fileInfo, path,
					sizeof(path), NULL, 0, NULL, 0) == UNZ_OK)
				{
					std::string pathString(path);
					std::string lowerPath = lowerCaseString(pathString);
					m_pathsMap[lowerPath].push_back(pathString);
					m_entryMap[pathString] = entry;
				}
			} while (unzGoToNextFile(m_unzFile) == UNZ_OK);
		}
		m_scanned = true;
	}
	return m_unzFile != NULL;
}

bool TCUnzip::getPaths(StringVector &paths)
{
	if (scan())
	{
		paths.clear();
		paths.reserve(m_pathsMap.size());
		for (StringStringListMap::const_iterator it = m_pathsMap.begin();
			it != m_pathsMap.end(); ++it)
		{
			const StringList &pathList = it->second;
			for (StringList::const_iterator itList = pathList.begin();
				itList != pathList.end(); ++itList)
			{
				paths.push_back(*itList);
			}
		}
		return true;
	}
	return false;
}

bool TCUnzip::setFileDate(const std::string &path, const tm_unz &unzTime)
{
#ifdef WIN32
	return false;
#else // WIN32
	time_t fileTime = 0;
	struct tm fileTm;
	// Use gmtime to initialize fileTm structure to sane values.
	//gmtime_r(&fileTime, &fileTm);
	memset(&fileTm, 0, sizeof(fileTm));
	fileTm.tm_sec = unzTime.tm_sec;
	fileTm.tm_min = unzTime.tm_min;
	fileTm.tm_hour = unzTime.tm_hour;
	fileTm.tm_mday = unzTime.tm_mday;
	fileTm.tm_mon = unzTime.tm_mon;
	fileTm.tm_year = unzTime.tm_year - 1900;
	fileTm.tm_isdst = -1;	// Automatic
	fileTime = timegm(&fileTm);
	if (fileTime >= 0)
	{
		struct timeval times[2];
		times[0].tv_sec = fileTime;
		times[0].tv_usec = 0;
		times[1].tv_sec = fileTime;
		times[1].tv_usec = 0;
		utimes(path.c_str(), times);
		return true;
	}
	else
	{
		return false;
	}
#endif // !WIN32
}

bool TCUnzip::extractFile(
	const std::string &path,
	Entry &entry,
	const char *outputDir,
	StringTimeMap &dirs)
{
	bool retValue = true;
	std::string outPath;
	
	combinePath(outputDir, path.c_str(), outPath);
	if (isDirectoryPath(path))
	{
		retValue = ensurePath(outPath);
		if (retValue)
		{
			dirs[outPath] = entry.fileInfo.tmu_date;
		}
	}
	else if (unzSetOffset(m_unzFile, entry.offset) == UNZ_OK &&
		unzOpenCurrentFile(m_unzFile) == UNZ_OK)
	{
		char *dir = directoryFromPath(outPath.c_str());
		// Note: some (most?) directories have their own entry in the zip.
		// However, for some reason, some directories don't have an entry, so
		// we have to make sure that the directory exists before extracting any
		// files into it.
		if (!ensurePath(dir))
		{
			retValue = false;
		}
		FILE *file = NULL;
		if (retValue)
		{
			file = ucfopen(outPath.c_str(), "wb");
		}
		if (file != NULL)
		{
			TCByte buf[BUFFER_SIZE];
			int count;

			retValue = true;
			while ((count = unzReadCurrentFile(m_unzFile, buf, BUFFER_SIZE)) >
				0)
			{
				if (fwrite(buf, count, 1, file) != 1)
				{
					retValue = false;
					break;
				}
			}
			fclose(file);
		}
		unzCloseCurrentFile(m_unzFile);
		if (retValue)
		{
			setFileDate(outPath, entry.fileInfo.tmu_date);
		}
	}
	return retValue;
}

int TCUnzip::unzipMinizip(
	const char *filename,
	const char *outputDir /*= NULL*/)
{
	int retValue = -1;
	if (!open(filename))
	{
		return -1;
	}
	if (scan())
	{
		retValue = 0;
		StringTimeMap dirs;
		for (EntryMap::iterator it = m_entryMap.begin();
			it != m_entryMap.end(); ++it)
		{
			if (!extractFile(it->first, it->second, outputDir, dirs))
			{
				retValue = -1;
			}
		}
		// Process dirs in reverse order in order to get the deepest ones first.
		// The whole reason for doing the dir times separetly is because every
		// time a file is written inside a dir, its time updates.
#ifdef COCOA
		// There seems to be a bug in the COCOA cross-compiler.
		for (StringTimeMap::reverse_iterator it = dirs.rbegin();
#else // COCOA
		for (StringTimeMap::const_reverse_iterator it = dirs.rbegin();
#endif // !COCOA
			it != dirs.rend(); ++it)
		{
			setFileDate(it->first, it->second);
		}
	}
	close();
	return retValue;
}

#endif // HAVE_MINIZIP
