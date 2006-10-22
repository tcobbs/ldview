#include "TCUserDefaults.h"
#include "TCStringArray.h"
#include "TCAlert.h"
#include "TCAlertManager.h"
#include "mystring.h"

#if defined(__APPLE__) && !defined(_QT)
#import <Foundation/Foundation.h>
#endif // __APPLE__ && !_QT
#include <stdio.h>

TCUserDefaults* TCUserDefaults::currentUserDefaults = NULL;

TCUserDefaults::TCUserDefaultsCleanup TCUserDefaults::userDefaultsCleanup;
char *TCUserDefaults::argv0 = NULL;

TCUserDefaults::TCUserDefaultsCleanup::~TCUserDefaultsCleanup(void)
{
	delete argv0;
	argv0 = NULL;
	if (currentUserDefaults)
	{
		currentUserDefaults->release();
	}
}

TCUserDefaults::TCUserDefaults(void)
	:appName(NULL),
	sessionName(NULL),
	commandLine(NULL)
#ifdef WIN32
	,hAppDefaultsKey(NULL),
	hSessionKey(NULL)
#endif // WIN32
#if defined(__APPLE__) && !defined(_QT)
	,sessionDict(nil)
#endif // __APPLE__ && !defined(_QT)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TCUserDefaults");
#endif
#ifdef _QT
	qSettings = new QSettings;
#endif // _QT
#if defined(__APPLE__) && !defined(_QT)
	appName = copyString([[[NSBundle mainBundle] bundleIdentifier]
		cStringUsingEncoding: NSASCIIStringEncoding]);
#endif // __APPLE__ && !_QT
}

void TCUserDefaults::dealloc(void)
{
#ifdef _QT
	delete qSettings;
#endif // _QT
#if defined(__APPLE__) && !defined(_QT)
	[sessionDict release];
#endif // __APPLE__ && !_QT
	delete appName;
	delete sessionName;
	appName = NULL;
	if (commandLine)
	{
		commandLine->release();
		commandLine = NULL;
	}
#ifdef WIN32
	if (hSessionKey && hSessionKey != hAppDefaultsKey)
	{
		RegCloseKey(hSessionKey);
	}
	if (hAppDefaultsKey)
	{
		RegCloseKey(hAppDefaultsKey);
	}
#endif // WIN32
	TCObject::dealloc();
}

TCUserDefaults::~TCUserDefaults(void)
{
}

void TCUserDefaults::deleteResult(void* value)
{
	delete (char*)value;
}

TCUserDefaults* TCUserDefaults::getCurrentUserDefaults(void)
{
	if (!currentUserDefaults)
	{
		currentUserDefaults = new TCUserDefaults;
	}
	return currentUserDefaults;
}

TCStringArray* TCUserDefaults::getProcessedCommandLine(void)
{
	return getCurrentUserDefaults()->defGetProcessedCommandLine();
}

TCStringArray* TCUserDefaults::getAllKeys(void)
{
	return getCurrentUserDefaults()->defGetAllKeys();
}

TCStringArray* TCUserDefaults::getAllSessionNames(void)
{
	return getCurrentUserDefaults()->defGetAllSessionNames();
}

void TCUserDefaults::saveSessionNameInKey(const char* key)
{
	getCurrentUserDefaults()->defSaveSessionNameInKey(key);
}

char* TCUserDefaults::getSavedSessionNameFromKey(const char* key)
{
	return getCurrentUserDefaults()->defGetSavedSessionNameFromKey(key);
}

void TCUserDefaults::removeSession(const char* value)
{
	getCurrentUserDefaults()->defRemoveSession(value);
}


void TCUserDefaults::setCommandLine(char *argv[])
{
	TCStringArray *argArray = new TCStringArray;
	int i;

	delete argv0;
	argv0 = copyString(argv[0]);
	for (i = 0; argv[i]; i++)
	{
		if (i > 0)
		{
			argArray->addString(argv[i]);
		}
	}
	getCurrentUserDefaults()->defSetCommandLine(argArray);
	argArray->release();
}

void TCUserDefaults::setCommandLine(const char *args)
{
	TCStringArray *argArray = new TCStringArray;
	const char *tmpString = args;
	char tmpBuf[1024];

	while (tmpString[0])
	{
		const char *end = NULL;

		if (tmpString[0] == '"')
		{
			end = strchr(tmpString + 1, '"');
			tmpString++;
		}
		else
		{
			end = strchr(tmpString, ' ');
			if (tmpString[0] == '-')
			{
				const char *equals = strchr(tmpString, '=');

				if (equals && equals < end && equals[1] == '"')
				{
					end = strchr(equals + 2, '"');
					if (end[0] == '"')
					{
						end++;
					}
				}
			}
		}
		if (end)
		{
			int length = end - tmpString;

			strncpy(tmpBuf, tmpString, length);
			tmpBuf[length] = 0;
			if (length)
			{
				argArray->addString(tmpBuf);
			}
			tmpString += length + 1;
			if (end[0] == '"' && tmpString[0])
			{
				tmpString++;
			}
		}
		else
		{
			if (strlen(tmpString))
			{
				argArray->addString(tmpString);
			}
			tmpString += strlen(tmpString);
		}
	}
	getCurrentUserDefaults()->defSetCommandLine(argArray);
	argArray->release();
}

