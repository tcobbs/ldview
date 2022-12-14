#ifndef __TCTYPEDOBJECTARRAY_H__
#define __TCTYPEDOBJECTARRAY_H__

#include <TCFoundation/TCObjectArray.h>

#ifdef WIN32
#pragma warning ( disable: 4710 )
#endif // WIN32

template <class Type> class TCTypedObjectArray : public TCObjectArray
{
public:
	explicit TCTypedObjectArray(size_t count = 0)
		:TCObjectArray(count) {}
	TCTypedObjectArray(const TCTypedObjectArray<Type> &other)
		:TCObjectArray(other) {}
	virtual TCObject *copy(void) const
	{
		return new TCTypedObjectArray<Type>(*this);
	}

	void addObject(Type* object)
		{ TCObjectArray::addObject(object); }
	void insertObject(Type* object, size_t index = 0)
		{ TCObjectArray::insertObject(object, index); }
	bool replaceObject(Type* object, size_t index)
		{ return TCObjectArray::replaceObject(object, index); }
	ptrdiff_t indexOfObject(Type* object) const
		{ return TCObjectArray::indexOfObject(object); }
	ptrdiff_t indexOfObjectIdenticalTo(Type* object) const
		{ return TCObjectArray::indexOfObjectIdenticalTo(object); }
	bool removeObject(Type* object)
		{ return TCObjectArray::removeObject(object); }
	bool removeObjectIdenticalTo(Type* object)
		{ return TCObjectArray::removeObjectIdenticalTo(object); }
	bool removeObjectAtIndex(size_t index)
		{ return TCObjectArray::removeObjectAtIndex(index); }
	Type* objectAtIndex(size_t index)
		{ return (Type*)TCObjectArray::objectAtIndex(index); }
	const Type* objectAtIndex(size_t index) const
		{ return (Type*)TCObjectArray::objectAtIndex(index); }
	Type* operator[](size_t index)
		{ return (Type*)TCObjectArray::objectAtIndex(index); }
	const Type* operator[](size_t index) const
		{ return (Type*)TCObjectArray::objectAtIndex(index); }
	Type* lastObject(void)
		{ return (Type*)TCObjectArray::lastObject(); }
	const Type* lastObject(void) const
		{ return (Type*)TCObjectArray::lastObject(); }
protected:
};

#endif // __TCTYPEDOBJECTARRAY_H__
