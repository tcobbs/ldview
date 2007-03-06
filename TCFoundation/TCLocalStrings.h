#ifndef __TCLOCALSTRINGS_H__
#define __TCLOCALSTRINGS_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCStlIncludes.h>

#ifdef _QT
#include <qstring.h>
#include <qtextcodec.h>

typedef std::map<QString, QString> QStringQStringMap;
#endif // _QT

class TCDictionary;
typedef std::map<std::wstring, std::wstring> WStringWStringMap;
typedef std::map<std::string, std::string> StringStringMap;
typedef std::map<int, wchar_t *> IntWCharMap;

class TCExport TCLocalStrings: public TCObject
{
public:
	static bool setStringTable(const char *stringTable, bool replace = true);
	static bool setStringTable(const wchar_t *stringTable, bool replace = true);
	static bool loadStringTable(const char *filaname, bool replace = true);
#ifndef WIN32
	static const QString &get(const char *key);
#else // WIN32
	static const char *get(const char *key);
#endif // WIN32
	static const char *getUtf8(const char *key);
	static const wchar_t *get(const wchar_t *key);
	static void dumpTable(const char *filename, const char *header);
protected:
	TCLocalStrings(void);
	virtual ~TCLocalStrings(void);
	virtual void dealloc(void);
	bool instSetStringTable(const char *stringTable, bool replace);
	bool instSetStringTable(const wchar_t *stringTable, bool replace);
#ifndef WIN32
	const QString &instGetLocalString(const char *key);
#else // WIN32
	const char *instGetLocalString(const char *key);
#endif // WIN32
	const char *instGetUtf8LocalString(const char *key);
	const wchar_t *instGetLocalString(const wchar_t *key);
	void instDumpTable(const char *filename, const char *header);
	void instSetCodePage(int codePage);
	void mbstowstring(std::wstring &dst, const char *src, int length = -1);

	TCDictionary *stringDict;
	WStringWStringMap m_strings;
	StringStringMap m_utf8Strings;
#ifndef WIN32
	QStringQStringMap m_qStrings;
	QString m_emptyQString;
	QTextCodec *m_textCodec;

	void buildQStringMap(void);
#endif // WIN32

	int m_codePage;

	static TCLocalStrings *currentLocalStrings;
	static TCLocalStrings *getCurrentLocalStrings(void);

	static class TCLocalStringsCleanup
	{
	public:
		TCLocalStringsCleanup(void);
		~TCLocalStringsCleanup(void);
	} localStringsCleanup;

	static IntWCharMap sm_codePages;
	static void initCodePages(void);
	friend class TCLocalStringsCleanup;
};

#endif // __TCLOCALSTRINGS_H__