void TCUserDefaults::setAppName(const char* value)
{
	getCurrentUserDefaults()->defSetAppName(value);
}

const char* TCUserDefaults::getAppName(void)
{
	return getCurrentUserDefaults()->defGetAppName();
}

void TCUserDefaults::setSessionName(const char* value, const char* saveKey,
									bool copyCurrent)
{
	getCurrentUserDefaults()->defSetSessionName(value, saveKey, copyCurrent);
}

const char* TCUserDefaults::getSessionName(void)
{
	return getCurrentUserDefaults()->defGetSessionName();
}

void TCUserDefaults::setStringForKey(const char* value, const char* key,
									 bool sessionSpecific)
{
	getCurrentUserDefaults()->defSetStringForKey(value, key, sessionSpecific);
}

char* TCUserDefaults::stringForKey(const char* key, const char* defaultValue,
								   bool sessionSpecific)
{
	return getCurrentUserDefaults()->defStringForKey(key, sessionSpecific,
		defaultValue);
}

void TCUserDefaults::setLongForKey(long value, const char* key,
								   bool sessionSpecific)
{
	getCurrentUserDefaults()->defSetLongForKey(value, key,
		sessionSpecific);
}

long TCUserDefaults::longForKey(const char* key, long defaultValue,
								bool sessionSpecific)
{
	return getCurrentUserDefaults()->defLongForKey(key, sessionSpecific,
		defaultValue);
}

void TCUserDefaults::setFloatForKey(float value, const char* key,
									bool sessionSpecific)
{
	char stringValue[128];

	sprintf(stringValue, "%.24g", value);
	setStringForKey(stringValue, key, sessionSpecific);
}

float TCUserDefaults::floatForKey(const char* key, float defaultValue,
								  bool sessionSpecific)
{
	char *stringValue = stringForKey(key, NULL, sessionSpecific);
	float returnValue = defaultValue;

	if (stringValue)
	{
		if (sscanf(stringValue, "%g", &returnValue) != 1)
		{
			returnValue = defaultValue;
		}
		delete stringValue;
	}
	return returnValue;
}

void TCUserDefaults::removeValue(const char* key, bool sessionSpecific)
{
	getCurrentUserDefaults()->defRemoveValue(key, sessionSpecific);
}

void TCUserDefaults::defSaveSessionNameInKey(const char* key)
{
	char *savedSessionName = copyString(sessionName);

	if (savedSessionName)
	{
//		defSetSessionName(NULL);
		defSetStringForKey(savedSessionName, key, false);
//		defSetSessionName(savedSessionName);
		delete savedSessionName;
	}
	else
	{
		defRemoveValue(key, false);
	}
}

char* TCUserDefaults::defGetSavedSessionNameFromKey(const char* key)
{
	char *savedSessionName;
//	char *currentSessionName = copyString(sessionName);

//	defSetSessionName(NULL);
	savedSessionName = defStringForKey(key, false);
//	defSetSessionName(currentSessionName);
//	delete currentSessionName;
	return savedSessionName;
}

void TCUserDefaults::defRemoveSession(const char *value)
{
#ifdef _QT
	char sessionKey[1024];

	sprintf(sessionKey, "/%s/Sessions/%s", appName, value);
	deleteSubkeys(sessionKey);
#endif // _QT
#if defined(__APPLE__) && !defined(_QT)
	[[NSUserDefaults standardUserDefaults] removePersistentDomainForName:
		getSessionKey(value)];
	if (strcmp(sessionName, value) == 0)
	{
		[sessionDict removeAllObjects];
	}
#endif // __APPLE__ && !_QT
#ifdef WIN32
	HKEY hSessionsKey = openKeyPathUnderKey(hAppDefaultsKey, "Sessions");

	if (hSessionsKey)
	{
		HKEY hDelKey = openKeyPathUnderKey(hSessionsKey, value);

		if (hDelKey)
		{
			deleteSubKeys(hDelKey);
			RegCloseKey(hDelKey);
			RegDeleteKey(hSessionsKey, value);
		}
	}
	RegCloseKey(hSessionsKey);
#endif // WIN32
}

