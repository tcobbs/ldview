#include "LDLibraryUpdater.h"
#include "LDLibraryUpdateInfo.h"
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCWebClient.h>
#include <TCFoundation/TCThread.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include <TCFoundation/TCSortedStringArray.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/mystring.h>


LDLibraryUpdater::LDLibraryUpdater(void)
	:m_webClients(new TCWebClientArray),
	m_thread(NULL),
	m_libraryUpdateKey(NULL),
	m_ldrawDir(NULL)
{
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
		delete m_ldrawDir;
		m_ldrawDir = copyString(ldrawDir);
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

				debugPrintf("Update file: %s\n", updateInfo->getExeUrl());
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
			debugPrintf("Full file: %s\n", fullUpdateInfo->getExeUrl());
		}
		else
		{
			debugPrintf("Full update info not found!!!\n");
		}
	}
	updateArray->release();
	TCObject::release(fullUpdateInfo);
}

void LDLibraryUpdater::checkForUpdates(void)
{
	if (m_ldrawDir)
	{
		m_thread = new TCThread(this,
		 (TCThreadStartMemberFunction)(&LDLibraryUpdater::threadStart));
//		m_thread->setFinishMemberFunction(
//		 (TCThreadFinishMemberFunction)(&LDLibraryUpdater::threadFinish));
		m_thread->run();
	}
}

THREAD_RET_TYPE LDLibraryUpdater::threadStart(TCThread * /*thread*/)
{
	TCWebClient *webClient = NULL;
	int dataLength;
	bool aborted;
	int oldDebugLevel = getDebugLevel();

	setDebugLevel(1);
	TCProgressAlert::send("LDLibraryUpdater",
		"Downloading update list from ldraw.org", 0.01f, &aborted);
	if (!aborted)
	{
		debugPrintf("downloadTest: 0x%08X\n", GetCurrentThreadId());
//		webClient = new TCWebClient("http://www.ldraw.org/cgi-bin/ptreleases.cgi");
		webClient = new TCWebClient(
			"http://www.halibut.com/~tcobbs/ldraw/private/ptreleases_cgi.txt");
		webClient->setOwner(this);
		webClient->fetchURL();
		TCProgressAlert::send("LDLibraryUpdater", "Parsing update list", 0.1f,
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
			debugPrintf("No Page Data!\n");
		}
	}
	if (webClient)
	{
		webClient->release();
	}
	setDebugLevel(oldDebugLevel);
	TCProgressAlert::send("LDLibraryUpdater", "Done", 1.0f);
	return 0;
}

//void LDLibraryUpdater::threadFinish(TCThread * /*thread*/)
//{
//}

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
