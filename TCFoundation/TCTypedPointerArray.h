#ifndef __TCTYPEDPOINTERARRAY_H__
#define __TCTYPEDPOINTERARRAY_H__

#include <TCFoundation/TCArray.h>

#ifdef WIN32
#pragma warning ( disable: 4710 )
#endif // WIN32

template <class Type> class TCTypedPointerArray : public TCArray
{
	public:
		explicit TCTypedPointerArray(unsigned int count = 0)
			:TCArray(count) {}

		void addPointer(Type pointer)
			{ TCArray::addItem(pointer); }
		void insertPointer(Type pointer, unsigned int index = 0)
			{ TCArray::insertItem(pointer, index); }
		int replacePointer(Type pointer, unsigned int index)
			{ return TCArray::replaceItem(pointer, index); }
		int indexOfPointer(Type pointer)
			{ return TCArray::indexOfItem(pointer); }
		int indexOfPointerIdenticalTo(Type pointer)
			{ return TCArray::indexOfItemIdenticalTo(pointer); }
		int removePointer(Type pointer)
			{ return TCArray::removeItem(pointer); }
		int removePointerIdenticalTo(Type pointer)
			{ return TCArray::removeItemIdenticalTo(pointer); }
		int removePointer(int index)
			{ return TCArray::removeItem(index); }
		Type pointerAtIndex(unsigned int index)
			{ return (Type)TCArray::itemAtIndex(index); }
		Type operator[](unsigned int index)
			{ return (Type)TCArray::itemAtIndex(index); }
	protected:
};

#endif __TCTYPEDPOINTERARRAY_H__