void TCUserDefaults::defSetStringForKey(const char* value, const char* key,
										bool sessionSpecific)
{
#ifdef _QT
	qSettings->writeEntry(qKeyForKey(key, sessionSpecific), value);
#endif // _QT
#if defined(__APPLE__) && !defined(_QT)
	NSString *nsKey = [NSString stringWithCString: key encoding:
		NSASCIIStringEncoding];

	if (sessionDict)
	{
		[sessionDict setObject: [NSString stringWithCString: value encoding:
			NSASCIIStringEncoding] forKey: nsKey];
		[[NSUserDefaults standardUserDefaults] setPersistentDomain: sessionDict
			forName: getSessionKey()];
	}
	else
	{
		[[NSUserDefaults standardUserDefaults] setObject:
			[NSString stringWithCString: value encoding: NSASCIIStringEncoding]
			forKey: nsKey];
	}
#endif // __APPLE__ && !_QT
#ifdef WIN32
	defSetValueForKey((LPBYTE)value, strlen(value) + 1, REG_SZ, key,
		sessionSpecific);
#endif // WIN32
	sendValueChangedAlert(key);
}

int TCUserDefaults::defCommandLineIndexForKey(const char *key)
{
	if (commandLine)
	{
		int i;
		int count = commandLine->getCount();
		int keyLength = strlen(key);
		char *keyEquals = new char[keyLength + 3];
		int returnValue = -1;

		strcpy(keyEquals, "-");
		strcat(keyEquals, key);
		strcat(keyEquals, "=");
		for (i = 0; i < count && returnValue == -1; i++)
		{
			char *arg = commandLine->stringAtIndex(i);

			if (stringHasCaseInsensitivePrefix(arg, keyEquals))
			{
				returnValue = i;
			}
		}
		delete keyEquals;
		return returnValue;
	}
	return -1;
}

char* TCUserDefaults::defCommandLineStringForKey(const char* key)
{
	if (commandLine)
	{
		int index = defCommandLineIndexForKey(key);

		if (index >= 0)
		{
			char *arg = commandLine->stringAtIndex(index);

			arg += strlen(key) + 2;
			if (strlen(arg) > 0)
			{
				int argLen = strlen(arg);

				if (arg[0] == '"' && arg[argLen - 1] == '"')
				{
					char *retValue = copyString(arg + 1);

					retValue[argLen - 2] = 0;
					return retValue;
				}
				else
				{
					return copyString(arg);
				}
			}
		}
	}
	return NULL;
}

char* TCUserDefaults::defStringForKey(const char* key, bool sessionSpecific,
									  const char* defaultValue)
{
	char *commandLineValue = defCommandLineStringForKey(key);

	if (commandLineValue)
	{
		return commandLineValue;
	}
#ifdef _QT
	QString string = qSettings->readEntry(qKeyForKey(key, sessionSpecific),
		defaultValue);

	if (string == QString::null)
	{
		return NULL;
	}
	else
	{
		char *returnValue = new char[string.length() + 1];

		strcpy(returnValue, string);
		return returnValue;
	}
#endif // _QT
#if defined(__APPLE__) && !defined(_QT)
	NSString *returnString;
	NSString *nsKey = [NSString stringWithCString: key encoding:
		NSASCIIStringEncoding];

	if (sessionDict)
	{
		returnString = [sessionDict objectForKey: nsKey];
	}
	else
	{
		returnString = [[NSUserDefaults standardUserDefaults] objectForKey:
			nsKey];
	}
	if ([returnString isKindOfClass: [NSString class]])
	{
		return copyString([returnString cStringUsingEncoding:
			NSASCIIStringEncoding]);
	}
	else
	{
		return copyString(defaultValue);
	}
#endif // __APPLE__ && !_QT
#ifdef WIN32
	DWORD size;
	LPBYTE value = defValueForKey(size, REG_SZ, key, sessionSpecific);

	if (value)
	{
		return (char*)value;
	}
	else
	{
		return copyString(defaultValue);
	}
#endif // WIN32
}

void TCUserDefaults::sendValueChangedAlert(const char *key)
{
	TCAlert *alert = new TCAlert(TCUserDefaults::alertClass(), key);

	TCAlertManager::sendAlert(alert);
	alert->release();
}

