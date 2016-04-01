#ifndef Vector_h__
#define Vector_h__

#include "Allocator.h"
#include "Memory.h"
#include <algorithm>
#include <assert.h>
#include <type_traits>
#include <functional>

// Generic Vector Container template class
// Operations supported:
// Add, Remove, Resize
// operator [] overloaded for both const and not const types
// Simple iterator

// Vector Base handles all the low level memory book-keeping operations
// Should anything wrong happen during the execution of any Vector method calls 
// the underlying instance of TVectorBase should be safely disposed.
template <typename T, typename TAllocator>
struct TVectorBase
{
	typedef TAllocator FAllocator_Type;

	TVectorBase();
	TVectorBase(const FAllocator_Type& Allocator);
	TVectorBase(SizeType Count, const FAllocator_Type& Allocator);

	~TVectorBase();
protected:
	T* Begin;
	T* End;
	T* Capacity;

	T* AllocateElements(SizeType Count);
	void FreeElements(T* Memory);
	SizeType GetIncreasedCapacity(SizeType CurrentCapacity);

	FAllocator_Type Allocator;
};


template <typename T, typename TAllocator = FAllocator>
class TVector : public TVectorBase<T, TAllocator>
{
private:
	typedef TVectorBase<T, TAllocator> Super;


public:
	typedef T ValueType;
	typedef T* Pointer;
	typedef const T* ConstPointer;
	typedef T& Reference;
	typedef const T& ConstReference;
	typedef T* Iterator;
	typedef const T* ConstIterator;

	using TVectorBase::Begin;
	using TVectorBase::End;
	using TVectorBase::Capacity;
	using TVectorBase::Allocator;

	TVector();
	TVector(const TVector& Other);
	TVector(TVector&& Other);
	TVector(std::initializer_list<T> Elements);
	TVector(const FAllocator_Type& Allocator);
	TVector(SizeType Count, const FAllocator_Type& Allocator = DEFAULT_ALLOCATOR);

	void Add(const ValueType& Element);
	void Add(ValueType&& Element);
	void RemoveAt(SizeType Index);
	void RemoveAt(Iterator Position);
	void Remove(const ValueType& Element);
	void RemoveSwap(const ValueType& Element);
	void RemoveSwapAt(SizeType Index);
	void Pop();
	Iterator Find(std::function<bool(Reference)> Predicate);
	void Resize(SizeType Count);
	void Resize(SizeType Count, const ValueType& Value);
	void Reserve(SizeType Count);
	void Clear();

	inline Reference At(SizeType Index);
	inline ConstReference At(SizeType Index) const;

	inline Reference Last();
	inline ConstReference Last() const;

	inline Reference operator[](SizeType Index);
	inline ConstReference operator[](SizeType Index) const;
	TVector& operator=(const TVector& Other);
	TVector& operator=(TVector&& Other);

	inline Pointer Data();
	inline ConstPointer Data() const;

	inline SizeType Size() const;

	inline Iterator begin();
	inline Iterator end();

	inline ConstIterator begin() const;
	inline ConstIterator end() const;

	inline bool operator==(TVector Other) const;
private:
	void InsertValueAtEnd(const ValueType& Element);
	void InsertValueAtEnd(ValueType&& Element);
	void InsertValuesAtEnd(SizeType Count, const ValueType& Value);
	void InsertValuesAtEnd(SizeType Count);
};

template <typename T, typename TAllocator /*= FAllocator*/>
typename TVector<T, TAllocator>::ConstReference TVector<T, TAllocator>::At(SizeType Index) const
{
	assert(Begin + Index < End && Begin + Index >= Begin);
	return *(Begin + Index);
}

template <typename T, typename TAllocator /*= FAllocator*/>
typename TVector<T, TAllocator>::Reference TVector<T, TAllocator>::At(SizeType Index)
{
	assert(Begin + Index < End && Begin + Index >= Begin);
	return *(Begin + Index);
}

template <typename T, typename TAllocator /*= FAllocator*/>
TVector<T, TAllocator>& TVector<T, TAllocator>::operator=(const TVector& Other)
{
	this->Clear();
	this->Resize(Other.Size());
	Memory::Copy(Other.Begin, Other.End, Begin);
	End = Begin + Other.Size();
	Capacity = Begin + Size();

	return *this;
}

