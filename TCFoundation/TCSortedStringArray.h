#ifndef __TCSORTEDSTRINGARRAY_H__
#define __TCSORTEDSTRINGARRAY_H__

#include <TCFoundation/TCStringArray.h>

class TCExport TCSortedStringArray : public TCStringArray
{
	public:
		explicit TCSortedStringArray(size_t = 0, bool = 1);

		virtual size_t addString(const char*);
		virtual ptrdiff_t indexOfString(const char*);
		virtual TCObject *copy(void) const;
	protected:
		virtual ~TCSortedStringArray(void);
		virtual void insertString(const char*, size_t = 0);
		virtual bool replaceString(const char*, size_t);
};

#endif // __TCSORTEDSTRINGARRAY_H__