void TCUserDefaults::defSetLongForKey(long value, const char* key,
									  bool sessionSpecific)
{
#ifdef _QT
	qSettings->writeEntry(qKeyForKey(key, sessionSpecific), (int)value);
#endif // _QT
#if defined(__APPLE__) && !defined(_QT)
	NSString *nsKey = [NSString stringWithCString: key encoding:
		NSASCIIStringEncoding];
	NSNumber *numberValue = [NSNumber numberWithLong: value];

	if (sessionDict)
	{
		[sessionDict setObject: numberValue forKey: nsKey];
		[[NSUserDefaults standardUserDefaults] setPersistentDomain: sessionDict
			forName: getSessionKey()];
	}
	else
	{
		[[NSUserDefaults standardUserDefaults] setObject: numberValue forKey:
			nsKey];
	}
#endif // __APPLE__ && !_QT
#ifdef WIN32
	defSetValueForKey((LPBYTE)&value, sizeof value, REG_DWORD, key,
		sessionSpecific);
#endif // WIN32
	sendValueChangedAlert(key);
}

long TCUserDefaults::defLongForKey(const char* key, bool sessionSpecific,
								   long defaultValue)
{
	char *commandLineValue = defCommandLineStringForKey(key);

	if (commandLineValue)
	{
		long returnValue;

		if (sscanf(commandLineValue, "%li", &returnValue) == 1)
		{
			delete commandLineValue;
			return returnValue;
		}
		delete commandLineValue;
	}
#ifdef _QT
	return qSettings->readNumEntry(qKeyForKey(key, sessionSpecific),
		defaultValue);
#endif // _QT
#if defined(__APPLE__) && !defined(_QT)
	NSNumber *returnNumber;
	NSString *nsKey = [NSString stringWithCString: key encoding:
		NSASCIIStringEncoding];

	if (sessionDict)
	{
		returnNumber = [sessionDict objectForKey: nsKey];
	}
	else
	{
		returnNumber = [[NSUserDefaults standardUserDefaults] objectForKey:
			nsKey];
	}
	if ([returnNumber isKindOfClass: [NSNumber class]])
	{
		return [returnNumber longValue];		
	}
	else
	{
		return defaultValue;
	}
#endif // __APPLE__ && !_QT
#ifdef WIN32
	DWORD size;
	LPBYTE value = defValueForKey(size, REG_DWORD, key, sessionSpecific);

	if (value)
	{
		long returnValue;

		memcpy(&returnValue, value, sizeof returnValue);
		delete value;
		return returnValue;
	}
	else
	{
		return defaultValue;
	}
#endif // WIN32
}

void TCUserDefaults::defRemoveValue(const char* key, bool sessionSpecific)
{
#ifdef _QT
	qSettings->removeEntry(qKeyForKey(key, sessionSpecific));
#endif // _QT
#if defined(__APPLE__) && !defined(_QT)
	NSString *nsKey = [NSString stringWithCString: key encoding:
		NSASCIIStringEncoding];

	if (sessionDict)
	{
		[sessionDict removeObjectForKey: nsKey];
		[[NSUserDefaults standardUserDefaults] setPersistentDomain: sessionDict
			forName: getSessionKey()];
	}
	else
	{
		[[NSUserDefaults standardUserDefaults] removeObjectForKey: nsKey];
	}
#endif // __APPLE__ && !_QT
#ifdef WIN32
	HKEY hParentKey;

	if (sessionSpecific)
	{
		hParentKey = hSessionKey;
	}
	else
	{
		hParentKey = hAppDefaultsKey;
	}
	if (hParentKey)
	{
		const char* spot;

		if ((spot = strrchr(key, '/')) != NULL)
		{
			int subKeyLength = spot - key;
			char* keyPath = strncpy(new char[subKeyLength + 1], key, subKeyLength);

			keyPath[subKeyLength] = 0;
			hParentKey = openKeyPathUnderKey(hParentKey, keyPath, true);
			delete keyPath;
			spot++;
		}
		else
		{
			spot = key;
		}
		RegDeleteValue(hParentKey, spot);
		if (hParentKey != hSessionKey && hParentKey != hAppDefaultsKey)
		{
			RegCloseKey(hParentKey);
		}
	}
#endif // WIN32
}

TCStringArray* TCUserDefaults::defGetProcessedCommandLine(void)
{
	return commandLine;
}

TCStringArray* TCUserDefaults::defGetAllKeys(void)
{
	TCStringArray *allKeys = new TCStringArray;

#ifdef _QT
	defGetAllKeysUnderKey(qKeyForKey("", true), allKeys);
#endif // _QT
#if defined(__APPLE__) && !defined(_QT)
	NSArray *nsAllKeys;
	int i;
	int count;
	
	if (sessionDict)
	{
		nsAllKeys = [sessionDict allKeys];
	}
	else
	{
		// We have to synchronize before we read things, because
		// otherwise the non-session values won't get flushed into the
		// main app's persistent domain.
		[[NSUserDefaults standardUserDefaults] synchronize];
		nsAllKeys = [[[NSUserDefaults standardUserDefaults]
			persistentDomainForName: [NSString stringWithCString:
			appName encoding: NSASCIIStringEncoding]] allKeys];
	}
	count = [nsAllKeys count];
	for (i = 0; i < count; i++)
	{
		allKeys->addString([[nsAllKeys objectAtIndex: i]
			cStringUsingEncoding: NSASCIIStringEncoding]);
	}
#endif // __APPLE__ && !_QT
#ifdef WIN32
	defGetAllKeysUnderKey(hSessionKey, "", allKeys);
#endif // WIN32
	return allKeys;
}

