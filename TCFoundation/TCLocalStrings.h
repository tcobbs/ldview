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

class TCExport TCLocalStrings: public TCObject
{
public:
	static bool setStringTable(const char *stringTable, bool replace = true);
	static bool setStringTable(const wchar_t *stringTable, bool replace = true);
	static bool loadStringTable(const char *filaname, bool replace = true);
#ifdef _QT
	static const QString &get(const char *key);
#else // _QT
	static const char *get(const char *key);
#endif // _QT
	static const wchar_t *get(const wchar_t *key);
	static void dumpTable(const char *filename, const char *header);
protected:
	TCLocalStrings(void);
	virtual ~TCLocalStrings(void);
	virtual void dealloc(void);
	bool instSetStringTable(const char *stringTable, bool replace);
	bool instSetStringTable(const wchar_t *stringTable, bool replace);
#ifdef _QT
	const QString &instGetLocalString(const char *key);
#else // _QT
	const char *instGetLocalString(const char *key);
#endif // _QT
	const wchar_t *instGetLocalString(const wchar_t *key);
	void instDumpTable(const char *filename, const char *header);
	void instSetCodePage(int codePage);
	void mbstowstring(std::wstring &dst, const char *src, int length = -1);

	TCDictionary *stringDict;
	WStringWStringMap m_strings;
#ifdef _QT
	QStringQStringMap m_qStrings;
	QString m_emptyQString;
	QTextCodec *m_textCodec;

	void buildQStringMap(void);
#endif // _QT

	int m_codePage;

	static TCLocalStrings *currentLocalStrings;
	static TCLocalStrings *getCurrentLocalStrings(void);

	static class TCLocalStringsCleanup
	{
	public:
		~TCLocalStringsCleanup(void);
	} localStringsCleanup;
	friend class TCLocalStringsCleanup;
};

#endif // __TCLOCALSTRINGS_H__
