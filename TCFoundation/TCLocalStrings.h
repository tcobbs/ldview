#ifndef __TCLOCALSTRINGS_H__
#define __TCLOCALSTRINGS_H__

#include <TCFoundation/TCObject.h>

class TCDictionary;

class TCExport TCLocalStrings: public TCObject
{
	public:
		static bool setStringTable(const char *stringTable,
			bool replace = true);
		static bool loadStringTable(const char *filaname, bool replace = true);
		static const char *get(const char *key);
	protected:
		TCLocalStrings(void);
		virtual ~TCLocalStrings(void);
		virtual void dealloc(void);
		bool instSetStringTable(const char *stringTable, bool replace);
		const char *instGetLocalString(const char *key);

		TCDictionary *stringDict;

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
