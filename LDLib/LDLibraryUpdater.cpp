#include "LDLibraryUpdater.h"
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

// NOTE: warning level on this source file had to be set to level 3 in Visual
// Studio in order to get it to compile without warnings.  For some unknown
// reason, disabling the warnings that show up including the below doesn't work.
// This is only necessary in VS 6.  The Release build target is set to warning
// level 4, because I built that with VC++ Toolkit 2003.
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/bind.hpp>


#ifdef _QT
#include <ctype.h>
#endif // _QT

#define MAX_DL_THREADS 2

LDLibraryUpdater::LDLibraryUpdater(void)
	:m_webClients(new TCWebClientArray),
	m_finishedWebClients(new TCWebClientArray),
//	m_thread(NULL),
	m_thread(NULL),
	m_mutex(new boost::mutex),
	m_threadFinish(new boost::condition),
	m_libraryUpdateKey(NULL),
	m_ldrawDir(NULL),
	m_ldrawDirParent(NULL),
	m_updateQueue(NULL),
	m_updateUrlList(NULL),
	m_downloadList(NULL),
	m_aborting(false)
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
		m_thread->join();
		delete m_thread;
	}
	delete m_libraryUpdateKey;
	delete m_ldrawDir;
	delete m_ldrawDirParent;
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

		delete m_ldrawDir;
		m_ldrawDir = copyString(ldrawDir);
		delete m_ldrawDirParent;
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
	int i;
	int count = updateArray->getCount();
	int leftIndex = -1;
	int rightIndex = -1;

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
		return strcasecmp(left, right);
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

