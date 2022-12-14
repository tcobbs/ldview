#ifndef __TCSTRINGARRAY_H__
#define __TCSTRINGARRAY_H__

#include <TCFoundation/TCArray.h>

#if defined(__APPLE__)
#	define stricmp strcasecmp
#endif

typedef char ** TCCharStarStar;

class TCExport TCStringArray : public TCArray<>
{
	public:
		explicit TCStringArray(size_t = 0, bool = true);
		TCStringArray(const TCCharStarStar items, size_t numItems,
			bool caseSensitive = true);

		virtual size_t addString(const char*);
		virtual void insertString(const char*, size_t = 0);
		virtual bool replaceString(const char*, size_t);
		virtual bool appendString(const char*, size_t);
		virtual ptrdiff_t indexOfString(const char*);
		virtual bool removeString(const char*);
		virtual bool removeStringAtIndex(size_t);
		virtual void removeAll(void);
		virtual const char* stringAtIndex(size_t) const;
		virtual char* stringAtIndex(size_t);
		virtual char* operator[](size_t);
		virtual const char* operator[](size_t) const;
		virtual bool readFile(const char*);
		bool isCaseSensitive(void) { return caseSensitive; }
		virtual TCObject *copy(void) const;
	protected:
		virtual ~TCStringArray(void);
		virtual void dealloc(void);
		virtual void copyContents(TCStringArray *otherStringArray) const;

		bool caseSensitive;
};

#endif // __TCSTRINGARRAY_H__
