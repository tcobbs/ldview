#ifndef __TCARRAY_H__
#define __TCARRAY_H__

#include <TCFoundation/TCObject.h>

typedef int (*TCArraySortFunction)(const void*, const void*);

class TCExport TCArray : public TCObject
{
	public:
		explicit TCArray(unsigned int = 0);

		virtual void addItem(void*);
		virtual void insertItem(void*, unsigned int = 0);
		virtual int replaceItem(void*, unsigned int);
		virtual int indexOfItem(void*);
		virtual int removeItem(void*);
		virtual int removeItem(int);
		virtual void removeAll(void);
		virtual void* itemAtIndex(unsigned int);
		int getCount(void) { return count; }
		virtual int setCapacity(unsigned);
		virtual void sortUsingFunction(TCArraySortFunction function);
		void **getItems(void) { return items; }
		virtual TCObject *copy(void);
	protected:
		virtual ~TCArray(void);
		virtual void dealloc(void);

		void** items;
		unsigned int count;
		unsigned int allocated;
};

#endif // __TCARRAY_H__