template <typename T, typename TAllocator /*= FAllocator*/>
TVector<T, TAllocator>& TVector<T, TAllocator>::operator=(TVector&& Other)
{
	this->Begin = Other.Begin;
	this->End = Other.End;
	this->Capacity = Other.Capacity;
	this->Allocator = Other.Allocator;
	Other.Begin = Other.End = Other.Capacity = nullptr;

	return *this;
}

template <typename T, typename TAllocator /*= FAllocator*/>
TVector<T, TAllocator>::TVector(const TVector& Other) : Super()
{
	this->Resize(Other.Size());
	Memory::Copy(Other.Begin, Other.End, Begin);
	End = Begin + Other.Size();
	Capacity = Begin + Size();
}

template <typename T, typename TAllocator /*= FAllocator*/>
void TVector<T, TAllocator>::Clear()
{
	Memory::Destroy(Begin, End);
	End = Begin;
}

template <typename T, typename TAllocator /*= FAllocator*/>
typename TVector<T, TAllocator>::ConstReference TVector<T, TAllocator>::Last() const
{
	return At(Size() - 1);
}

template <typename T, typename TAllocator /*= FAllocator*/>
typename TVector<T, TAllocator>::Reference TVector<T, TAllocator>::Last()
{
	return At(Size() - 1);
}

template <typename T, typename TAllocator>
bool TVector<T, TAllocator>::operator==(TVector Other) const
{
	if (Size() != Other.Size())
		return false;

	for (SizeType Index = 0; Index < Size(); Index++)
	{
		if ((At(Index) == Other.At(Index)) == false)
		{
			return false;
		}
	}

	return true;
}

template <typename T, typename TAllocator>
void TVector<T, TAllocator>::RemoveSwapAt(SizeType Index)
{
	Memory::Swap(*(Begin + Index), *(End - 1));
	Pop();
}

template <typename T, typename TAllocator>
void TVector<T, TAllocator>::RemoveSwap(const ValueType& Element)
{
	for (T* Current = Begin; Current != End; ++Current)
	{
		if ((*Current) == Element && Current != (End - 1))
		{
			RemoveSwapAt(Current - Begin);
			break;
		}
	}
}

template <typename T, typename TAllocator>
SizeType TVector<T, TAllocator>::Size() const
{
	return End - Begin;
}

template <typename T, typename TAllocator>
void TVector<T, TAllocator>::Remove(const ValueType& Element)
{
	for (T* Current = Begin; Current != End; ++Current)
	{
		if ((*Current) == Element)
		{
			RemoveAt(Current - Begin);
			break;
		}
	}
}

template <typename T, typename TAllocator /*= FAllocator*/>
typename TVector<T, TAllocator>::Iterator TVector<T, TAllocator>::Find(std::function<bool(Reference)> Predicate)
{
	for (T* Current = Begin; Current != End; ++Current)
	{
		if (Predicate(*Current))
		{
			return Current;
		}
	}

	return nullptr;
}

template <typename T, typename TAllocator>
void TVector<T, TAllocator>::RemoveAt(SizeType Index)
{
	if (Index == Size() - 1)
	{
		Pop();
	}
	else
	{
		T* Element = Begin + Index;
		if (Element < End)
		{
			Memory::Copy(Begin + Index + 1, End, Begin + Index);
			--End;
		}
		else
		{
			// Uh oh, not what we want
		}
	}
}


template <typename T, typename TAllocator /*= FAllocator*/>
void TVector<T, TAllocator>::RemoveAt(Iterator Position)
{
	RemoveAt(Position - Begin);
}


template <typename T, typename TAllocator /*= FAllocator*/>
void TVector<T, TAllocator>::Reserve(SizeType Count)
{
	if (Count > (SizeType)(Capacity - Begin))
	{
		SizeType PreviousSize = End - Begin;
		SizeType GrowSize = Count;
		SizeType NewSize = std::max(GetIncreasedCapacity(PreviousSize), PreviousSize + Count);

		T* NewMemory = AllocateElements(NewSize);
		Memory::Copy<T>(Begin, End, NewMemory);

		T* NewEnd = NewMemory + PreviousSize;

		Memory::Destroy<T>(Begin, End);
		FreeElements(Begin);

		Begin = NewMemory;
		End = Begin + PreviousSize;
		Capacity = Begin + NewSize;
	}
}


template <typename T, typename TAllocator>
void TVector<T, TAllocator>::Resize(SizeType Count)
{
	if (Count > (SizeType)(Capacity - Begin))
	{
		InsertValuesAtEnd(Count - (End - Begin));
	}
	else
	{
		Memory::Destroy<T>(Begin + Count, End);
		End = Begin + Count;
	}
}

