#include "LDLibraryUpdater.h"
#include "LDLibraryUpdateInfo.h"
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCWebClient.h>
#include <TCFoundation/TCThread.h>
#include <TCFoundation/TCThreadManager.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCUnzip.h>

#define MAX_DL_THREADS 2

LDLibraryUpdater::LDLibraryUpdater(void)
	:m_webClients(new TCWebClientArray),
	m_thread(NULL),
	m_libraryUpdateKey(NULL),
	m_ldrawDir(NULL),
	m_ldrawDirParent(NULL),
	m_updateQueue(NULL),
	m_updateUrlList(NULL),
	m_downloadList(NULL)
{
	m_error[0] = 0;
}

LDLibraryUpdater::~LDLibraryUpdater(void)
{
}

void LDLibraryUpdater::dealloc(void)
{
	TCObject::release(m_webClients);
	TCObject::release(m_thread);
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
		completeTextFile = fopen(buf, "rb");
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

void LDLibraryUpdater::parseUpdateList(const char *updateList)
{
	int lineCount;
	char **updateListLines = componentsSeparatedByString(updateList, "\n",
		lineCount);
	char lastUpdateName[1024];
	int i;
	bool fullUpdateNeeded = true;
	LDLibraryUpdateInfoArray *updateArray = new LDLibraryUpdateInfoArray;
	LDLibraryUpdateInfo *fullUpdateInfo = NULL;
	bool zipSupported = TCUnzip::supported();

	for (i = 0; i < lineCount; i++)
	{
		LDLibraryUpdateInfo *updateInfo = new LDLibraryUpdateInfo;

		if (updateInfo->parseUpdateLine(updateListLines[i]))
		{
			if (updateInfo->isFullUpdate())
			{
				fullUpdateInfo = updateInfo;
			}
			else
			{
				updateArray->addObject(updateInfo);
				updateInfo->release();
			}
		}
		else
		{
			updateInfo->release();
		}
	}
	deleteStringArray(updateListLines, lineCount);
	updateArray->sort();
	TCObject::release(m_updateQueue);
	m_updateQueue = NULL;
	if (determineLastUpdate(updateArray, lastUpdateName))
	{
		int updatesNeededCount = 0;

//		strcat(lastUpdateName, ".exe");
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

				if (zipSupported && updateInfo->getZipUrl())
				{
					getUpdateQueue()->addString(updateInfo->getZipUrl());
				}
				else
				{
					getUpdateQueue()->addString(updateInfo->getExeUrl());
				}
			}
			if (!updatesNeededCount)
			{
				debugPrintf("No update needed.\n");
			}
		}
	}
	if (fullUpdateNeeded)
	{
		printf("Full update needed.\n");
		if (fullUpdateInfo)
		{
			if (zipSupported && fullUpdateInfo->getZipUrl())
			{
				getUpdateQueue()->addString(fullUpdateInfo->getZipUrl());
			}
			else
			{
				getUpdateQueue()->addString(fullUpdateInfo->getExeUrl());
			}
		}
		else
		{
			debugPrintf("Full update info not found!!!\n");
		}
	}
	updateArray->release();
	TCObject::release(fullUpdateInfo);
}

TCStringArray *LDLibraryUpdater::getUpdateQueue(void)
{
	if (!m_updateQueue)
	{
		m_updateQueue = new TCStringArray;
	}
	return m_updateQueue;
}

void LDLibraryUpdater::checkForUpdates(void)
{
	m_error[0] = 0;
	if (m_ldrawDir)
	{
		setDebugLevel(3);
		m_thread = new TCThread(this,
		 (TCThreadStartMemberFunction)(&LDLibraryUpdater::threadStart));
		m_thread->setFinishMemberFunction(
		 (TCThreadFinishMemberFunction)(&LDLibraryUpdater::threadFinish));
		m_thread->run();
	}
	else
	{
		strcpy(m_error, "Cannot determine LDraw directory.\n");
	}
}

THREAD_RET_TYPE LDLibraryUpdater::threadStart(TCThread * /*thread*/)
{
	TCWebClient *webClient = NULL;
	int dataLength;
	bool aborted;
//	int oldDebugLevel = getDebugLevel();

	retain();	// We don't want to go away until our thread has finished.
	TCProgressAlert::send(LD_LIBRARY_UPDATER,
		"Downloading update list from ldraw.org", 0.01f, &aborted);
	if (!aborted)
	{
		debugPrintf("downloadTest: 0x%08X\n", GetCurrentThreadId());
		webClient =
			new TCWebClient("http://www.ldraw.org/cgi-bin/ptreleases.cgi");
//		webClient = new TCWebClient(
//			"http://www.halibut.com/~tcobbs/ldraw/private/ptreleases_cgi.txt");
		webClient->setOwner(this);
		webClient->fetchURL();
		TCProgressAlert::send(LD_LIBRARY_UPDATER, "Parsing update list", 0.09f,
			&aborted);
	}
	if (!aborted)
	{
		dataLength = webClient->getPageLength();
		if (dataLength)
		{
			BYTE *data = webClient->getPageData();
			char *string = new char[dataLength + 1];

			memcpy(string, data, dataLength);
			string[dataLength] = 0;
			debugPrintf("Got Page Data! (length = %d)\n", dataLength);
			parseUpdateList(string);
			delete string;
		}
		else
		{
			strcpy(m_error, "Error downloading update list.\n");
			debugPrintf("No Page Data!\n");
		}
	}
	if (webClient)
	{
		webClient->release();
	}
	if (m_updateQueue && m_updateQueue->getCount() && !aborted)
	{
		TCProgressAlert::send(LD_LIBRARY_UPDATER, "Downloading updates", 0.1f,
			&aborted);
		downloadUpdates(&aborted);
		if (!aborted)
		{
			extractUpdates();
		}
	}
	setDebugLevel(0);
	TCProgressAlert::send(LD_LIBRARY_UPDATER, "Done", 1.0f);
	return 0;
}

