#include "Model.h"
#include "Mesh.h"
#include "Skeleton.h"

FModel::FModel()
{
	
}

FModel::~FModel()
{
	delete Skeleton;
	Skeleton = nullptr;

	delete Mesh;
	Mesh = nullptr;

	for (size_t i = 0; i < Animations.Size(); i++)
	{
		delete Animations[i];
	}
	Animations.Clear();
}