template <typename T, typename TAllocator>
void TVector<T, TAllocator>::Resize(SizeType Count, const ValueType& Value)
{
	if (Count > (SizeType)(Capacity - Begin))
	{
		InsertValuesAtEnd(Count - (End - Begin), Value);
	}
	else
	{
		Memory::Destroy<T>(Begin + Count, End);
		End = Begin + Count;
	}
}

template <typename T, typename TAllocator>
void TVector<T, TAllocator>::InsertValuesAtEnd(SizeType Count, const ValueType& Value)
{
	if (Count > (SizeType)(Capacity - End))
	{
		SizeType PreviousSize = End - Begin;
		SizeType GrowSize = GetIncreasedCapacity(PreviousSize);
		SizeType NewSize = std::max<SizeType>(GetIncreasedCapacity(PreviousSize), PreviousSize + Count);

		T* NewMemory = AllocateElements(NewSize);

		Memory::Copy<T>(Begin, End, NewMemory);

		T* NewEnd = NewMemory + PreviousSize;
		Memory::Fill(NewEnd, Count, Value);

		Memory::Destroy<T>(Begin, End);
		FreeElements(Begin);

		Begin = NewMemory;
		End = Begin + PreviousSize + Count;
		Capacity = Begin + NewSize;
	}
	else
	{
		Memory::Fill(End, Count, Value);
		End += Count;
	}
}


template <typename T, typename TAllocator>
void TVector<T, TAllocator>::InsertValuesAtEnd(SizeType Count)
{
	if (Count > (SizeType)(Capacity - End))
	{
		SizeType PreviousSize = End - Begin;
		SizeType GrowSize = GetIncreasedCapacity(PreviousSize);
		SizeType NewSize = GrowSize > (PreviousSize + Count) ? GrowSize : (PreviousSize + Count); // std::max(GrowSize, PreviousSize + Count);

		T* NewMemory = AllocateElements(NewSize);

		Memory::Copy<T>(Begin, End, NewMemory);

		T* NewEnd = NewMemory + PreviousSize;
		Memory::FillZero(NewEnd, Count);

		Memory::Destroy<T>(Begin, End);
		FreeElements(Begin);

		Begin = NewMemory;
		End = Begin + PreviousSize + Count;
		Capacity = Begin + NewSize;
	}
	else
	{
		Memory::FillZero(End, Count);
		End += Count;
	}
}

template <typename T, typename TAllocator>
void TVector<T, TAllocator>::InsertValueAtEnd(const ValueType& Element)
{
	SizeType NewCapacity = GetIncreasedCapacity(Capacity - Begin);
	SizeType CurrentCount = End - Begin;

	T* NewMemory = AllocateElements(NewCapacity);

	Memory::Copy<T>(Begin, End, NewMemory);
	Memory::Destroy<T>(Begin, End);

	FreeElements(Begin);

	Begin = NewMemory;
	End = Begin + CurrentCount;
	Capacity = Begin + NewCapacity;

	new (End++) ValueType(Element);
}


template <typename T, typename TAllocator /*= FAllocator*/>
void TVector<T, TAllocator>::InsertValueAtEnd(ValueType&& Element)
{
	SizeType NewCapacity = GetIncreasedCapacity(Capacity - Begin);
	SizeType CurrentCount = End - Begin;

	T* NewMemory = AllocateElements(NewCapacity);

	Memory::Move<T>(Begin, End, NewMemory);
	Memory::Destroy<T>(Begin, End);

	FreeElements(Begin);

	Begin = NewMemory;
	End = Begin + CurrentCount;
	Capacity = Begin + NewCapacity;

	new (End++) ValueType(std::move(Element));
}

template <typename T, typename TAllocator>
void TVector<T, TAllocator>::Add(const ValueType& Element)
{
	if (End < Capacity)
	{
		new (End++) ValueType(Element);
	}
	else
	{
		InsertValueAtEnd(Element);
	}
}

template <typename T, typename TAllocator /*= FAllocator*/>
void TVector<T, TAllocator>::Add(ValueType&& Element)
{
	if (End < Capacity)
	{
		new (End++) ValueType(std::move(Element));
	}
	else
	{
		InsertValueAtEnd(std::forward<ValueType>(Element));
	}
}

template <typename T, typename TAllocator>
void TVector<T, TAllocator>::Pop()
{
	if (End > Begin)
	{
		--End;
		End->~ValueType();
	}
}

