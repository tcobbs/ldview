#ifndef __TCTYPEDVALUEARRAY_H__
#define __TCTYPEDVALUEARRAY_H__

#include <TCFoundation/TCArray.h>

#ifdef WIN32
#pragma warning ( disable: 4710 )
#endif // WIN32

template <class Type> class TCTypedValueArray : public TCArray<Type>
{
	public:
		explicit TCTypedValueArray(size_t count = 0)
			:TCArray<Type>(count) {}
		TCTypedValueArray(const TCTypedValueArray<Type> &other)
			:TCArray<Type>(other) {}

		virtual TCObject *copy(void) const
		{
			return new TCTypedValueArray<Type>(*this);
		}
		void addValue(Type value)
			{ TCArray<Type>::addItem(value); }
		void insertValue(Type value, size_t index = 0)
			{ TCArray<Type>::insertItem(value, index); }
		bool replaceValue(Type value, size_t index)
			{ return TCArray<Type>::replaceItem(value, index); }
		ptrdiff_t indexOfValue(Type value) const
			{ return TCArray<Type>::indexOfItem(value); }
		bool removeValueAtIndex(size_t index)
			{ return TCArray<Type>::removeItemAtIndex(index); }
		bool removeValues(ptrdiff_t index, size_t numValues)
			{ return TCArray<Type>::removeItems(index, numValues); }
		Type valueAtIndex(size_t index)
			{ return (Type)TCArray<Type>::itemAtIndex(index); }
		const Type valueAtIndex(size_t index) const
			{ return (const Type)TCArray<Type>::itemAtIndex(index); }
		Type operator[](size_t index)
			{ return TCArray<Type>::itemAtIndex(index); }
		const Type operator[](size_t index) const
			{ return (const Type)TCArray<Type>::itemAtIndex(index); }
		Type *getValues(void)
			{ return (Type *)TCArray<Type>::getItems(); }
		const Type *getValues(void) const
			{ return (const Type *)TCArray<Type>::getItems(); }
		void sort(void)
			{ TCArray<Type>::sortUsingFunction((TCArraySortFunction)valueSortFunction); }
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

#endif // __TCTYPEDVALUEARRAY_H__
