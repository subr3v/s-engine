#ifndef CoreTypes_h__
#define CoreTypes_h__

#include <cstddef>
#include <glm/glm.hpp>

typedef unsigned int	uint;
typedef unsigned long	uint32;
typedef unsigned short	uint16;
typedef unsigned char	uint8;
typedef long			int32;
typedef short			int16;
typedef char			int8;
typedef unsigned char	byte;

typedef unsigned long	u32;
typedef unsigned short	u16;
typedef unsigned char	u8;
typedef long			s32;
typedef short			s16;
typedef char			s8;
typedef std::size_t SizeType;
typedef const char* ConstantString;
typedef glm::vec3 FVector;
typedef glm::vec2 FVector2D;

#define ENGINE_EXPORT_DLL __declspec(dllexport)

#endif // CoreTypes_h__
