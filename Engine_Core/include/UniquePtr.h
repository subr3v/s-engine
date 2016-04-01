#ifndef UniquePtr_h__
#define UniquePtr_h__

template <typename T>
class TUniquePtr
{
public:
	typedef T* Pointer;
	typedef T Value_Type;

	TUniquePtr() : CurrentPointer(nullptr)
	{

	}

	TUniquePtr(TUniquePtr&& Other) : CurrentPointer(Other.CurrentPointer)
	{
		Other.CurrentPointer = nullptr;
	}

	TUniquePtr(T* Value) : CurrentPointer(Value)
	{
		
	}


	TUniquePtr& operator=(std::nullptr_t)
	{
		delete CurrentPointer;
		CurrentPointer = nullptr;
		return *this;
	}

	TUniquePtr& operator=(TUniquePtr&& Other)
	{
		delete CurrentPointer;
		CurrentPointer = Other.CurrentPointer;
		Other.CurrentPointer = nullptr;
		return *this;
	}

	~TUniquePtr()
	{
		delete CurrentPointer;
		CurrentPointer = nullptr;
	}

	operator bool() const
	{
		return CurrentPointer != nullptr;
	}

	Value_Type operator*() const
	{
		assert(CurrentPointer);
		return *CurrentPointer;
	}

	Pointer operator->() const
	{
		return CurrentPointer;
	}

	Pointer Get() const
	{
		return CurrentPointer;
	}
private:
	TUniquePtr& operator=(Pointer);
	TUniquePtr(TUniquePtr& Other);

	Pointer CurrentPointer;
};


template <typename T>
inline TUniquePtr<T> Make_Unique()
{
	return TUniquePtr<T>(new T());
}

template <typename T, typename TArg1>
inline TUniquePtr<T> Make_Unique(TArg1&& Arg1)
{
	return TUniquePtr<T>(new T(std::forward<TArg1>(Arg1)));
}

template <typename T, typename TArg1, typename TArg2>
inline TUniquePtr<T> Make_Unique(TArg1&& Arg1, TArg2&& Arg2)
{
	return TUniquePtr<T>(new T(std::forward<TArg1>(Arg1), std::forward<TArg2>(Arg2)));
}

template <typename T, typename TArg1, typename TArg2, typename TArg3>
inline TUniquePtr<T> Make_Unique(TArg1&& Arg1, TArg2&& Arg2, TArg3&& Arg3)
{
	return TUniquePtr<T>(new T(std::forward<TArg1>(Arg1), std::forward<TArg2>(Arg2), std::forward<TArg3>(Arg3)));
}

template <typename T, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
inline TUniquePtr<T> Make_Unique(TArg1&& Arg1, TArg2&& Arg2, TArg3&& Arg3, TArg4&& Arg4)
{
	return TUniquePtr<T>(new T(std::forward<TArg1>(Arg1), std::forward<TArg2>(Arg2), std::forward<TArg3>(Arg3), std::forward<TArg4>(Arg4)));
}

template <typename T, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5>
inline TUniquePtr<T> Make_Unique(TArg1&& Arg1, TArg2&& Arg2, TArg3&& Arg3, TArg4&& Arg4, TArg5&& Arg5)
{
	return TUniquePtr<T>(new T(std::forward<TArg1>(Arg1), std::forward<TArg2>(Arg2), std::forward<TArg3>(Arg3), std::forward<TArg4>(Arg4), std::forward<TArg5>(Arg5)));
}

template <typename T, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6>
inline TUniquePtr<T> Make_Unique(TArg1&& Arg1, TArg2&& Arg2, TArg3&& Arg3, TArg4&& Arg4, TArg5&& Arg5, TArg6&& Arg6)
{
	return TUniquePtr<T>(new T(std::forward<TArg1>(Arg1), std::forward<TArg2>(Arg2), std::forward<TArg3>(Arg3), std::forward<TArg4>(Arg4), std::forward<TArg5>(Arg5), std::forward<TArg6>(Arg6)));
}

template <typename T, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7>
inline TUniquePtr<T> Make_Unique(TArg1&& Arg1, TArg2&& Arg2, TArg3&& Arg3, TArg4&& Arg4, TArg5&& Arg5, TArg6&& Arg6, TArg7&& Arg7)
{
	return TUniquePtr<T>(new T(std::forward<TArg1>(Arg1), std::forward<TArg2>(Arg2), std::forward<TArg3>(Arg3), std::forward<TArg4>(Arg4), std::forward<TArg5>(Arg5), std::forward<TArg6>(Arg6), std::forward<TArg7>(Arg7)));
}

template <typename T, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8>
inline TUniquePtr<T> Make_Unique(TArg1&& Arg1, TArg2&& Arg2, TArg3&& Arg3, TArg4&& Arg4, TArg5&& Arg5, TArg6&& Arg6, TArg7&& Arg7, TArg8&& Arg8)
{
	return TUniquePtr<T>(new T(std::forward<TArg1>(Arg1), std::forward<TArg2>(Arg2), std::forward<TArg3>(Arg3), std::forward<TArg4>(Arg4), std::forward<TArg5>(Arg5), std::forward<TArg6>(Arg6), std::forward<TArg7>(Arg7), std::forward<TArg8>(Arg8)));
}

template <typename T, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8, typename TArg9>
inline TUniquePtr<T> Make_Unique(TArg1&& Arg1, TArg2&& Arg2, TArg3&& Arg3, TArg4&& Arg4, TArg5&& Arg5, TArg6&& Arg6, TArg7&& Arg7, TArg8&& Arg8, TArg9&& Arg9)
{
	return TUniquePtr<T>(new T(std::forward<TArg1>(Arg1), std::forward<TArg2>(Arg2), std::forward<TArg3>(Arg3), std::forward<TArg4>(Arg4), std::forward<TArg5>(Arg5), std::forward<TArg6>(Arg6), std::forward<TArg7>(Arg7), std::forward<TArg8>(Arg8), std::forward<TArg9>(Arg9)));
}

#endif // UniquePtr_h__