TCStringArray* TCUserDefaults::defGetAllSessionNames(void)
{
	TCStringArray *allSessionNames = new TCStringArray;

#ifdef _QT
	char key[1024];
	QStringList subkeyList;
	int i;
	int count;
	
	sprintf(key, "/%s/Sessions/", appName);
	subkeyList = qSettings->subkeyList(key);
	count = subkeyList.count();
	for (i = 0; i < count; i++)
	{
		allSessionNames->addString(subkeyList[i]);
	}
#endif // _QT
#if defined(__APPLE__) && !defined(_QT)
	NSArray *domainNames = [[NSUserDefaults standardUserDefaults]
		persistentDomainNames];
	NSString *prefix = getSessionKey("");
	int prefixLength = [prefix length];
	int i;
	int count;

	count = [domainNames count];
	for (i = 0; i < count; i++)
	{
		NSString *domainName = [domainNames objectAtIndex: i];
		if (![domainName isEqualToString: prefix] &&
			[domainName hasPrefix: prefix])
		{
			NSString *sessionName = [domainName substringFromIndex:
				prefixLength];
			allSessionNames->addString([sessionName cStringUsingEncoding:
				NSASCIIStringEncoding]);
		}
	}
#endif // __APPLE__ && !_QT
#ifdef WIN32
	HKEY hSessionsKey = openKeyPathUnderKey(hAppDefaultsKey, "Sessions");
	if (hSessionsKey)
	{
		int i;
		int status = ERROR_SUCCESS;

		for (i = 0; status == ERROR_SUCCESS; i++)
		{
			char keyName[1024];
			DWORD keyNameLen = sizeof(keyName);

			if ((status = RegEnumKey(hSessionsKey, i, keyName, keyNameLen)) ==
				ERROR_SUCCESS)
			{
				HKEY hSubKey;

				if (RegOpenKey(hSessionsKey, keyName, &hSubKey) == ERROR_SUCCESS)
				{
					allSessionNames->addString(keyName);
					RegCloseKey(hSubKey);
				}
			}
		}
		RegCloseKey(hSessionsKey);
	}
#endif // WIN32
	return allSessionNames;
}

void TCUserDefaults::defSetCommandLine(TCStringArray *argArray)
{
	int i, j;
	int count = argArray->getCount();

	if (commandLine)
	{
		commandLine->release();
	}
	commandLine = new TCStringArray;
	for (i = 0, j = 0; i < count; i++)
	{
		char *arg = argArray->stringAtIndex(i);

		if (i > 0 && strcmp(arg, "=") == 0)
		{
			commandLine->appendString("=", j - 1);
		}
		else if (i > 0 && stringHasSuffix(commandLine->stringAtIndex(j - 1),
			"="))
		{
			commandLine->appendString(arg, j - 1);
		}
		else
		{
			commandLine->addString(arg);
			j++;
		}
	}
}

void TCUserDefaults::defSetAppName(const char* value)
{
	if (appName != value)
	{
		delete appName;
		appName = copyString(value);
		defSetSessionName(NULL, NULL, false);
#ifdef WIN32
		if (hAppDefaultsKey)
		{
			RegCloseKey(hAppDefaultsKey);
		}
		hAppDefaultsKey = openAppDefaultsKey();
		hSessionKey = hAppDefaultsKey;
#endif // WIN32
#if defined(__APPLE__) && !defined(_QT)
		initSessionDict();
#endif // __APPLE__ && !_QT
	}
}

