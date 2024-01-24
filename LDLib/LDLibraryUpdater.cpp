#if defined(USE_CPP11) || !defined(_NO_BOOST)

#include "LDLibraryUpdater.h"
#include "LDrawModelViewer.h"

#ifndef USE_CPP11
// One of the include files triggerred below the boost ones causes warnings to
// show up during the parsing of the boost ones if these are moved down.  Please
// don't move them down.

#ifdef WIN32
#pragma warning(push, 3)
#endif // WIN32

#include <boost/version.hpp>
#include <boost/bind.hpp>

#ifdef WIN32
#pragma warning(pop)
#endif // WIN32

#endif // !USE_CPP11

#include "LDLibraryUpdateInfo.h"
#include <LDLoader/LDLModel.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCWebClient.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCUnzip.h>
#include <TCFoundation/TCLocalStrings.h>
#include <fstream>

#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif // !WIN32

#if defined(_QT) || defined(_OSMESA)
#include <ctype.h>
#endif // _QT || _OSMESA

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

#define MAX_DL_THREADS 2

LDLibraryUpdater::LDLibraryUpdater(void)
	:m_webClients(new TCWebClientArray),
	m_finishedWebClients(new TCWebClientArray),
//	m_thread(NULL),
	m_thread(NULL),
#ifdef USE_CPP11
	m_mutex(new std::mutex),
	m_threadFinish(new std::condition_variable),
#else
	m_mutex(new boost::mutex),
	m_threadFinish(new boost::condition),
#endif
	m_libraryUpdateKey(NULL),
	m_ldrawDir(NULL),
	m_ldrawDirParent(NULL),
	m_updateQueue(NULL),
	m_updateUrlList(NULL),
	m_downloadList(NULL),
	m_initialQueueSize(0),
	m_aborting(false),
	m_install(false),
	m_libraryUpdater(NULL)
{
	m_error[0] = 0;
}

LDLibraryUpdater::~LDLibraryUpdater(void)
{
}

void LDLibraryUpdater::dealloc(void)
{
	TCObject::release(m_webClients);
	TCObject::release(m_finishedWebClients);
//	TCObject::release(m_thread);
	delete m_threadFinish;
	delete m_mutex;
	if (m_thread)
	{
		try
		{
			m_thread->join();
		}
		catch (...)
		{
			// Ignore
		}
		delete m_thread;
	}
	delete[] m_libraryUpdateKey;
	delete[] m_ldrawDir;
	delete[] m_ldrawDirParent;
	TCObject::release(m_updateQueue);
	TCObject::release(m_updateUrlList);
	TCObject::release(m_downloadList);
	TCObject::dealloc();
}

void LDLibraryUpdater::setLibraryUpdateKey(const char *libraryUpdateKey)
{
	m_libraryUpdateKey = copyString(libraryUpdateKey);
}

void LDLibraryUpdater::setLdrawDir(const char *ldrawDir)
{
	if (ldrawDir != m_ldrawDir)
	{
		char *trimSpot;
		char *slashSpot;

		delete[] m_ldrawDir;
		m_ldrawDir = copyString(ldrawDir);
		delete[] m_ldrawDirParent;
		m_ldrawDirParent = copyString(ldrawDir);
		stripTrailingPathSeparators(m_ldrawDirParent);
		trimSpot = strrchr(m_ldrawDirParent, '\\');
		slashSpot = strrchr(m_ldrawDirParent, '/');
		if (slashSpot)
		{
			if (!trimSpot || slashSpot > trimSpot)
			{
				trimSpot = slashSpot;
			}
		}
		if (trimSpot)
		{
			trimSpot[1] = 0;
		}
	}
}

