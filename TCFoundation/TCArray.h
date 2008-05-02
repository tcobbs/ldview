#ifndef __TCARRAY_H__
#define __TCARRAY_H__

#include <TCFoundation/TCObject.h>

typedef int (*TCArraySortFunction)(const void*, const void*);
typedef void **VoidStarStar;

class TCExport TCArray : public TCObject
{
	public:
		explicit TCArray(unsigned int = 0);
		TCArray (const TCArray &other);

		virtual void addItem(void*);
		virtual void insertItem(void*, unsigned int = 0);
		virtual int replaceItem(void*, unsigned int);
		virtual int indexOfItem(void*) const;
		virtual int removeItem(void*);
		virtual int removeItem(int);
		virtual int removeItems(int index, int numItems);
		virtual void removeAll(void);
		virtual void* itemAtIndex(unsigned int);
		virtual const void* itemAtIndex(unsigned int) const;
		int getCount(void) const { return count; }
		virtual int setCapacity(unsigned);
		virtual void sortUsingFunction(TCArraySortFunction function);
		void **getItems(void) { return items; }
		const VoidStarStar getItems(void) const { return items; }
		virtual TCObject *copy(void) const;
	protected:
		virtual ~TCArray(void);
		virtual void dealloc(void);

		void** items;
		unsigned int count;
		unsigned int allocated;
};

#endif // __TCARRAY_H__