bool LDLibraryUpdater::determineLastUpdate(LDLibraryUpdateInfoArray
										   *updateArray, char *updateName)
{
	char *lastRecordedUpdate = NULL;

	if (m_libraryUpdateKey)
	{
		lastRecordedUpdate =
			TCUserDefaults::stringForKey(m_libraryUpdateKey, NULL, false);
	}
	if (lastRecordedUpdate)
	{
		strcpy(updateName, lastRecordedUpdate);
		delete lastRecordedUpdate;
		return true;
	}
	else
	{
		char buf[1024];
		FILE *completeTextFile;
		int i;
		bool done = false;

		updateName[0] = 0;
		sprintf(buf, "%s\\models\\complete.txt", m_ldrawDir);
		// Despite the name of the function, LDLModel::openModelFile just opens
		// a file.  But it supports case-insensitive opening of a file on a
		// case-sensitive file system.
		completeTextFile = LDLModel::openModelFile(buf);
		if (completeTextFile)
		{
			bool firstUpdateFound = false;

			// First, we'll see what's the latest update in the installed
			// complete package.
			while (1)
			{
				char *lcad;

				if (fgets(buf, sizeof(buf), completeTextFile) == NULL)
				{
					// If we have a real complete.txt file out of a complete
					// update install, we shouldn't get here, but we certainly
					// don't want to assume anything.
					break;
				}
				lcad = strcasestr(buf, "lcad");
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
			fclose(completeTextFile);
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
				strncpy(tmpFilename, "note", 4);
				sprintf(buf, "%s\\models\\%s.txt", m_ldrawDir, tmpFilename);
				if (fileExists(buf))
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

bool LDLibraryUpdater::parseUpdateList(const char *updateList)
{
	int lineCount;
	char **updateListLines = componentsSeparatedByString(updateList, "\n",
		lineCount);
	char lastUpdateName[1024];
	int i;
	bool fullUpdateNeeded = true;
	LDLibraryUpdateInfoArray *updateArray = new LDLibraryUpdateInfoArray;
	LDLibraryUpdateInfoArray *exeUpdateArray = new LDLibraryUpdateInfoArray;
	LDLibraryUpdateInfo *fullUpdateInfo = NULL;
	LDLibraryUpdateInfo *baseUpdateInfo = NULL;
	bool zipSupported = TCUnzip::supported();

	for (i = 0; i < lineCount; i++)
	{
		LDLibraryUpdateInfo *updateInfo = new LDLibraryUpdateInfo;

		if (updateInfo->parseUpdateLine(updateListLines[i]))
		{
			// We're not going to apply so many updates as to have to go all the
			// way back to the EXE-only updates.  If ZIP is supported, only pay
			// attention to the ZIP updates; if ZIP isn't supported, only pay
			// attention to the EXE updates.
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
		if (fullUpdateInfo && baseUpdateInfo)
		{
			getUpdateQueue()->addString(baseUpdateInfo->getUrl());
			getUpdateQueue()->addString(fullUpdateInfo->getUrl());
		}
		else
		{
			updateArray->release();
			exeUpdateArray->release();
			TCObject::release(fullUpdateInfo);
			TCObject::release(baseUpdateInfo);
			return false;
		}
	}
	else
	{
		// Sort the updates by date.  Note that they will be either ZIP updates or
		// EXE updates, but not both.
		updateArray->sort();
		if (updateArray->getCount() == 0)
		{
			updateArray->release();
			exeUpdateArray->release();
			TCObject::release(fullUpdateInfo);
			TCObject::release(baseUpdateInfo);
			return false;
		}
		if (determineLastUpdate(exeUpdateArray, lastUpdateName))
		{
			int updatesNeededCount = updateArray->getCount();

			for (i = updateArray->getCount() - 1; i >= 0; i--)
			{
				if (strcmp((*updateArray)[i]->getName(), lastUpdateName) == 0)
				{
					updatesNeededCount = updateArray->getCount() - i - 1;
					break;
				}
			}
			if (updatesNeededCount < updateArray->getCount())
			{
				fullUpdateNeeded = false;
				for (i = updateArray->getCount() - updatesNeededCount;
					i < updateArray->getCount(); i++)
				{
					LDLibraryUpdateInfo *updateInfo = (*updateArray)[i];

					getUpdateQueue()->addString(updateInfo->getUrl());
				}
				if (!updatesNeededCount)
				{
					debugPrintf("No update needed.\n");
				}
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
	updateArray->release();
	exeUpdateArray->release();
	TCObject::release(fullUpdateInfo);
	TCObject::release(baseUpdateInfo);
	return true;
}

TCStringArray *LDLibraryUpdater::getUpdateQueue(void)
{
	if (!m_updateQueue)
	{
		m_updateQueue = new TCStringArray;
	}
	return m_updateQueue;
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
		setDebugLevel(3);
		try
		{
			m_thread = new boost::thread(
				boost::bind(&LDLibraryUpdater::threadRun, this));
			//m_thread = new boost::thread(threadHelper);
		}
		catch (...)
		{
			strcpy(m_error, TCLocalStrings::get("LDLUpdateCreateThreadError"));
		}
	}
	else
	{
		strcpy(m_error, TCLocalStrings::get("LDLUpdateNoLDrawDir"));
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
	TCStringArray *extraInfo = NULL;

	retain();	// We don't want to go away until our thread has finished.
				// Note that the corresponding release() is in the threadFinish
				// function.
	TCProgressAlert::send(LD_LIBRARY_UPDATER,
		TCLocalStrings::get("LDLUpdateDlList"), 0.01f, &aborted);
	if (!aborted)
	{
		const char *url = "http://www.ldraw.org/cgi-bin/ptreleases.cgi?"
			"output=TAB&fields=type-format-date-url-size";
		
		url = "http://www.halibut.com/~tcobbs/PartUpdates.txt";
		webClient = new TCWebClient(url);
		webClient->setOwner(this);
		webClient->fetchURL();
		TCProgressAlert::send(LD_LIBRARY_UPDATER,
			TCLocalStrings::get("LDLUpdateParseList"), 0.09f, &aborted);
	}
	if (!aborted)
	{
		dataLength = webClient->getPageLength();
		if (dataLength)
		{
			TCByte *data = webClient->getPageData();
			char *string = new char[dataLength + 1];

			memcpy(string, data, dataLength);
			string[dataLength] = 0;
			debugPrintf("Got Page Data! (length = %d)\n", dataLength);
			if (!parseUpdateList(string))
			{
				strcpy(m_error, TCLocalStrings::get("LDLUpdateDlParseError"));
				aborted = true;
			}
			delete string;
		}
		else
		{
			strcpy(m_error, TCLocalStrings::get("LDLUpdateDlListError"));
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
				TCLocalStrings::get("LDLUpdateDlUpdates"), 0.1f, &aborted);
			downloadUpdates(&aborted);
			if (!aborted)
			{
				extractUpdates(&aborted);
				if (!aborted)
				{
					extraInfo = (TCStringArray *)m_updateUrlList->copy();
				}
			}
		}
		else
		{
			extraInfo = new TCStringArray;
			// DO NOT make the following a local string.
			extraInfo->addString("None");
		}
	}
	setDebugLevel(0);
	if (strlen(m_error))
	{
		TCProgressAlert::send(LD_LIBRARY_UPDATER, m_error, 2.0f);
	}
	else if (aborted)
	{
		TCProgressAlert::send(LD_LIBRARY_UPDATER,
			TCLocalStrings::get("LibraryUpdateCanceled"), 1.0f);
	}
	else
	{
		TCProgressAlert::send(LD_LIBRARY_UPDATER,
			TCLocalStrings::get("LDLUpdateDone"), 1.0f, extraInfo);
	}
	TCObject::release(extraInfo);
}

void LDLibraryUpdater::extractUpdate(const char *filename)
{
	if (stringHasCaseInsensitiveSuffix(filename, ".zip"))
	{
		TCUnzip *unzip = new TCUnzip;

		if (unzip->unzip(filename, m_ldrawDirParent) != 0)
		{
			sprintf(m_error, TCLocalStrings::get("LDLUpdateUnzipError"),
				filename);
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
		sprintf(commandLine, "%s /y", shortFilename);
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
					break;
				}
				Sleep(50);
			}
		}
		else
		{
			sprintf(m_error, TCLocalStrings::get("LDLUpdateExecuteError"),
				filename);
		}
#else
		strcpy(m_error, TCLocalStrings::get("LDLUpdateNoDos"));
#endif
	}
	else
	{
		sprintf(m_error, TCLocalStrings::get("LDLUpdateUnknownType"), filename);
	}
}

void LDLibraryUpdater::extractUpdates(bool *aborted)
{
	int i, j;
	int count = m_updateUrlList->getCount();

	TCProgressAlert::send(LD_LIBRARY_UPDATER,
		TCLocalStrings::get("LDLUpdateExtracting"), 0.9f, aborted);
	for (i = 0; i < count && !*aborted; i++)
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
			if (strcasecmp(urlFile, downloadFile) == 0)
			{
				extractUpdate(filename);
				m_downloadList->removeString(j);
				sendExtractProgress(aborted);
				break;
			}
		}
	}
}

void LDLibraryUpdater::updateDlFinish(TCWebClient *webClient)
{
	boost::mutex::scoped_lock lock(*m_mutex);

	if (!m_aborting)
	{
		if (webClient->getPageLength())
		{
			char filename[1024];

#ifdef WIN32
			sprintf(filename, "%s\\%s", m_ldrawDir, webClient->getFilename());
#else // WIN32
			sprintf(filename, "%s/%s", m_ldrawDir, webClient->getFilename());
#endif // WIN32
			debugPrintf("Done downloading file: %s\n", filename);
			m_downloadList->addString(filename);
		}
		else
		{
			sprintf(m_error, TCLocalStrings::get("LDLUpdateDlError"),
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
	int webClientCount;
	boost::mutex::scoped_lock lock(*m_mutex);

	webClientCount = m_webClients->getCount();
	lock.unlock();
	while (webClientCount < MAX_DL_THREADS && m_updateQueue->getCount() > 0)
	{
		TCWebClient *webClient = new TCWebClient((*m_updateQueue)[0]);

		m_updateQueue->removeString(0);
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
	int i;
	int count;
	int completedUpdates;
	float fileFraction;
	float progress;
	
	{
		boost::mutex::scoped_lock lock(*m_mutex);

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
		TCLocalStrings::get("LDLUpdateDlUpdates"), progress, aborted);
}

void LDLibraryUpdater::sendExtractProgress(bool *aborted)
{
	int total = m_updateUrlList->getCount();
	int finished = total - m_downloadList->getCount();
	float fileFraction = 0.99f / (float)total * 0.1f;
	float progress = 0.9f + (float)finished * fileFraction;

	TCProgressAlert::send(LD_LIBRARY_UPDATER,
		TCLocalStrings::get("LDLUpdateExtracting"), progress, aborted);
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
		boost::mutex::scoped_lock lock(*m_mutex, false);

		processUpdateQueue();
		lock.lock();
		if (m_finishedWebClients->getCount() > 0)
		{
			finishedWebClient = (*m_finishedWebClients)[0];
			finishedWebClient->retain();
			m_finishedWebClients->removeObject(0);
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
				boost::xtime xt;

				boost::xtime_get(&xt, boost::TIME_UTC);
				xt.nsec += 250 * 1000 * 1000;
				m_threadFinish->timed_wait(lock, xt);
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
		boost::mutex::scoped_lock lock(*m_mutex);
		int i;
		int count = m_webClients->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_webClients)[i]->abort();
		}
		while (m_webClients->getCount())
		{
			int index = m_webClients->getCount() - 1;
			TCWebClient *webClient = (*m_webClients)[index];

			lock.unlock();
			webClient->getFetchThread()->join();
			lock.lock();
			m_webClients->removeObject(index);
		}
	}
}

void LDLibraryUpdater::threadFinish(void)
{
	release();
}

bool LDLibraryUpdater::fileExists(const char *filename)
{
	FILE* file = LDLModel::openModelFile(filename);

	if (file)
	{
		fclose(file);
		return true;
	}
	else
	{
		return false;
	}
}
