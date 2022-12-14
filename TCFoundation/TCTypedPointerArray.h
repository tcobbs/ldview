#ifndef __TCTYPEDPOINTERARRAY_H__
#define __TCTYPEDPOINTERARRAY_H__

#include <TCFoundation/TCArray.h>

#ifdef WIN32
#pragma warning ( disable: 4710 )
#endif // WIN32

template <class Type> class TCTypedPointerArray : public TCArray<>
{
	public:
		explicit TCTypedPointerArray(size_t count = 0)
			:TCArray<>(count) {}
		TCTypedPointerArray(const TCTypedPointerArray<Type> &other)
			:TCArray<>(other) {}

		virtual TCObject *copy(void) const
		{
			return new TCTypedPointerArray<Type>(*this);
		}
		void addPointer(Type pointer)
			{ TCArray<>::addItem(pointer); }
		void insertPointer(Type pointer, size_t index = 0)
			{ TCArray<>::insertItem(pointer, index); }
		bool replacePointer(Type pointer, size_t index)
			{ return TCArray<>::replaceItem(pointer, index); }
		ptrdiff_t indexOfPointer(Type pointer)
			{ return TCArray<>::indexOfItem(pointer); }
		bool removePointer(Type pointer)
			{ return TCArray<>::removeItem(pointer); }
		bool removePointerAtIndex(size_t index)
			{ return TCArray<>::removeItemAtIndex(index); }
		Type pointerAtIndex(size_t index)
			{ return (Type)TCArray<>::itemAtIndex(index); }
		Type operator[](size_t index)
			{ return (Type)TCArray<>::itemAtIndex(index); }
	protected:
};

#endif // __TCTYPEDPOINTERARRAY_H__
