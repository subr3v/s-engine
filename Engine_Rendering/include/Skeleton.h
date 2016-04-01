#ifndef Skeleton_h__
#define Skeleton_h__

#include <string>
#include "Vector.h"
#include <glm/glm.hpp>

class FBone
{
public:
	std::string Name;
	int ParentIndex;
	glm::mat4 InverseBindPose;
	glm::mat4 BindPoseTransform;

	FBone(const std::string& Name, int ParentIndex) : Name(Name), ParentIndex(ParentIndex) { }
	FBone() { }
};

template <typename T>
struct FAnimationChannelKey
{
	float time;
	T Value;
};

struct FAnimationChannel
{
	int BoneID;
	TVector<glm::mat4> Transforms;
};

class FSkeletalAnimation
{
public:
	std::string Name;
	float Duration;
	TVector<FAnimationChannel> Keyframes;
	class FSkeleton* Skeleton;

	FSkeletalAnimation(class FSkeleton* Skeleton);
};

class FSkeleton
{
public:
	FSkeleton();
	TVector<FBone> Bones;

	FBone* FindBoneByName(const std::string& name);
	int FindBoneIndexByName(const std::string& name) const;
};

class FSkeletonDebugger
{
public:
	void DrawSkeleton(FSkeleton* Skeleton);
private:
	void DrawBone(const glm::mat4& Transform);
};

#endif // Skeleton_h__