void TCUserDefaults::defSetSessionName(const char* value, const char *saveKey,
									   bool copyCurrent)
{
	if (value != sessionName)
	{
#ifdef _QT
		char key[1024];
		QStringList sessionNames;
		
		sprintf(key, "/%s/Sessions/", appName);
		sessionNames = qSettings->subkeyList(key);
		if (value && sessionNames.findIndex(value) == -1)
		{
			char srcKey[1024];
			char dstKey[1024];

			sprintf(dstKey, "%s%s", key, value);
			if (sessionName)
			{
				sprintf(srcKey, "%s%s", key, sessionName);
			}
			else
			{
				sprintf(srcKey, "/%s/", appName);
			}
			copyTree(dstKey, srcKey, key);
		}
		delete sessionName;
		sessionName = copyString(value);
#endif // _QT
#if defined(__APPLE__) && !defined(_QT)
		sessionName = copyString(value);
		if ([[NSUserDefaults standardUserDefaults]
			persistentDomainForName: getSessionKey()] == nil)
		{
			// The new session doesn't exist yet, so copy the current session
			// into it.  Note that if the current session is already in
			// sessionDict, we can just continue to use that.  Otherwise,
			// we need to create a new sessionDict.
			if (!sessionDict)
			{
				// We have to synchronize before we read things, because
				// otherwise the non-session values won't get flushed into the
				// main app's persistent domain.
				[[NSUserDefaults standardUserDefaults] synchronize];
				sessionDict = [[[NSUserDefaults standardUserDefaults]
					persistentDomainForName: [NSString stringWithCString:
					appName encoding: NSASCIIStringEncoding]] mutableCopy];
			}
			[[NSUserDefaults standardUserDefaults] setPersistentDomain:
				sessionDict forName: getSessionKey()];
		}
		else
		{
			initSessionDict();
		}
#endif // __APPLE__ && !_QT
#ifdef WIN32
		HKEY hOldSessionKey = hSessionKey;

		delete sessionName;
		sessionName = copyString(value);
		if (sessionName && appName)
		{
			hSessionKey = openSessionKey();
			if (!hSessionKey)
			{
				hSessionKey = hOldSessionKey;
				if (copyCurrent)
				{
					TCStringArray *allKeys;
					int i;
					int count;
					char *sessionPrefix = new char[strlen(sessionName) + 128];

					sprintf(sessionPrefix, "Sessions/%s/", sessionName);
					allKeys = defGetAllKeys();
					count = allKeys->getCount();
					for (i = 0; i < count; i++)
					{
						char *key = allKeys->stringAtIndex(i);
						char *newKey = new char[strlen(sessionPrefix) +
							strlen(key) + 4];

						sprintf(newKey, "%s%s", sessionPrefix, key);
						if (defIsLongKey(key, true))
						{
							long value = defLongForKey(key, true);

							hSessionKey = hAppDefaultsKey;
							setLongForKey(value, newKey);
						}
						else
						{
							char *value = defStringForKey(key, true);

							hSessionKey = hAppDefaultsKey;
							setStringForKey(value, newKey);
							delete value;
						}
						hSessionKey = hOldSessionKey;
						delete newKey;
					}
					allKeys->release();
					delete sessionPrefix;
				}
				else
				{
					char *dummyKeyName = new char[strlen(sessionName) + 128];

					sprintf(dummyKeyName, "Sessions/%s/dummy", sessionName);
					setLongForKey(1, dummyKeyName);
					removeValue(dummyKeyName);
					delete dummyKeyName;
				}
				hSessionKey = openSessionKey();
			}
		}
		else
		{
			hSessionKey = hAppDefaultsKey;
		}
		if (hOldSessionKey && hOldSessionKey != hAppDefaultsKey &&
			hOldSessionKey != hSessionKey)
		{
			RegCloseKey(hOldSessionKey);
		}
#endif // WIN32
	}
	if (saveKey)
	{
		defSaveSessionNameInKey(saveKey);
	}
}

#ifdef WIN32

void TCUserDefaults::defSetValueForKey(const LPBYTE value, int length,
									   DWORD type, const char* key,
									   bool sessionSpecific)
{
	int index = defCommandLineIndexForKey(key);
	HKEY hParentKey;

	if (index >= 0)
	{
		commandLine->removeString(index);
	}
	if (sessionSpecific)
	{
		hParentKey = hSessionKey;
	}
	else
	{
		hParentKey = hAppDefaultsKey;
	}
	if (hParentKey)
	{
		const char* spot;

		if ((spot = strrchr(key, '/')) != NULL)
		{
			int subKeyLength = spot - key;
			char* keyPath = strncpy(new char[subKeyLength + 1], key, subKeyLength);

			keyPath[subKeyLength] = 0;
			hParentKey = openKeyPathUnderKey(hParentKey, keyPath, true);
			spot++;
			delete keyPath;
		}
		else
		{
			spot = key;
		}
		RegSetValueEx(hParentKey, spot, 0, type, value, length);
		if (hParentKey != hSessionKey && hParentKey != hAppDefaultsKey)
		{
			RegCloseKey(hParentKey);
		}
	}
}

