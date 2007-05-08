#ifndef __TCUSERDEFAULTS_H__
#define __TCUSERDEFAULTS_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCStlIncludes.h>

//#ifdef WIN32
//#include <windows.h>
//#endif // WIN32
#ifdef _QT
#include <qsettings.h>
#include <stdlib.h>
#endif // _QT
#if defined(__APPLE__) && !defined(_QT)
#ifndef __OBJC__
// We want to use Objective-C stuff, but this file will be included from
// non-Objective-C files, so we need to make it work there too.
#define NSString void
#define NSMutableDictionary void
#endif // __OBJC__
#endif // __APPLE__ && !_QT

typedef std::vector<long> LongVector;

class TCStringArray;

class TCExport TCUserDefaults: public TCObject
{
	public:
		static void setStringForKey(const char* value,
			const char* key, bool sessionSpecific = true);
#ifndef TC_NO_UNICODE
		static void setStringForKey(CUCSTR value, const char* key,
			bool sessionSpecific = true);
#endif //TC_NO_UNICODE
		static char* stringForKey(const char* key,
			const char* defaultValue = NULL, bool sessionSpecific = true);
		static UCSTR stringForKeyUC(const char* key, CUCSTR defaultValue = NULL,
			bool sessionSpecific = true);
		static void setLongForKey(long value, const char* key,
			bool sessionSpecific = true);
		static long longForKey(const char* key, long defaultValue = 0,
			bool sessionSpecific = true);
		static void setLongVectorForKey(const LongVector &value,
			const char* key, bool sessionSpecific = true);
		static LongVector longVectorForKey(const char* key,
			const LongVector &defaultValue = LongVector(),
			bool sessionSpecific = true);
		static void setFloatForKey(float value, const char* key,
			bool sessionSpecific = true);
		static float floatForKey(const char* key, float defaultValue = 0.0f,
			bool sessionSpecific = true);
		static void removeValue(const char* key, bool sessionSpecific = true);
		static void setAppName(const char* value);
		static const char* getAppName(void);
		static void setSessionName(const char* value,
			const char* saveKey = NULL, bool copyCurrent = true);
		static const char* getSessionName(void);
		static void deleteResult(void* value);
		// The following would be const char *argv[], but for some reason the
		// non-const argv in main can't be auto-converted to the const version.
		static void setCommandLine(char *argv[]);
		static void setCommandLine(const char *args);
		static TCStringArray* getProcessedCommandLine(void);
		static const char* getArgv0(void) { return argv0; }
		static TCStringArray* getAllKeys(void);
		static TCStringArray* getAllSessionNames(void);
		static void saveSessionNameInKey(const char *key);
		static char* getSavedSessionNameFromKey(const char *key);
		static void removeSession(const char *value);
		static const char *alertClass(void) { return "TCUserDefaultsChanged"; }
		static void flush(void);
	protected:
		TCUserDefaults(void);
		virtual void dealloc(void);
		virtual ~TCUserDefaults(void);
		static TCUserDefaults* getCurrentUserDefaults(void);
		void defSetStringForKey(const char* value, const char* key,
			bool sessionSpecific);
#ifndef TC_NO_UNICODE
		void defSetStringForKey(CUCSTR value, const char* key,
			bool sessionSpecific);
#endif //TC_NO_UNICODE
		char* defStringForKey(const char* key, bool sessionSpecific,
			const char* defaultValue = NULL);
		UCSTR defStringForKeyUC(const char* key, bool sessionSpecific,
			CUCSTR defaultValue = NULL);
		void defSetLongForKey(long value, const char* key,
			bool sessionSpecific);
		long defLongForKey(const char* key, bool sessionSpecific,
			long defaultValue = 0, bool *found = NULL);
		void defSetLongVectorForKey(const LongVector &value,
			const char* key, bool sessionSpecific = true);
		LongVector defLongVectorForKey(const char* key,
			bool sessionSpecific,
			const LongVector &defaultValue = LongVector());
		void defRemoveValue(const char* key, bool sessionSpecific);
		void defSetAppName(const char* value);
		const char* defGetAppName(void) { return appName; }
		void defSetSessionName(const char* value, const char* saveKey,
			bool copyCurrent);
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
		void defFlush(void);
		void sendValueChangedAlert(const char *key);
		bool matchesCommandLine(const char *key, long value);
		bool matchesCommandLine(const char *key, const char *value);
		static std::string arrayKey(const char *key, int index);

#ifdef WIN32
		HKEY openAppDefaultsKey(void);
		HKEY openSessionKey(void);
		HKEY openKeyPathUnderKey(HKEY parentKey, const char* keyPath,
			bool create = false);
		void defSetValueForKey(const LPBYTE value, int length,
			DWORD type, const char* key, bool sessionSpecific,
			bool unicode = false);
		LPBYTE defValueForKey(DWORD& size, DWORD type,
			const char* key, bool sessionSpecific, bool unicode = false);
		void defGetAllKeysUnderKey(HKEY parentKey, const char* keyPath,
			TCStringArray *allKeys);
		bool defIsLongKey(const char* key, bool sessionSpecific);
		void deleteSubKeys(HKEY hKey);

		HKEY hAppDefaultsKey;
		HKEY hSessionKey;
#endif // WIN32
#ifdef _QT
		QSettings *qSettings;
		char qKey[1024];

		char *qKeyForKey(const char *key, bool sessionSpecific);
		void deleteSubkeys(const char *key);
		void defGetAllKeysUnderKey(const char *key, TCStringArray *allKeys);
		void copyTree(const char *dstKey, const char *srcKey,
			const char *skipKey);
#endif // _QT
		char* appName;
		char* sessionName;
		static TCUserDefaults* currentUserDefaults;
		TCStringArray* commandLine;

#if defined(__APPLE__) && !defined(_QT)
		NSMutableDictionary *sessionDict;

		NSString *getSessionKey(const char *key = NULL);
		void initSessionDict(void);
#endif // __APPLE__ && !_QT
		static class TCUserDefaultsCleanup
		{
		public:
			~TCUserDefaultsCleanup(void);
		} userDefaultsCleanup;
		static char *argv0;
		friend class TCUserDefaultsCleanup;
};

#endif // __TCUSERDEFAULTS_H__
