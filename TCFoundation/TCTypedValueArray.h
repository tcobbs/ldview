#ifndef __TCTYPEDVALUEARRAY_H__
#define __TCTYPEDVALUEARRAY_H__

#include <TCFoundation/TCArray.h>

#ifdef WIN32
#pragma warning ( disable: 4710 )
#endif // WIN32

template <class Type> class TCTypedValueArray : public TCArray
{
	public:
		explicit TCTypedValueArray(unsigned int count = 0)
			:TCArray(count) {}

		void addValue(Type value)
			{ TCArray::addItem((void *)value); }
		void insertValue(Type value, unsigned int index = 0)
			{ TCArray::insertItem((void *)value, index); }
		int replaceValue(Type value, unsigned int index)
			{ return TCArray::replaceItem((void *)value, index); }
		int indexOfValue(Type value)
			{ return TCArray::indexOfItem((void *)value); }
		int removeValue(int index)
			{ return TCArray::removeItem(index); }
		Type valueAtIndex(unsigned int index)
			{ return (Type)TCArray::itemAtIndex(index); }
		Type operator[](unsigned int index)
			{ return (Type)TCArray::itemAtIndex(index); }
		Type *getValues(void)
			{ return (Type *)TCArray::getItems(); }
	protected:
		~TCTypedValueArray(void) {}
};

typedef TCTypedValueArray<int> TCIntArray;
typedef TCTypedValueArray<TCULong> TCULongArray;
typedef TCTypedValueArray<float> TCFloatArray;

#endif // __TCTYPEDVALUEARRAY_H__
