#ifndef MemoryWriter_h__
#define MemoryWriter_h__

#include <vector>
#include "Archive.h"
#include "Core.hpp"
#include <glm/glm.hpp>

class FSerializer
{
public:
	FSerializer(IArchive* Archive, bool bIsLoading, bool bOwnArchive = true, bool bEndianIndependent = false);
	~FSerializer();

	inline void SerializeUInt8(uint8& i) { Serialize(&i, 1); }
	inline void SerializeUInt16(uint16& i) { Serialize(&i, 2); }
	inline void SerializeUInt32(uint32& i) { Serialize(&i, 4); }
	inline void SerializeInt(int& i) { Serialize(&i, sizeof(int)); }
	inline void SerializeInt8(int8& i) { Serialize(&i, 1); }
	inline void SerializeInt16(int16& i) { Serialize(&i, 2); }
	inline void SerializeInt32(int32& i) { Serialize(&i, 4); }
	inline void SerializeFloat(float& f) { Serialize(&f, 4); }
	inline void SerializeBool(bool& b) { Serialize(&b, 1); }

	inline void SerializeString(std::string& String)
	{
		u32 ArraySize = String.size();
		SerializeUInt32(ArraySize);

		if (String.size() < ArraySize)
		{
			String.resize(ArraySize);
		}

		for (u32 i = 0; i < ArraySize; i++)
		{
			SerializeInt8(String[i]);
		}
	}

	template <typename T>
	inline void SerializeObject(T& Object)
	{
		Object.Serialize(*this);
	}

	template <typename T>
	inline void SerializeObjectArray(TVector<T>& ObjectArray, u32 ArrayFixedSize = 0)
	{
		uint32 ArraySize = ArrayFixedSize;

		if (ArrayFixedSize == 0)
		{
			ArraySize = ObjectArray.Size();
			SerializeUInt32(ArraySize);

			if (ObjectArray.Size() < ArraySize)
			{
				ObjectArray.Resize(ArraySize);
			}
		}

		SerializeObjects(ObjectArray.Data(), ArraySize);
	}

	template <typename T>
	void SerializeObjects(T* Objects, u32 ElementCount)
	{
		for (u32 i = 0; i < ElementCount; i++)
		{
			SerializeObject(Objects[i]);
		}
	}

	// Template specialization for POD objects
	template <>
	inline void SerializeObject(uint8& i) { SerializeUInt8(i); }

	template <>
	inline void SerializeObject(uint16& i) { SerializeUInt16(i); }

	template <>
	inline void SerializeObject(uint32& i) { SerializeUInt32(i); }

	template <>
	inline void SerializeObject(int8& i) { SerializeInt8(i); }

	template <>
	inline void SerializeObject(int16& i) { SerializeInt16(i); }

	template <>
	inline void SerializeObject(int32& i) { SerializeInt32(i); }

	template <>
	inline void SerializeObject(float& f) { SerializeFloat(f); }

	template <>
	inline void SerializeObject(bool& b) { SerializeBool(b); }

	// Template specialization for glm vectors
	template <>
	inline void SerializeObject(glm::vec2& f) { SerializeFloat(f.x); SerializeFloat(f.y); }

	template <>
	inline void SerializeObject(glm::vec3& f) { SerializeFloat(f.x); SerializeFloat(f.y); SerializeFloat(f.z); }

	template <>
	inline void SerializeObject(glm::vec4& f) { SerializeFloat(f.x); SerializeFloat(f.y); SerializeFloat(f.z); SerializeFloat(f.w); }

	template <>
	inline void SerializeObject(glm::ivec4& f) { SerializeInt(f.x); SerializeInt(f.y); SerializeInt(f.z); SerializeInt(f.w); }

	// Template specialization for optimized array serialization
	template <>
	void SerializeObjects(u32* Objects, u32 ElementCount)
	{
		SerializeElements(Objects, sizeof(u32) * ElementCount, sizeof(u32));
	}

private:

	void Serialize(void* Data, u32 ByteSize);
	void SerializeEndianIndependent(void* Data, u32 ByteSize);
	void SerializeRaw(void* Data, u32 ByteSize);

	void SerializeElements(void* Data, u32 ByteSize, u32 ElementSize);
	void SerializeElementsEndianIndependent(void* Data, u32 ByteSize, u32 ElementSize);
	void SerializeElementsRaw(void* Data, u32 ByteSize, u32 ElementSize);


	bool bEndianIndependent;
	bool bIsLoading;
	bool bOwnArchive;
	IArchive* Archive;
	TVector<char> EndiandSwapBuffer;
};


#endif // MemoryWriter_h__
