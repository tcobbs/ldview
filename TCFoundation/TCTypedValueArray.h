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
		TCTypedValueArray(const TCTypedValueArray<Type> &other)
			:TCArray(other) {}

		virtual TCObject *copy(void)
		{
			return new TCTypedValueArray<Type>(*this);
		}
		void addValue(Type value)
			{ TCArray::addItem((void *)(long)value); }
		void insertValue(Type value, unsigned int index = 0)
			{ TCArray::insertItem((void *)(long)value, index); }
		int replaceValue(Type value, unsigned int index)
			{ return TCArray::replaceItem((void *)(long)value, index); }
		int indexOfValue(Type value)
			{ return TCArray::indexOfItem((void *)(long)value); }
		int removeValue(int index)
			{ return TCArray::removeItem(index); }
		int removeValues(int index, int numValues)
			{ return TCArray::removeItems(index, numValues); }
		Type valueAtIndex(unsigned int index)
			{ return (Type)TCArray::itemAtIndex(index); }
		Type operator[](unsigned int index)
			{ return (Type)TCArray::itemAtIndex(index); }
		Type *getValues(void)
			{ return (Type *)TCArray::getItems(); }
		void sort(void)
			{ sortUsingFunction(valueSortFunction); }
	protected:
		~TCTypedValueArray(void) {}
		static int valueSortFunction(const void *left, const void *right)
		{
			Type leftVal = *(Type*)left;
			Type rightVal = *(Type*)right;

			if (leftVal < rightVal)
			{
				return -1;
			}
			else if (leftVal > rightVal)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
};

typedef TCTypedValueArray<int> TCIntArray;
typedef TCTypedValueArray<TCULong> TCULongArray;
typedef TCTypedValueArray<float> TCFloatArray;

#endif // __TCTYPEDVALUEARRAY_H__
