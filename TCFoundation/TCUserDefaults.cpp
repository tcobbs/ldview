#include <stdio.h>

#include "TCUserDefaults.h"
#include "TCStringArray.h"
#include "mystring.h"

TCUserDefaults* TCUserDefaults::currentUserDefaults = NULL;

TCUserDefaults::TCUserDefaultsCleanup TCUserDefaults::userDefaultsCleanup;

TCUserDefaults::TCUserDefaultsCleanup::~TCUserDefaultsCleanup(void)
{
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
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TCUserDefaults");
#endif
#ifdef _QT
	qSettings = new QSettings;
#endif // _QT
}

void TCUserDefaults::dealloc(void)
{
#ifdef _QT
	delete qSettings;
#endif // _QT
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
		char *end = NULL;

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
				char *equals = strchr(tmpString, '=');

				if (equals < end && equals[1] == '"')
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
			argArray->addString(tmpBuf);
			tmpString += length + 1;
			if (end[0] == '"' && tmpString[0])
			{
				tmpString++;
			}
		}
		else
		{
			argArray->addString(tmpString);
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

void TCUserDefaults::setSessionName(const char* value, const char* saveKey)
{
	getCurrentUserDefaults()->defSetSessionName(value, saveKey);
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
#ifdef WIN32
	defSetValueForKey((LPBYTE)value, strlen(value) + 1, REG_SZ, key,
		sessionSpecific);
#else // WIN32
#ifdef _QT
	qSettings->writeEntry(qKeyForKey(key), value);
#endif // _QT
#endif // !WIN32
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
#else // WIN32
#ifdef _QT
	QString string = qSettings->readEntry(qKeyForKey(key), defaultValue);

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
#else // _QT
	return copyString(defaultValue);
#endif // !_QT
#endif // !WIN32
}

void TCUserDefaults::defSetLongForKey(long value, const char* key,
									  bool sessionSpecific)
{
#ifdef WIN32
	defSetValueForKey((LPBYTE)&value, sizeof value, REG_DWORD, key,
		sessionSpecific);
#else // WIN32
#ifdef _QT
	qSettings->writeEntry(qKeyForKey(key), (int)value);
#endif // _QT
#endif // !WIN32
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
	return qSettings->readNumEntry(qKeyForKey(key), defaultValue);
#endif // _QT
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
	return defaultValue;
}

void TCUserDefaults::defRemoveValue(const char* key, bool sessionSpecific)
{
#ifdef _QT
	qSettings->removeEntry(key);
#endif // _QT
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
#else // WIN32
#endif // WIN32
}

TCStringArray* TCUserDefaults::defGetProcessedCommandLine(void)
{
	return commandLine;
}

TCStringArray* TCUserDefaults::defGetAllKeys(void)
{
	TCStringArray *allKeys = new TCStringArray;

#ifdef WIN32
	defGetAllKeysUnderKey(hSessionKey, "", allKeys);
#endif
	return allKeys;
}

TCStringArray* TCUserDefaults::defGetAllSessionNames(void)
{
	TCStringArray *allSessionNames = new TCStringArray;

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
#endif
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
		defSetSessionName(NULL);
#ifdef WIN32
		if (hAppDefaultsKey)
		{
			RegCloseKey(hAppDefaultsKey);
		}
		hAppDefaultsKey = openAppDefaultsKey();
		hSessionKey = hAppDefaultsKey;
#endif // WIN32
	}
}

void TCUserDefaults::defSetSessionName(const char* value, const char *saveKey)
{
	if (value != sessionName)
	{
		delete sessionName;
		sessionName = copyString(value);
#ifdef WIN32
		HKEY hOldSessionKey = hSessionKey;

		if (sessionName && appName)
		{
			hSessionKey = openSessionKey();
			if (!hSessionKey)
			{
				TCStringArray *allKeys;
				int i;
				int count;
				char *sessionPrefix = new char[strlen(sessionName) + 128];

				sprintf(sessionPrefix, "Sessions/%s/", sessionName);
				hSessionKey = hOldSessionKey;
				allKeys = defGetAllKeys();
				count = allKeys->getCount();
				for (i = 0; i < count; i++)
				{
					char *key = allKeys->stringAtIndex(i);
					char *newKey = new char[strlen(sessionPrefix) + strlen(key) +
						4];

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
				delete sessionPrefix;
				allKeys->release();
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

char *TCUserDefaults::qKeyForKey(const char *key)
{
	sprintf(qKey, "/%s/%s", appName, key);
	return qKey;
}

#endif // _QT