LPBYTE TCUserDefaults::defValueForKey(DWORD& size, DWORD type, const char* key,
									  bool sessionSpecific)
{
	HKEY hParentKey;

	if (sessionSpecific)
	{
		hParentKey = hSessionKey;
	}
	else
	{
		hParentKey = hAppDefaultsKey;
	}
	if (hParentKey)
	{
		DWORD valueType;
		const char* spot;
		LPBYTE value = NULL;

		if ((spot = strrchr(key, '/')) != NULL)
		{
			int subKeyLength = spot - key;
			char* keyPath = strncpy(new char[subKeyLength + 1], key, subKeyLength);

			keyPath[subKeyLength] = 0;
			hParentKey = openKeyPathUnderKey(hParentKey, keyPath, true);
			delete keyPath;
			spot++;
		}
		else
		{
			spot = key;
		}
		size = 0;
		if (RegQueryValueEx(hParentKey, spot, 0, &valueType, NULL,
			&size) == ERROR_SUCCESS)
		{
			if (valueType == type)
			{
				value = new BYTE[size];
				if (RegQueryValueEx(hParentKey, spot, 0, &valueType,
					value, &size) != ERROR_SUCCESS)
				{
					delete value;
					value = NULL;
				}
			}
		}
		if (hParentKey != hSessionKey && hParentKey != hAppDefaultsKey)
		{
			RegCloseKey(hParentKey);
		}
		return value;
	}
	return NULL;
}

void TCUserDefaults::defGetAllKeysUnderKey(HKEY parentKey, const char* keyPath,
										   TCStringArray* allKeys)
{
	int i;
	long status = ERROR_SUCCESS;

	for (i = 0; status == ERROR_SUCCESS; i++)
	{
		char valueName[1024];
		DWORD valueNameLen = sizeof(valueName);
 
		if ((status = RegEnumValue(parentKey, i, valueName, &valueNameLen, NULL,
			NULL, NULL, NULL)) == ERROR_SUCCESS)
		{
			char *keyName = new char[strlen(valueName) + strlen(keyPath) + 16];

			sprintf(keyName, "%s%s", keyPath, valueName);
			allKeys->addString(keyName);
			delete keyName;
		}
	}
	status = ERROR_SUCCESS;
	for (i = 0; status == ERROR_SUCCESS; i++)
	{
		char keyName[1024];
		DWORD keyNameLen = sizeof(keyName);

		if ((status = RegEnumKey(parentKey, i, keyName, keyNameLen)) ==
			ERROR_SUCCESS)
		{
			HKEY hSubKey;

			if (RegOpenKey(parentKey, keyName, &hSubKey) == ERROR_SUCCESS)
			{
				char *newPath = new char[strlen(keyName) + strlen(keyPath) + 4];

				sprintf(newPath, "%s%s/", keyPath, keyName);
				if (strcmp(newPath, "Sessions/") != 0)
				{
					// Don't return session keys.
					defGetAllKeysUnderKey(hSubKey, newPath, allKeys);
				}
				delete newPath;
				RegCloseKey(hSubKey);
			}
		}
	}
}

bool TCUserDefaults::defIsLongKey(const char *key, bool sessionSpecific)
{
	DWORD size;
	LPBYTE value = defValueForKey(size, REG_DWORD, key, sessionSpecific);

	if (value)
	{
		delete value;
		return true;
	}
	return false;
}

HKEY TCUserDefaults::openKeyPathUnderKey(HKEY parentKey, const char* keyPath,
										 bool create)
{
	int count;
	char** components = componentsSeparatedByString(keyPath, "/", count);
	HKEY currentKey = parentKey;
	DWORD disposition;
	int i;
	BOOL failed = NO;

	for (i = 0; i < count && !failed; i++)
	{
		char* component = components[i];

		if (strlen(component) > 0)
		{
			HKEY newKey;

			if (create)
			{
				if (RegCreateKeyEx(currentKey, component, 0, NULL, 0,
					KEY_WRITE | KEY_READ, NULL, &newKey, &disposition) !=
					ERROR_SUCCESS)
				{
					failed = YES;
				}
			}
			else
			{
				if (RegOpenKeyEx(currentKey, component, 0, KEY_WRITE | KEY_READ,
					&newKey) != ERROR_SUCCESS)
				{
					failed = YES;
				}
			}
			if (currentKey != parentKey)
			{
				RegCloseKey(currentKey);
			}
			currentKey = newKey;
		}
	}
	deleteStringArray(components, count);
	if (failed)
	{
		return NULL;
	}
	else
	{
		return currentKey;
	}
}

HKEY TCUserDefaults::openAppDefaultsKey(void)
{
//	HKEY softwareKey;
//	DWORD disposition;
//	HKEY returnValue = NULL;
	char* keyPath = new char[strlen(appName) + 128];
	HKEY retValue;

	sprintf(keyPath, "Software/%s", appName);
	retValue = openKeyPathUnderKey(HKEY_CURRENT_USER, keyPath, true);
	delete keyPath;
	return retValue;
}

