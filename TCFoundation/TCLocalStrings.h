#ifndef __TCLOCALSTRINGS_H__
#define __TCLOCALSTRINGS_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCStlIncludes.h>

class TCDictionary;
typedef std::map<std::wstring, std::wstring> WStringWStringMap;

class TCExport TCLocalStrings: public TCObject
{
	public:
		static bool setStringTable(const char *stringTable,
			bool replace = true);
		static bool loadStringTable(const char *filaname, bool replace = true);
		static const char *get(const char *key);
		static const wchar_t *get(const wchar_t *key);
	protected:
		TCLocalStrings(void);
		virtual ~TCLocalStrings(void);
		virtual void dealloc(void);
		bool instSetStringTable(const char *stringTable, bool replace);
		const char *instGetLocalString(const char *key);
		const wchar_t *instGetLocalString(const wchar_t *key);
		void mbstowstring(std::wstring &dst, const char *src, int length = -1);
		void wstringtostring(std::string &dst, const std::wstring &src);

		TCDictionary *stringDict;
		WStringWStringMap m_strings;

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