int LDLibraryUpdater::compareUpdates(LDLibraryUpdateInfoArray *updateArray,
									 const char *left, const char *right)
{
	size_t i;
	size_t count = updateArray->getCount();
	ptrdiff_t leftIndex = -1;
	ptrdiff_t rightIndex = -1;

	for (i = 0; i < count && (leftIndex == -1 || rightIndex == -1); i++)
	{
		const char *updateName = (*updateArray)[i]->getName();

		if (leftIndex == -1 && strcasecmp(updateName, left) == 0)
		{
			leftIndex = i;
		}
		if (rightIndex == -1 && strcasecmp(updateName, right) == 0)
		{
			rightIndex = i;
		}
	}
	if (leftIndex == -1 || rightIndex == -1)
	{
		int leftUpdateNumber = getUpdateNumber(left);
		int rightUpdateNumber = getUpdateNumber(right);

		if (leftUpdateNumber < rightUpdateNumber)
		{
			return -1;
		}
		else if (leftUpdateNumber > rightUpdateNumber)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (leftIndex < rightIndex)
	{
		return -1;
	}
	else if (leftIndex > rightIndex)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int LDLibraryUpdater::getUpdateNumber(const char *updateName)
{
	if (strlen(updateName) < 8)
	{
		return 0;
	}
	else
	{
		int updateNumber = atoi(&updateName[4]);

		if (updateNumber > 9700 && updateNumber <= 9999)
		{
			return updateNumber + 190000;
		}
		else
		{
			return updateNumber + 200000;
		}
		return updateNumber;
	}
}

void LDLibraryUpdater::scanDir(const std::string &dir, StringList &dirList)
{
	std::string path = m_ldrawDir;

	path += "/";
#ifdef WIN32
	path += dir + "/";
	std::string findString = path + "*.dat";
	WIN32_FIND_DATA ffd;
	HANDLE hFind;

	hFind = FindFirstFile(findString.c_str(), &ffd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		dirList.push_back(path + ffd.cFileName);
		while (FindNextFile(hFind, &ffd))
		{
			dirList.push_back(path + ffd.cFileName);
		}
		FindClose(hFind);
	}
#else // WIN32
	bool found = false;
	DIR *pDir = opendir(path.c_str());
	if (pDir != NULL)
	{
		const char *dirName = dir.c_str();

		dirent *de;
		while ((de = readdir(pDir)) && !found)
		{
#ifdef _AIX
			struct stat entry;
			char filename[PATH_MAX];
			strncpy(filename, path.c_str(),PATH_MAX);
			strncat(filename, "/", PATH_MAX);
			strncat(filename, de->d_name,  PATH_MAX);
			lstat(filename,&entry);
			if (S_ISDIR(entry.st_mode) &&
				strcasecmp(de->d_name, dirName) == 0)
#else
			if ((de->d_type & DT_DIR) &&
				strcasecmp(de->d_name, dirName) == 0)
#endif
			{
				path += de->d_name;
				path += "/";
				found = true;
			}
		}
		closedir(pDir);
	}
	if (found)
	{
		pDir = opendir(path.c_str());
		if (pDir != NULL)
		{
			dirent *de;
			while ((de = readdir(pDir)))
			{
#ifdef _AIX
				struct stat entry;
				char filename[PATH_MAX];
				strncpy(filename, path.c_str(),PATH_MAX);
				strncat(filename, "/", PATH_MAX);
				strncat(filename, de->d_name,  PATH_MAX);
				lstat(filename,&entry);
				if (S_ISDIR(entry.st_mode) &&
					stringHasCaseInsensitiveSuffix(de->d_name, ".dat"))
#else
				if ((de->d_type & DT_DIR) == 0 &&
					stringHasCaseInsensitiveSuffix(de->d_name, ".dat"))
#endif
				{
					dirList.push_back(path + de->d_name);
				}
			}
			closedir(pDir);
		}
	}
#endif // !WIN32
}

bool LDLibraryUpdater::findOfficialRelease(
	const std::string &filename,
	char *updateName)
{
	FILE *file;
	bool retValue = false;

	if ((file = ucfopen(filename.c_str(), "rb")) != NULL)
	{
		while (!retValue)
		{
#ifdef USE_CPP11
			char line[1024] = { 0 };
#else // USE_CPP11
			char line[1024];
#endif // USE_CPP11

			if (fgets(line, sizeof(line) - 1, file) == NULL)
			{
				break;
			}
			stripCRLF(line);
			stripLeadingWhitespace(line);
			if (line[0] == '0')
			{
				char ldrawOrg[1024];
				char update[1024];
				char updateNum[1024];

				if (sscanf(line, "0 %s %*s %s %s", ldrawOrg, update, updateNum)
					== 3)
				{
					if (strcmp(ldrawOrg, "!LDRAW_ORG") == 0 &&
						strcmp(update, "UPDATE") == 0)
					{
						if (strcmp(updateNum, updateName) > 0)
						{
							strcpy(updateName, updateNum);
						}
						retValue = true;
					}
				}
			}
			else
			{
				stripTrailingWhitespace(line);
				if (line[0] != 0)
				{
					// Done with header
					break;
				}
			}
		}
		fclose(file);
	}
	return retValue;
}

bool LDLibraryUpdater::findLatestOfficialRelease(
	const StringList &dirList,
	char *updateName,
	const LDLibraryUpdateInfo *lastUpdate,
	bool *aborted)
{
	bool retValue = false;
	int i = 0;
	float size = (float)dirList.size();

	updateName[0] = 0;
	for (StringList::const_iterator it = dirList.begin(); it != dirList.end();
		++it)
	{
		retValue = findOfficialRelease(*it, updateName) || retValue;
		if (++i % 25 == 0)
		{
			if (retValue && lastUpdate != NULL)
			{
				std::string temp = updateName;

				temp = std::string("lcad") + temp.substr(2, 2) + temp.substr(5, 2);
				if (strcasecmp(temp.c_str(), lastUpdate->getName()) == 0)
				{
					// There can't be a file with an update that is after the
					// latest official LDraw library release, so if we find a
					// file from that release, it is by definition the latest
					// official local file, so we're done.
					return true;
				}
			}
			TCProgressAlert::send(LD_LIBRARY_UPDATER,
				ls(_UC("LDLUpdateScanning")), 0.03f + i / size * 0.07f, aborted,
				this);
			if (*aborted)
			{
				return false;
			}
		}
	}
	return retValue;
}

bool LDLibraryUpdater::determineLastUpdate(
	LDLibraryUpdateInfoArray *updateArray,
	char *updateName,
	bool *aborted)
{
	char *lastRecordedUpdate = NULL;
	StringList dirList;

	TCProgressAlert::send(LD_LIBRARY_UPDATER, ls(_UC("LDLUpdateScanning")),
		0.021f, aborted, this);
	if (*aborted)
	{
		return false;
	}
	scanDir("parts", dirList);
	scanDir("p", dirList);
	TCProgressAlert::send(LD_LIBRARY_UPDATER, ls(_UC("LDLUpdateScanning")),
		0.03f, aborted, this);
	if (findLatestOfficialRelease(dirList, updateName,
		updateArray->lastObject(), aborted))
	{
		std::string temp = updateName;

		temp = temp.substr(2, 2) + temp.substr(5, 2);
		strcpy(updateName, "lcad");
		strcat(updateName, temp.c_str());
		return true;
	}
	if (*aborted)
	{
		return false;
	}
	if (m_libraryUpdateKey)
	{
		lastRecordedUpdate =
			TCUserDefaults::stringForKey(m_libraryUpdateKey, NULL, false);
	}
	if (lastRecordedUpdate)
	{
		strcpy(updateName, lastRecordedUpdate);
		delete[] lastRecordedUpdate;
		return true;
	}
	else
	{
		std::string line;
		std::ifstream completeTextStream;
		unsigned int i;
		bool done = false;
		std::string filename = m_ldrawDir;

		updateName[0] = 0;
		filename += "/models/complete.txt";
		// LDLModel::openFile just opens a file, but it supports
		// case-insensitive opening of a file on a case-sensitive file system.
		LDLModel::openFile(filename.c_str(), completeTextStream);
		if (completeTextStream.is_open() && !completeTextStream.fail())
		{
			bool firstUpdateFound = false;

			// First, we'll see what's the latest update in the installed
			// complete package.
			while (1)
			{
				char *lcad;

				if (!std::getline(completeTextStream, line))
				{
					// If we have a real complete.txt file out of a complete
					// update install, we shouldn't get here, but we certainly
					// don't want to assume anything.
					break;
				}
				lcad = strcasestr(&line[0], "lcad");
				if (lcad && isdigit(lcad[4]) && isdigit(lcad[5]) &&
					isdigit(lcad[6]) && isdigit(lcad[7]) &&
					strncmp(lcad + 8, ".exe", 4) == 0)
				{
					char tmpStr[9];

					firstUpdateFound = true;
					strncpy(tmpStr, lcad, 8);
					tmpStr[8] = 0;
					if (compareUpdates(updateArray, updateName, tmpStr) < 0)
					{
						strcpy(updateName, tmpStr);
					}
				}
				else if (firstUpdateFound)
				{
					// All the updates are listed in a row, so if we see a line
					// in the file that doesn't list an update, and we've
					// already seen lines that do list updates, then we're done
					// scanning the file.
					break;
				}
			}
			completeTextStream.close();
		}
		else
		{
			// If we don't find complete.txt, we're going to assume that no
			// complete part updates have been installed.
			return false;
		}
		if (!updateName[0])
		{
			// We found a complete.txt file, but couldn't find any update names
			// in it.  So we'll just have to assume here also that no complete
			// part updates have been installed.
			return false;
		}
		for (i = 0; i < updateArray->getCount() && !done; i++)
		{
			LDLibraryUpdateInfo *updateInfo = (*updateArray)[i];
			const char *updateInfoName = updateInfo->getName();

			if (compareUpdates(updateArray, updateName,
				updateInfoName) < 0)
			{
				char tmpFilename[9];

				strcpy(tmpFilename, updateInfoName);
				tmpFilename[0] = 'n';
				tmpFilename[1] = 'o';
				tmpFilename[2] = 't';
				tmpFilename[3] = 'e';
				filename = m_ldrawDir;
				filename += "/models/";
				filename += tmpFilename;
				filename += ".txt";
				if (fileExists(filename.c_str()))
				{
					strcpy(updateName, updateInfoName);
				}
				else
				{
					done = true;
				}
			}
		}
		return true;
	}
}

bool LDLibraryUpdater::parseUpdateList(const char *updateList, bool *aborted)
{
	size_t lineCount;
	char **updateListLines = componentsSeparatedByString(updateList, "\n",
		lineCount);
	char lastUpdateName[1024];
	char lastExeUpdateName[1024];
	ptrdiff_t i;
	bool fullUpdateNeeded = true;
	LDLibraryUpdateInfoArray *updateArray = new LDLibraryUpdateInfoArray;
	LDLibraryUpdateInfoArray *exeUpdateArray = new LDLibraryUpdateInfoArray;
	LDLibraryUpdateInfo *fullUpdateInfo = NULL;
	LDLibraryUpdateInfo *baseUpdateInfo = NULL;
	bool zipSupported = TCUnzip::supported();
	bool retValue = true;

	try
	{
		for (i = 0; (size_t)i < lineCount; i++)
		{
			LDLibraryUpdateInfo *updateInfo = new LDLibraryUpdateInfo;

			if (updateInfo->parseUpdateLine(updateListLines[i]))
			{
				// We're not going to apply so many updates as to have to go all
				// the way back to the EXE-only updates.  If ZIP is supported,
				// only pay attention to the ZIP updates; if ZIP isn't supported,
				// only pay attention to the EXE updates.
				if ((zipSupported &&
					updateInfo->getFormat() == LDLibraryZipFormat) ||
					(!zipSupported &&
					updateInfo->getFormat() == LDLibraryExeFormat))
				{
					switch (updateInfo->getUpdateType())
					{
					case LDLibraryFullUpdate:
						fullUpdateInfo = updateInfo;
						// We're going to release below, so retain here.
						fullUpdateInfo->retain();
						break;
					case LDLibraryPartialUpdate:
						if (!m_install)
						{
							updateArray->addObject(updateInfo);
						}
						break;
					case LDLibraryBaseUpdate:
						if (m_install)
						{
							baseUpdateInfo = updateInfo;
							baseUpdateInfo->retain();
						}
						break;
					case LDLibraryUnknownUpdate:
						break;
					}
				}
				if (updateInfo->getFormat() == LDLibraryExeFormat &&
					updateInfo->getUpdateType() == LDLibraryPartialUpdate)
				{
					if (!m_install)
					{
						exeUpdateArray->addObject(updateInfo);
					}
				}
			}
			updateInfo->release();
		}
		deleteStringArray(updateListLines, lineCount);
		TCObject::release(m_updateQueue);
		m_updateQueue = NULL;
		if (m_install)
		{
			if (fullUpdateInfo != NULL)
			{
				// Note: Base is gone now, but on the off chance that it comes
				// back (which I can't really imagine), and in an effort to not
				// change working code any more than necessary, it is now
				// optional for an initial install.
				if (baseUpdateInfo != NULL)
				{
					getUpdateQueue()->addString(baseUpdateInfo->getUrl());
				}
				getUpdateQueue()->addString(fullUpdateInfo->getUrl());
			}
			else
			{
				throw 0;
			}
		}
		else
		{
			// Sort the updates by date.  Note that they will be either ZIP
			// updates or EXE updates, but not both.
			updateArray->sort();
			if (updateArray->getCount() == 0)
			{
				throw 0;
			}
			bool haveExeUpdates = false;//determineLastUpdate(exeUpdateArray,
				//lastExeUpdateName, aborted);
			bool haveZipUpdates = false;
			
			if (zipSupported)
			{
				haveZipUpdates = determineLastUpdate(updateArray,
					lastUpdateName, aborted);
				if (*aborted)
				{
					throw 0;
				}
			}
			if (haveExeUpdates && !haveZipUpdates)
			{
				strcpy(lastUpdateName, lastExeUpdateName);
			}
			if (haveZipUpdates || haveExeUpdates)
			{
				size_t updatesNeededCount = updateArray->getCount();

				for (i = (ptrdiff_t)updateArray->getCount() - 1; i >= 0; i--)
				{
					if (strcmp((*updateArray)[i]->getName(), lastUpdateName)
						== 0)
					{
						updatesNeededCount = updateArray->getCount() - i - 1;
						break;
					}
				}
				if (updatesNeededCount < updateArray->getCount())
				{
					fullUpdateNeeded = false;
					for (i = updateArray->getCount() - updatesNeededCount;
						(size_t)i < updateArray->getCount(); i++)
					{
						LDLibraryUpdateInfo *updateInfo = (*updateArray)[i];

						getUpdateQueue()->addString(updateInfo->getUrl());
					}
					if (!updatesNeededCount)
					{
						debugPrintf("No update needed.\n");
					}
				}
				else if (strlen(lastUpdateName) == 8)
				{
					ucstrcpy(m_error, ls(_UC("LDLUpdateCGIOutOfDate")));
					throw 0;
				}
			}
			if (fullUpdateNeeded)
			{
				debugPrintf("Full update needed.\n");
				if (fullUpdateInfo)
				{
					getUpdateQueue()->addString(fullUpdateInfo->getUrl());
				}
				else
				{
					debugPrintf("Full update info not found!!!\n");
				}
			}
		}
	}
	catch (...)
	{
		retValue = false;
	}
	updateArray->release();
	exeUpdateArray->release();
	TCObject::release(fullUpdateInfo);
	TCObject::release(baseUpdateInfo);
	return retValue;
}

TCStringArray *LDLibraryUpdater::getUpdateQueue(void)
{
	if (!m_updateQueue)
	{
		m_updateQueue = new TCStringArray;
	}
	return m_updateQueue;
}

bool LDLibraryUpdater::caseSensitiveFileSystem(UCSTR &error)
{
	size_t tempFilenameLen = strlen(m_ldrawDir) + 32;
	char *tempFilename = new char[tempFilenameLen];
	bool retValue = false;
	int i;

	for (i = 0; i < 100000; i++)
	{
		FILE *file;

		snprintf(tempFilename, tempFilenameLen, "%s/LDView%X.tmp", m_ldrawDir, i);
		file = ucfopen(tempFilename, "r");
		if (file)
		{
			fclose(file);
		}
		else
		{
			file = ucfopen(tempFilename, "w");
			if (file)
			{
				char *tempFilename2 = copyString(tempFilename);

				fclose(file);
				convertStringToLower(tempFilename2);
				file = ucfopen(tempFilename2, "r");
				delete[] tempFilename2;
				if (file)
				{
					fclose(file);
				}
				else
				{
					retValue = true;
				}
				unlink(tempFilename);
			}
			else
			{
				error = copyString(TCLocalStrings::get(
					_UC("LDLUpdateCantWrite")));
			}
			break;
		}
	}
	if (i == 100000)
	{
		error = copyString(TCLocalStrings::get(_UC("LDLUpdateTmpFileError")));
	}
	delete[] tempFilename;
	return retValue;
}

bool LDLibraryUpdater::canCheckForUpdates(UCSTR &error)
{
	bool caseSensitive;
	bool goodSuffix = false;
	error = NULL;

	caseSensitive = caseSensitiveFileSystem(error);
	if (error == NULL)
	{
		if (caseSensitive)
		{
#ifdef WIN32
			// I'm not sure if there ARE any case sensitive file systems in
			// Windows, but we may as well allow for it, since it's not really
			// much extra code.
			goodSuffix = stringHasSuffix(m_ldrawDir, "\\ldraw") ||
				stringHasSuffix(m_ldrawDir, "/ldraw");
#else
			goodSuffix = stringHasSuffix(m_ldrawDir, "/ldraw");
#endif
		}
		else
		{
#ifdef WIN32
			goodSuffix =
				stringHasCaseInsensitiveSuffix(m_ldrawDir, "\\ldraw") ||
				stringHasCaseInsensitiveSuffix(m_ldrawDir, "/ldraw");
#else
			goodSuffix = stringHasCaseInsensitiveSuffix(m_ldrawDir, "/ldraw");
#endif
		}
		if (!goodSuffix)
		{
			error = copyString(TCLocalStrings::get(_UC("LDLUpdateNotLDraw")));
		}
	}
	return goodSuffix;
}

void LDLibraryUpdater::installLDraw(void)
{
	m_install = true;
	launchThread();
}

void LDLibraryUpdater::checkForUpdates(void)
{
	m_install = false;
	launchThread();
}

void LDLibraryUpdater::launchThread(void)
{
	m_error[0] = 0;
	if (m_ldrawDir)
	{
		//ThreadHelper threadHelper(this);
		try
		{
#ifdef USE_CPP11
			m_thread = new std::thread(&LDLibraryUpdater::threadRun, this);
#else
			m_thread = new boost::thread(
				boost::bind(&LDLibraryUpdater::threadRun, this));
#endif
		}
		catch (...)
		{
			ucstrcpy(m_error, TCLocalStrings::get(_UC("LDLUpdateCreateThreadError")));
		}
	}
	else
	{
		ucstrcpy(m_error, TCLocalStrings::get(_UC("LDLUpdateNoLDrawDir")));
	}
}

void LDLibraryUpdater::threadRun(void)
{
	threadStart();
	threadFinish();
}

void LDLibraryUpdater::threadStart(void)
{
	TCWebClient *webClient = NULL;
	int dataLength;
	bool aborted;
	ucstringVector extraInfo;

	retain();	// We don't want to go away until our thread has finished.
				// Note that the corresponding release() is in the threadFinish
				// function.
	TCProgressAlert::send(LD_LIBRARY_UPDATER,
		TCLocalStrings::get(_UC("LDLUpdateDlList")), 0.01f, &aborted, this);
	if (!aborted)
	{
        std::string url = LDrawModelViewer::libraryUrl("updates?output=TAB");
		
		webClient = new TCWebClient(url.c_str());
		webClient->setOwner(this);
		webClient->fetchURL();
		TCProgressAlert::send(LD_LIBRARY_UPDATER,
			TCLocalStrings::get(_UC("LDLUpdateParseList")), 0.02f, &aborted,
			this);
	}
	if (!aborted)
	{
		dataLength = webClient->getPageLength();
		if (dataLength)
		{
			TCByte *data = webClient->getPageData();
			char *string = new char[(size_t)dataLength + 1];

			memcpy(string, data, dataLength);
			string[dataLength] = 0;
			debugPrintf("Got Page Data! (length = %d)\n", dataLength);
			if (!parseUpdateList(string, &aborted))
			{
				if (!aborted && !m_error[0])
				{
					ucstrcpy(m_error, TCLocalStrings::get(_UC("LDLUpdateDlParseError")));
				}
				aborted = true;
			}
			delete[] string;
		}
		else
		{
			ucstrcpy(m_error, TCLocalStrings::get(_UC("LDLUpdateDlListError")));
			aborted = true;
			debugPrintf("No Page Data!\n");
		}
	}
	if (webClient)
	{
		webClient->release();
	}
	if (!aborted)
	{
		if (m_updateQueue && m_updateQueue->getCount())
		{
			TCProgressAlert::send(LD_LIBRARY_UPDATER,
				TCLocalStrings::get(_UC("LDLUpdateDlUpdates")), 0.1f, &aborted,
				this);
			downloadUpdates(&aborted);
			if (!aborted)
			{
				extractUpdates(&aborted);
				if (!aborted)
				{
					size_t i;
					size_t count = m_updateUrlList->getCount();

					extraInfo.resize(count);
					for (i = 0; i < count; i++)
					{
						mbstoucstring(extraInfo[i], (*m_updateUrlList)[i]);
					}
				}
			}
		}
		else
		{
			// DO NOT make the following a local string.
			extraInfo.push_back(_UC("None"));
		}
	}
	if (ucstrlen(m_error))
	{
		TCProgressAlert::send(LD_LIBRARY_UPDATER, m_error, 2.0f, this);
	}
	else if (aborted)
	{
		TCProgressAlert::send(LD_LIBRARY_UPDATER,
			TCLocalStrings::get(_UC("LibraryUpdateCanceled")), 1.0f, this);
	}
	else
	{
		TCProgressAlert::send(LD_LIBRARY_UPDATER,
			TCLocalStrings::get(L"LDLUpdateDone"), 1.0f, extraInfo, this);
	}
}

void LDLibraryUpdater::extractUpdate(const char *filename)
{
	if (stringHasCaseInsensitiveSuffix(filename, ".zip"))
	{
		TCUnzip *unzip = new TCUnzip;

		if (unzip->unzip(filename, m_ldrawDirParent) != 0)
		{
			UCSTR ucFilename = mbstoucstring(filename);
			CUCSTR errorFormat =
				TCLocalStrings::get(_UC("LDLUpdateUnzipError"));
			sucprintf(m_error, COUNT_OF(m_error), errorFormat, ucFilename);
			delete[] ucFilename;
		}
		unzip->release();
	}
	else if (stringHasCaseInsensitiveSuffix(filename, ".exe"))
	{
#ifdef WIN32
		char commandLine[2048];
		char shortFilename[1024];
		char startupDir[1024];
		STARTUPINFO startupInfo;
		PROCESS_INFORMATION processInfo;
		int len;

		// We're dealing with 16-bit DOS programs, so we don't want any long
		// filenames in the executable path or the working directory path.
		len = GetShortPathName(filename, shortFilename, sizeof(shortFilename));
		if (len == 0 || len >= sizeof(shortFilename))
		{
			strncpy(shortFilename, filename, sizeof(shortFilename));
			shortFilename[sizeof(shortFilename) - 1] = 0;
		}
		len = GetShortPathName(m_ldrawDirParent, startupDir,
			sizeof(startupDir));
		if (len == 0 || len >= sizeof(startupDir))
		{
			strncpy(startupDir, m_ldrawDirParent, sizeof(startupDir));
			startupDir[sizeof(startupDir) - 1] = 0;
		}
		snprintf(commandLine, sizeof(commandLine), "%s /y", shortFilename);
		memset(&startupInfo, 0, sizeof(STARTUPINFO));
		startupInfo.cb = sizeof(STARTUPINFO);
		startupInfo.dwFlags = STARTF_USESHOWWINDOW;
		startupInfo.wShowWindow = SW_SHOWMINNOACTIVE;
		if (CreateProcess(shortFilename, commandLine, NULL, NULL, FALSE,
			DETACHED_PROCESS | NORMAL_PRIORITY_CLASS, NULL, startupDir,
			&startupInfo, &processInfo))
		{
			while (1)
			{
				DWORD exitCode;

				GetExitCodeProcess(processInfo.hProcess, &exitCode);
				if (exitCode != STILL_ACTIVE)
				{
					CloseHandle(processInfo.hProcess);
					CloseHandle(processInfo.hThread);
					break;
				}
				Sleep(50);
			}
		}
		else
		{
			sucprintf(m_error, COUNT_OF(m_error),
				TCLocalStrings::get(_UC("LDLUpdateExecuteError")),
				filename);
		}
#else
		ucstrcpy(m_error, TCLocalStrings::get(_UC("LDLUpdateNoDos")));
#endif
	}
	else
	{
		sucprintf(m_error, COUNT_OF(m_error),
			TCLocalStrings::get(_UC("LDLUpdateUnknownType")), filename);
	}
}

void LDLibraryUpdater::extractUpdates(bool *aborted)
{
	size_t i, j;
	size_t count = m_updateUrlList->getCount();

	TCProgressAlert::send(LD_LIBRARY_UPDATER,
		TCLocalStrings::get(_UC("LDLUpdateExtracting")), 0.9f, aborted, this);
	for (i = 0; i < count && !*aborted && ucstrlen(m_error) == 0; i++)
	{
		const char *url = (*m_updateUrlList)[i];
		const char *urlFile = strrchr(url, '\\');
		const char *slashSpot = strrchr(url, '/');

		if (slashSpot)
		{
			if (!urlFile || slashSpot > urlFile)
			{
				urlFile = slashSpot + 1;
			}
		}
		else if (urlFile)
		{
			urlFile++;
		}
		if (!urlFile)
		{
			continue;
		}
		for (j = 0; j < m_downloadList->getCount(); j++)
		{
			const char *filename = (*m_downloadList)[j];
			const char *downloadFile = strrchr(filename, '\\');

			slashSpot = strrchr(filename, '/');
			if (slashSpot)
			{
				if (!downloadFile || slashSpot > downloadFile)
				{
					downloadFile = slashSpot + 1;
				}
				else
				{
					downloadFile++;
				}
			}
			else if (downloadFile)
			{
				downloadFile++;
			}
			if (downloadFile && strcasecmp(urlFile, downloadFile) == 0)
			{
				extractUpdate(filename);
				m_downloadList->removeStringAtIndex(j);
				sendExtractProgress(aborted);
				break;
			}
		}
	}
}

void LDLibraryUpdater::updateDlFinish(TCWebClient *webClient)
{
#ifdef USE_CPP11
	std::unique_lock<std::mutex> lock(*m_mutex);
#else
	boost::mutex::scoped_lock lock(*m_mutex);
#endif

	if (!m_aborting)
	{
		if (webClient->getPageLength())
		{
			char filename[1024];

#ifdef WIN32
			snprintf(filename, sizeof(filename), "%s\\%s", m_ldrawDir, webClient->getFilename());
#else // WIN32
			snprintf(filename, sizeof(filename), "%s/%s", m_ldrawDir, webClient->getFilename());
#endif // WIN32
			debugPrintf("Done downloading file: %s\n", filename);
			m_downloadList->addString(filename);
		}
		else
		{
			sucprintf(m_error, COUNT_OF(m_error),
				TCLocalStrings::get(_UC("LDLUpdateDlError")),
				webClient->getURL());
			webClient->abort();
		}
		m_finishedWebClients->addObject(webClient);
		m_webClients->removeObject(webClient);
		m_threadFinish->notify_one();
	}
}

void LDLibraryUpdater::processUpdateQueue(void)
{
#ifdef USE_CPP11
	std::unique_lock<std::mutex> lock(*m_mutex);
#else
	boost::mutex::scoped_lock lock(*m_mutex);
#endif

	size_t webClientCount = m_webClients->getCount();
	lock.unlock();
	while (webClientCount < MAX_DL_THREADS && m_updateQueue->getCount() > 0)
	{
		TCWebClient *webClient = new TCWebClient((*m_updateQueue)[0]);

		m_updateQueue->removeStringAtIndex(0);
		webClient->setOwner(this);
		webClient->setFinishURLMemberFunction(
			(WebClientFinishMemberFunction)(&LDLibraryUpdater::updateDlFinish));
		webClient->setOutputDirectory(m_ldrawDir);
		if (webClient->fetchURLInBackground())
		{
			lock.lock();
			m_webClients->addObject(webClient);
			webClientCount = m_webClients->getCount();
			lock.unlock();
		}
		webClient->release();
	}
}

void LDLibraryUpdater::sendDlProgress(bool *aborted)
{
	size_t i;
	size_t count;
	size_t completedUpdates;
	float fileFraction;
	float progress;
	
	{
#ifdef USE_CPP11
		std::unique_lock<std::mutex> lock(*m_mutex);
#else
		boost::mutex::scoped_lock lock(*m_mutex);
#endif

		count = m_webClients->getCount();
		completedUpdates = m_initialQueueSize - m_updateQueue->getCount()
			- count;
		fileFraction = 0.99f / (float)m_initialQueueSize * 0.8f;
		progress = 0.1f + (float)completedUpdates * fileFraction;

		for (i = 0; i < count; i++)
		{
			TCWebClient *webClient = (*m_webClients)[i];
			int bytesRead = webClient->getBytesRead();
			int pageLength = webClient->getPageLength();

			if (bytesRead && pageLength)
			{
				progress += (float)(bytesRead) / (float)(pageLength) *
					fileFraction;
			}
		}
	}
	TCProgressAlert::send(LD_LIBRARY_UPDATER,
		TCLocalStrings::get(_UC("LDLUpdateDlUpdates")), progress, aborted,
		this);
}

void LDLibraryUpdater::sendExtractProgress(bool *aborted)
{
	size_t total = m_updateUrlList->getCount();
	size_t finished = total - m_downloadList->getCount();
	double fileFraction = 0.99 / (double)total * 0.1;
	double progress = 0.9 + (double)finished * fileFraction;

	TCProgressAlert::send(LD_LIBRARY_UPDATER,
		TCLocalStrings::get(_UC("LDLUpdateExtracting")), (float)progress, aborted,
		this);
}

void LDLibraryUpdater::downloadUpdates(bool *aborted)
{
	bool done = false;

	TCObject::release(m_downloadList);
	m_downloadList = new TCStringArray;
	m_initialQueueSize = m_updateQueue->getCount();
	TCObject::release(m_updateUrlList);
	m_updateUrlList = (TCStringArray *)m_updateQueue->copy();
	while (!done && !*aborted)
	{
		TCWebClient *finishedWebClient = NULL;
#ifdef USE_CPP11
		std::unique_lock<std::mutex> lock(*m_mutex, std::defer_lock);
#else
#if BOOST_VERSION >= 103500
		boost::mutex::scoped_lock lock(*m_mutex, boost::defer_lock);
#else	// BOOST version >= 1.35.0 above, < 1.35.0 below
		boost::mutex::scoped_lock lock(*m_mutex, false);
#endif // BOOST version < 1.35.0
#endif
		processUpdateQueue();
		lock.lock();
		if (m_finishedWebClients->getCount() > 0)
		{
			finishedWebClient = (*m_finishedWebClients)[0];
			finishedWebClient->retain();
			m_finishedWebClients->removeObjectAtIndex(0);
			if (finishedWebClient->getAborted())
			{
				*aborted = true;
				debugPrintf("Error downloading %s\n",
					finishedWebClient->getURL());
			}
		}
		if (m_webClients->getCount() > 0 || finishedWebClient)
		{
			// If we get here, we either have running web clients, a finished
			// web client, or both.
			if (finishedWebClient)
			{
				// We have a finished web client, so join against its thread
				// so that we can then get rid of it.
				lock.unlock();
				finishedWebClient->getFetchThread()->join();
				lock.lock();
				finishedWebClient->release();
			}
			else
			{
				// We don't have any finished web clients, but we do have at
				// least one web client still running, so wait for 250msec for
				// it to signal.
#ifdef USE_CPP11
				m_threadFinish->wait_for(lock, std::chrono::milliseconds(250));
#else
				boost::xtime xt;
#if BOOST_VERSION >= 105000
				boost::xtime_get(&xt, boost::TIME_UTC_);
#else
#ifndef TIME_UTC
				boost::xtime_get(&xt, boost::TIME_UTC);
#else
				boost::xtime_get(&xt, TIME_UTC);
#endif
#endif
				xt.nsec += 250 * 1000 * 1000;
				m_threadFinish->timed_wait(lock, xt);
#endif
			}
			lock.unlock();
			if (!*aborted)
			{
				sendDlProgress(aborted);
			}
			lock.lock();
			if (*aborted)
			{
				m_aborting = true;
			}
		}
		else
		{
			done = true;
		}
	}
	if (*aborted)
	{
#ifdef USE_CPP11
		std::unique_lock<std::mutex> lock(*m_mutex);
#else
		boost::mutex::scoped_lock lock(*m_mutex);
#endif
		size_t i;
		size_t count = m_webClients->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_webClients)[i]->abort();
		}
		while (m_webClients->getCount())
		{
			size_t index = m_webClients->getCount() - 1;
			TCWebClient *webClient = (*m_webClients)[index];

			lock.unlock();
			webClient->getFetchThread()->join();
			lock.lock();
			m_webClients->removeObjectAtIndex(index);
		}
	}
}

void LDLibraryUpdater::threadFinish(void)
{
	release();
}

bool LDLibraryUpdater::fileExists(const char *filename)
{
	std::ifstream stream;
	return LDLModel::openFile(filename, stream);
}

#endif // USE_CPP11 || !_NO_BOOST
