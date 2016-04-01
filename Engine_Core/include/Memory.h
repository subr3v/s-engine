#ifndef Memory_h__
#define Memory_h__

#include "CoreTypes.h"

namespace Memory
{
	template <typename T>
	void Copy(const T* Begin, const T* End, T* Destination)
	{
		const T* Current;
		T* CurrentDestination;
		for (Current = Begin, CurrentDestination = Destination; Current < End; ++Current, ++CurrentDestination)
		{
			new (CurrentDestination) T(*Current);
		}
	}

	template <typename T>
	void Move(const T* Begin, const T* End, T* Destination)
	{
		const T* Current = Begin;
		T* CurrentDestination;
		for (Current = Begin, CurrentDestination = Destination; Current < End; ++Current, ++CurrentDestination)
		{
			new (CurrentDestination)T(std::move((T&&)*Current));
		}
	}

	template <typename T>
	void Destroy(T* Begin, T* End)
	{
		for (T* Current = Begin; Current < End; ++Current)
		{
			Current->~T();
		}
	}

	template <typename T>
	void Fill(T* Begin, SizeType Count, const T& Value)
	{
		for (T* Current = Begin; Current < Begin + Count; ++Current)
		{
			new (Current) T(Value);
		}
	}

	template <typename T>
	void FillZero(T* Begin, SizeType Count)
	{
		memset(Begin, 0, sizeof(T) * Count);
	}

	template <typename T>
	void Swap(T& A, T& B)
	{
		T Temp = B;
		B = A;
		A = Temp;
	}

}

#endif // Memory_h__
