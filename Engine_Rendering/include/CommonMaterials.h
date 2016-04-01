#ifndef CommonMaterials_h__
#define CommonMaterials_h__

#include <string>
#include "CoreTypes.h"

struct FCommonMaterials
{
	static ConstantString kDiffuse;
	static ConstantString kDiffuseNormalMapped;
	static ConstantString kDiffuseNormalMappedRoughness;
	static ConstantString kDiffuseNormalMappedRoughnessRefraction;
	static ConstantString kDiffuseRoughness;
	static ConstantString kDiffuseRoughnessRefraction;
	static ConstantString kSprite;
	static ConstantString kPointSprite;
	static ConstantString kFlatColour;
	static ConstantString kFullscreenColour;
	static ConstantString kDeferredRenderingCombine;
	static ConstantString kDeferredRenderingLightPass;
	static ConstantString kDepthOnly;
};

#endif // CommonMaterials_h__
