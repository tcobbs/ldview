#include "TCUnzip.h"
#include "mystring.h"

#ifdef WIN32
#include <io.h>
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
	}
}

#ifdef WIN32

void TCUnzip::timetToFileTime(time_t t, LPFILETIME pft)
{
	ULARGE_INTEGER time_value;
	time_value.QuadPart = (t * 10000000LL) + 116444736000000000LL;
	pft->dwLowDateTime = time_value.LowPart;
	pft->dwHighDateTime = time_value.HighPart;
}

bool TCUnzip::setFileDate(HANDLE hFile, const tm_unz& unzTime)
{
	time_t fileTime = convertTime(unzTime);
	FILETIME ft;
	timetToFileTime(fileTime, &ft);
	return SetFileTime(hFile, &ft, &ft, &ft) ? true : false;
}

#endif // WIN32

// Note: static member function
time_t TCUnzip::convertTime(const tm_unz &unzTime)
{
	struct tm tms;
	memset(&tms, 0, sizeof(tms));
	tms.tm_sec = unzTime.tm_sec;
	tms.tm_min = unzTime.tm_min;
	tms.tm_hour = unzTime.tm_hour;
	tms.tm_mday = unzTime.tm_mday;
	tms.tm_mon = unzTime.tm_mon;
	tms.tm_year = unzTime.tm_year - 1900;
	// UTC does not use Daylight Saving Time
	tms.tm_isdst = 0;
#ifdef WIN32
	return _mkgmtime(&tms);
#else // WIN32
	return timegm(&tms);
#endif // !WIN32
}

#ifdef WIN32
bool TCUnzip::setFileDate(FILE* file, const tm_unz& unzTime)
#else // WIN32
bool TCUnzip::setFileDate(FILE* /*file*/, const tm_unz& /*unzTime*/)
#endif // !WIN32
{
#ifdef WIN32
	time_t fileTime = convertTime(unzTime);
	HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(file));
	if (hFile != INVALID_HANDLE_VALUE)
	{
		return setFileDate(hFile, unzTime);
	}
	else
	{
		return false;
	}
#else // WIN32
	return false;
#endif // !WIN32
}

bool TCUnzip::setFileDate(const std::string &path, const tm_unz &unzTime)
{
#ifdef WIN32
	std::wstring wpath;
	utf8towstring(wpath, path);
	DWORD flags = FILE_ATTRIBUTE_NORMAL;
	if (isDirectoryPath(path))
	{
		stripTrailingPathSeparators(&wpath[0]);
		// Windows sucks! Without the following, you cannot open a directory.
		flags = flags | FILE_FLAG_BACKUP_SEMANTICS;
	}
	HANDLE hFile = CreateFileW(wpath.c_str(), FILE_WRITE_ATTRIBUTES, 0, NULL, OPEN_EXISTING, flags, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		bool retValue = setFileDate(hFile, unzTime);
		CloseHandle(hFile);
		return retValue;
	}
	else
	{
		return false;
	}
#else // WIN32
	time_t fileTime = convertTime(unzTime);
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

bool TCUnzip::extractCurrentFile(
	const std::string &path,
	const unz_file_info &info,
	const char *outputDir,
	StringTimeMap &dirs)
{
	bool retValue = true;
	std::string outPath;
	
	combinePath(outputDir, path.c_str(), outPath);
	// NOTE: LDraw zip files no longer contain entries for the directories, only
	// for the files. To see a zip file that contains a directory, see here:
	// http://library.ldraw.org/library/updates/lcad0002.zip
	if (isDirectoryPath(path))
	{
		retValue = ensurePath(outPath);
		if (retValue)
		{
			dirs[outPath] = info.tmu_date;
		}
	}
	else if (unzOpenCurrentFile(m_unzFile) == UNZ_OK)
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
#ifdef WIN32
			setFileDate(file, info.tmu_date);
#endif // WIN32
			fclose(file);
		}
		unzCloseCurrentFile(m_unzFile);
		if (retValue)
		{
#ifndef WIN32
			setFileDate(outPath, info.tmu_date);
#endif // !WIN32
		}
	}
	return retValue;
}

int TCUnzip::unzipMinizip(
	const char *filename,
	const char *outputDir /*= NULL*/)
{
	if (!open(filename))
	{
		return -1;
	}
	if (unzGoToFirstFile(m_unzFile) != UNZ_OK)
	{
		return -1;
	}
	int retValue = 0;
	StringTimeMap dirs;
	while (true)
	{
		unz_file_info info;
		std::string subFilename;
		subFilename.resize(2048);
		if (unzGetCurrentFileInfo(m_unzFile, &info, &subFilename[0], subFilename.size(), NULL, 0, NULL, 0) != UNZ_OK)
		{
			retValue = -1;
			continue;
		}
		subFilename.resize(strlen(subFilename.c_str()));
		if (!extractCurrentFile(subFilename, info, outputDir, dirs))
		{
			retValue = -1;
			continue;;
		}
		int nextResult = unzGoToNextFile(m_unzFile);
		if (nextResult == UNZ_END_OF_LIST_OF_FILE)
		{
			break;
		}
		if (nextResult != UNZ_OK)
		{
			retValue = -1;
			break;
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
	close();
	return retValue;
}

#endif // HAVE_MINIZIP
