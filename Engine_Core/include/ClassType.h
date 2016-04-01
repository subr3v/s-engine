#ifndef ClassType_h__
#define ClassType_h__

#include <string>
#include "Vector.h"
#include <initializer_list>
#include "Core.hpp"

#define TYPE_OF(Class) TClassTypeId<Class>::Get()

// Taken from: http://blog.molecular-matters.com/2015/12/11/getting-the-type-of-a-template-argument-as-string-without-rtti/
namespace internal
{
	static const unsigned int FRONT_SIZE = sizeof("internal::GetTypeNameHelper<") - 1u;
	static const unsigned int BACK_SIZE = sizeof(">::GetTypeName") - 1u;

	template <typename T>
	struct GetTypeNameHelper
	{
		static const char* GetTypeName(void)
		{
			static const size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
			static char typeName[size] = {};
			memcpy(typeName, __FUNCTION__ + FRONT_SIZE, size - 1u);

			return typeName;
		}
	};
}

template <typename T>
const char* GetTypeName(void)
{
	return internal::GetTypeNameHelper<T>::GetTypeName();
}

class FClassInfo
{
public:
	const std::string& GetPrintName() const { return PrintName; }
	u32 GetIdentifier() const { return Identifier; }
private:
	FClassInfo(const char* PrintName);
	~FClassInfo();

	std::string PrintName;
	u32 Identifier;

	static u32 IdentifierGenerator;

	template<typename T> friend class TClassTypeId;
};

template <typename T>
class TClassTypeId
{
public:
	static const FClassInfo* Get()
	{
		static FClassInfo ClassInfo(GetTypeName<T>());
		return &ClassInfo;
	}
};

#endif // ClassType_h__