HKEY TCUserDefaults::openSessionKey(void)
{
	char* keyPath = new char[strlen(appName) + strlen(sessionName) + 128];
	HKEY retValue;

	sprintf(keyPath, "Software/%s/Sessions/%s", appName, sessionName);
	retValue = openKeyPathUnderKey(HKEY_CURRENT_USER, keyPath, false);
	delete keyPath;
	return retValue;
}

void TCUserDefaults::deleteSubKeys(HKEY hKey)
{
	while (1)
	{
		char name[1024];
		HKEY hSubKey;

		// Since we're deleting all sub-keys, we always just want the first
		// one.
		if (RegEnumKey(hKey, 0, name, sizeof(name)) != ERROR_SUCCESS)
		{
			break;
		}
		if (RegOpenKey(hKey, name, &hSubKey) == ERROR_SUCCESS)
		{
			deleteSubKeys(hSubKey);
			RegCloseKey(hSubKey);
			RegDeleteKey(hKey, name);
		}
	}
}

#endif // WIN32


#ifdef _QT

char *TCUserDefaults::qKeyForKey(const char *key, bool sessionSpecific)
{
	if (sessionSpecific && sessionName)
	{
		sprintf(qKey, "/%s/Sessions/%s/%s", appName, sessionName, key);
	}
	else
	{
		sprintf(qKey, "/%s/%s", appName, key);
	}
	return qKey;
}

void TCUserDefaults::deleteSubkeys(const char *key)
{
	QStringList subkeyList = qSettings->subkeyList(key);
	QStringList entryList = qSettings->entryList(key);
	int i;
	int count = subkeyList.count();

	for (i = 0; i < count; i++)
	{
		char subkey[1024];

		sprintf(subkey, "%s/%s", key, (const char *)subkeyList[i]);
		deleteSubkeys(subkey);
	}
	count = entryList.count();
	for (i = 0; i < count; i++)
	{
		char subkey[1024];

		sprintf(subkey, "%s/%s", key, (const char *)entryList[i]);
		qSettings->removeEntry(subkey);
	}
}

void TCUserDefaults::defGetAllKeysUnderKey(const char *key,
										   TCStringArray *allKeys)
{
	QStringList subkeyList = qSettings->subkeyList(key);
	QStringList entryList = qSettings->entryList(key);
	int i;
	int count = subkeyList.count();

	for (i = 0; i < count; i++)
	{
		char subkey[1024];

		sprintf(subkey, "%s/%s", key, (const char *)subkeyList[i]);
		defGetAllKeysUnderKey(subkey, allKeys);
	}
	count = entryList.count();
	for (i = 0; i < count; i++)
	{
		allKeys->addString(subkeyList[i]);
	}
}

void TCUserDefaults::copyTree(const char *dstKey, const char *srcKey,
							  const char *skipKey)
{
	QStringList subkeyList = qSettings->subkeyList(srcKey);
	QStringList entryList = qSettings->entryList(srcKey);
	int i;
	int count = subkeyList.count();

	if (strcmp(dstKey, skipKey) == 0)
	{
		return;
	}
	for (i = 0; i < count; i++)
	{
		char srcSubkey[1024];
		char dstSubkey[1024];

		sprintf(srcSubkey, "%s/%s", srcKey, (const char *)subkeyList[i]);
		sprintf(dstSubkey, "%s/%s", dstKey, (const char *)subkeyList[i]);
		copyTree(dstSubkey, srcSubkey, skipKey);
	}
	count = entryList.count();
	for (i = 0; i < count; i++)
	{
		char srcSubKey[1024];
		char dstSubKey[1024];

		sprintf(srcSubKey, "%s/%s", srcKey, (const char *)entryList[i]);
		sprintf(dstSubKey, "%s/%s", dstKey, (const char *)entryList[i]);
		qSettings->writeEntry(dstSubKey, qSettings->readEntry(srcSubKey));
	}
}

#endif // _QT

#if defined(__APPLE__) && !defined(_QT)

NSString *TCUserDefaults::getSessionKey(const char *key)
{
	if (key == NULL)
	{
		key = sessionName;
	}
	if (key)
	{
		return [NSString stringWithFormat: @"%s.Session.%s", appName, key];
	}
	else
	{
		return [NSString stringWithFormat: @"%s", appName];
	}
}

void TCUserDefaults::initSessionDict(void)
{
	if (sessionDict)
	{
		[sessionDict release];
		sessionDict = nil;
	}
	if (sessionName)
	{
		sessionDict = [[[NSUserDefaults standardUserDefaults]
			persistentDomainForName: getSessionKey()] mutableCopy];
		if (!sessionDict)
		{
			sessionDict = [[NSMutableDictionary alloc] init];
		}
	}
}

#endif // __APPLE__ && !_QT
