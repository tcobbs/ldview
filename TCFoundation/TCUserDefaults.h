#ifndef __TCUSERDEFAULTS_H__
#define __TCUSERDEFAULTS_H__

#include <TCFoundation/TCObject.h>

#ifdef WIN32
#include <windows.h>
#else // WIN32
#ifdef _QT
#include <qsettings.h>
#endif // _QT
#endif // !WIN32

class TCStringArray;

class TCExport TCUserDefaults: public TCObject
{
	public:
		TCUserDefaults(void);
		static void setStringForKey(const char* value,
			const char* key, bool sessionSpecific = true);
		static char* stringForKey(const char* key,
			const char* defaultValue = NULL, bool sessionSpecific = true);
		static void setLongForKey(long value, const char* key,
			bool sessionSpecific = true);
		static long longForKey(const char* key, long defaultValue = 0,
			bool sessionSpecific = true);
		static void setFloatForKey(float value, const char* key,
			bool sessionSpecific = true);
		static float floatForKey(const char* key, float defaultValue = 0.0f,
			bool sessionSpecific = true);
		static void removeValue(const char* key, bool sessionSpecific = true);
		static void setAppName(const char* value);
		static const char* getAppName(void);
		static void setSessionName(const char* value,
			const char* saveKey = NULL);
		static const char* getSessionName(void);
		static void deleteResult(void* value);
		static void setCommandLine(char *argv[]);
		static void setCommandLine(const char *args);
		static TCStringArray* getProcessedCommandLine(void);
		static TCStringArray* getAllKeys(void);
		static TCStringArray* getAllSessionNames(void);
		static void saveSessionNameInKey(const char *key);
		static char* getSavedSessionNameFromKey(const char *key);
		static void removeSession(const char *value);
	protected:
		virtual void dealloc(void);
		virtual ~TCUserDefaults(void);
		static TCUserDefaults* getCurrentUserDefaults(void);
		void defSetStringForKey(const char* value, const char* key,
			bool sessionSpecific);
		char* defStringForKey(const char* key, bool sessionSpecific,
			const char* defaultValue = NULL);
		void defSetLongForKey(long value, const char* key,
			bool sessionSpecific);
		long defLongForKey(const char* key, bool sessionSpecific,
			long defaultValue = 0);
		void defRemoveValue(const char* key, bool sessionSpecific);
		void defSetAppName(const char* value);
		const char* defGetAppName(void) { return appName; }
		void defSetSessionName(const char* value, const char* saveKey = NULL);
		const char* defGetSessionName(void) { return sessionName; }
		void defSetCommandLine(TCStringArray *argArray);
		char* defCommandLineStringForKey(const char* key);
		TCStringArray* defGetProcessedCommandLine(void);
		TCStringArray* defGetAllKeys(void);
		TCStringArray* defGetAllSessionNames(void);
		int defCommandLineIndexForKey(const char *key);
		void defSaveSessionNameInKey(const char *key);
		char* defGetSavedSessionNameFromKey(const char *key);
		void defRemoveSession(const char *value);

#ifdef WIN32
		HKEY openAppDefaultsKey(void);
		HKEY openSessionKey(void);
		HKEY openKeyPathUnderKey(HKEY parentKey, const char* keyPath,
			bool create = false);
		void defSetValueForKey(const LPBYTE value, int length,
			DWORD type, const char* key, bool sessionSpecific);
		LPBYTE defValueForKey(DWORD& size, DWORD type,
			const char* key, bool sessionSpecific);
		void defGetAllKeysUnderKey(HKEY parentKey, const char* keyPath,
			TCStringArray *allKeys);
		bool defIsLongKey(const char* key, bool sessionSpecific);
		void deleteSubKeys(HKEY hKey);

		HKEY hAppDefaultsKey;
		HKEY hSessionKey;
#else // WIN32
#ifdef _QT
		QSettings *qSettings;
		char qKey[1024];

		char *qKeyForKey(const char *key);
#endif // _QT
#endif // !WIN32

		char* appName;
		char* sessionName;
		static TCUserDefaults* currentUserDefaults;
		TCStringArray* commandLine;

		static class TCUserDefaultsCleanup
		{
		public:
			~TCUserDefaultsCleanup(void);
		} userDefaultsCleanup;
		friend TCUserDefaultsCleanup;
};

#endif // __TCUSERDEFAULTS_H__