void LDLibraryUpdater::extractUpdate(const char *filename)
{
	if (stringHasCaseInsensitiveSuffix(filename, ".zip"))
	{
		TCUnzip *unzip = new TCUnzip;

		if (unzip->unzip(filename, m_ldrawDirParent) != 0)
		{
			sprintf(m_error, "Error unzipping %s.\n", filename);
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
			sprintf(m_error, "Error executing %s.\n", filename);
		}
#else
		strcpy(m_error, "DOS executables not supported.\n");
#endif
	}
	else
	{
		sprintf(m_error, "Unknown update file type for file %s.\n", filename);
	}
}

void LDLibraryUpdater::extractUpdates(void)
{
	int i, j;
	int count = m_updateUrlList->getCount();

	for (i = 0; i < count; i++)
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
			}
			else if (downloadFile)
			{
				downloadFile++;
			}
			if (strcasecmp(urlFile, downloadFile) == 0)
			{
				extractUpdate(filename);
				m_downloadList->removeString(j);
				break;
			}
		}
	}
}

void LDLibraryUpdater::updateDlFinish(TCWebClient *webClient)
{
	if (webClient->getPageLength())
	{
		char filename[1024];

		sprintf(filename, "%s\\%s", m_ldrawDir, webClient->getFilename());
		debugPrintf("Done downloading file: %s\n", filename);
		m_downloadList->addString(filename);
	}
	else
	{
		sprintf(m_error, "Error downloading %s.\n", webClient->getURL());
	}
	m_webClients->removeObject(webClient);
}

void LDLibraryUpdater::processUpdateQueue(void)
{
	while (m_webClients->getCount() < MAX_DL_THREADS &&
		m_updateQueue->getCount() > 0)
	{
		TCWebClient *webClient = new TCWebClient((*m_updateQueue)[0]);

		m_updateQueue->removeString(0);
		webClient->setOwner(this);
		webClient->setFinishURLMemberFunction(
			(WebClientFinishMemberFunction)(&LDLibraryUpdater::updateDlFinish));
		webClient->setOutputDirectory(m_ldrawDir);
		if (webClient->fetchURLInBackground())
		{
			m_webClients->addObject(webClient);
		}
		webClient->release();
	}
}

void LDLibraryUpdater::sendDlProgress(bool *aborted)
{
	int i;
	int count = m_webClients->getCount();
	int completedUpdates = m_initialQueueSize - m_updateQueue->getCount()
		- m_webClients->getCount();
	float fileFraction = 0.99f / (float)m_initialQueueSize * 0.8f;
	float progress = 0.1f + (float)completedUpdates * fileFraction;

	for (i = 0; i < count; i++)
	{
		TCWebClient *webClient = (*m_webClients)[i];
		int bytesRead = webClient->getBytesRead();
		int pageLength = webClient->getPageLength();

		if (bytesRead && pageLength)
		{
			progress += (float)(bytesRead) / (float)(pageLength) * fileFraction;
		}
	}
	TCProgressAlert::send(LD_LIBRARY_UPDATER, "Downloading updates", progress,
		aborted);
}

void LDLibraryUpdater::downloadUpdates(bool *aborted)
{
	TCThreadManager *threadManager = TCThreadManager::threadManager();
	TCThread *finishedThread;
	bool done = false;

	TCObject::release(m_downloadList);
	m_downloadList = new TCStringArray;
	m_initialQueueSize = m_updateQueue->getCount();
	TCObject::release(m_updateUrlList);
	m_updateUrlList = (TCStringArray *)m_updateQueue->copy();
	while (!done && !*aborted)
	{
		processUpdateQueue();
		if (m_webClients->getCount())
		{
			if (threadManager->timedWaitForFinishedThread(250))
			{
				while ((finishedThread = threadManager->getFinishedThread())
					!= NULL)
				{
					threadManager->removeFinishedThread(finishedThread);
				}
			}
			sendDlProgress(aborted);
		}
		else
		{
			done = true;
		}
	}
}

void LDLibraryUpdater::threadFinish(TCThread * /*thread*/)
{
	release();
}

bool LDLibraryUpdater::fileExists(const char *filename)
{
	FILE* file = fopen(filename, "r");

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