template <typename T, typename TAllocator>
typename TVector<T, TAllocator>::ConstIterator TVector<T, TAllocator>::begin() const
{
	return Begin;
}

template <typename T, typename TAllocator>
typename TVector<T, TAllocator>::ConstIterator TVector<T, TAllocator>::end() const
{
	return End;
}

template <typename T, typename TAllocator>
typename TVector<T, TAllocator>::Iterator TVector<T, TAllocator>::begin()
{
	return Begin;
}

template <typename T, typename TAllocator>
typename TVector<T, TAllocator>::Iterator TVector<T, TAllocator>::end()
{
	return End;
}

template <typename T, typename TAllocator>
inline typename TVector<T, TAllocator>::ConstPointer TVector<T, TAllocator>::Data() const
{
	return Begin;
}

template <typename T, typename TAllocator>
inline typename TVector<T, TAllocator>::Pointer TVector<T, TAllocator>::Data()
{
	return Begin;
}

template <typename T, typename TAllocator>
inline typename TVector<T, TAllocator>::Reference TVector<T, TAllocator>::operator[](SizeType Index)
{
	return At(Index);
}

template <typename T, typename TAllocator>
inline typename TVector<T, TAllocator>::ConstReference TVector<T, TAllocator>::operator[](SizeType Index) const
{
	return At(Index);
}

template <typename T, typename TAllocator /*= FAllocator*/>
TVector<T, TAllocator>::TVector(std::initializer_list<T> Elements) : Super(Elements.size(), DEFAULT_ALLOCATOR)
{
	End = Begin + Elements.size();
	Memory::Copy<T>(Elements.begin(), Elements.end(), Begin);
}

template <typename T, typename TAllocator /*= FAllocator*/>
TVector<T, TAllocator>::TVector(TVector&& Other) : Super()
{
	this->Begin = Other.Begin;
	this->End = Other.End;
	this->Capacity = Other.Capacity;
	this->Allocator = Other.Allocator;
	Other.Begin = Other.End = Other.Capacity = nullptr;
}

template <typename T, typename TAllocator /*= FAllocator*/>
TVector<T, TAllocator>::TVector(SizeType Count, const FAllocator_Type& Allocator) : Super(Count, Allocator)
{

}

template <typename T, typename TAllocator /*= FAllocator*/>
TVector<T, TAllocator>::TVector(const FAllocator_Type& Allocator) : Super(Allocator)
{

}

template <typename T, typename TAllocator /*= FAllocator*/>
TVector<T, TAllocator>::TVector() : Super()
{

}

// Vector base implementation.

template <typename T, typename TAllocator /*= FAllocator*/>
TVectorBase<T, TAllocator>::TVectorBase(const FAllocator_Type& Allocator) : Begin(nullptr), End(nullptr), Capacity(nullptr), Allocator(Allocator)
{

}

template <typename T, typename TAllocator /*= FAllocator*/>
TVectorBase<T, TAllocator>::TVectorBase() : Begin(nullptr), End(nullptr), Capacity(nullptr), Allocator(DEFAULT_ALLOCATOR_NAME)
{

}

template <typename T, typename TAllocator /*= FAllocator*/>
TVectorBase<T, TAllocator>::TVectorBase(SizeType Count, const FAllocator_Type& Allocator) : Allocator(Allocator)
{
	Begin = AllocateElements(Count);
	End = Begin;
	Capacity = Begin + Count;
}

template <typename T, typename TAllocator /*= FAllocator*/>
TVectorBase<T, TAllocator>::~TVectorBase()
{
	if (Begin != nullptr)
	{
		Memory::Destroy<T>(Begin, End);
		FreeElements(Begin);
	}
}

template <typename T, typename TAllocator /*= FAllocator*/>
T* TVectorBase<T, TAllocator>::AllocateElements(SizeType Count)
{
	return (T*)Engine_MemAllocAligned(Allocator, sizeof(T) * Count,std::alignment_of<T>::value);
}

template <typename T, typename TAllocator /*= FAllocator*/>
void TVectorBase<T, TAllocator>::FreeElements(T* Memory)
{
	Engine_MemFree(Allocator, Memory);
}

template <typename T, typename TAllocator>
SizeType TVectorBase<T, TAllocator>::GetIncreasedCapacity(SizeType CurrentCapacity)
{
	return CurrentCapacity > 0 ? (CurrentCapacity * 2) : 1;
}

#endif // Vector_h__
