#ifndef __TCOBJECTARRAY_H__
#define __TCOBJECTARRAY_H__

#include <TCFoundation/TCArray.h>

class TCExport TCObjectArray : public TCArray<>
{
public:
	explicit TCObjectArray(size_t = 0);
	TCObjectArray (const TCObjectArray &other);

	void addObject(TCObject*);
	void insertObject(TCObject*, size_t = 0);
	bool replaceObject(TCObject*, size_t);
	ptrdiff_t indexOfObject(TCObject*) const;
	ptrdiff_t indexOfObjectIdenticalTo(TCObject*) const;
	bool removeObject(TCObject*);
	bool removeObjectIdenticalTo(TCObject*);
	virtual bool removeObjectAtIndex(size_t);
	virtual void removeAll(void);
	TCObject* objectAtIndex(size_t);
	const TCObject* objectAtIndex(size_t) const;
	TCObject* operator[](size_t);
	const TCObject* operator[](size_t) const;
	TCObject* lastObject(void)
		{ return count > 0 ? (*this)[count - 1] : NULL; }
	const TCObject* lastObject(void) const
		{ return count > 0 ? (*this)[count - 1] : NULL; }
	virtual TCObject *copy(void) const;
	virtual void sort(void);
protected:
	virtual ~TCObjectArray(void);
	virtual void dealloc(void);
	static int sortFunction(const void *left, const void *right);
};


#endif // __TCOBJECTARRAY_H__
