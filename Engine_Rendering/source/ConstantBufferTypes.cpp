#include "ConstantBufferTypes.h"

ConstantString FInstanceBuffer::kBufferName = "ObjectBuffer";
ConstantString FTextureNames::kDiffuseMap = "DiffuseTexture";
ConstantString FTextureNames::kNormalMap = "NormalTexture";
ConstantString FTextureNames::kRoughnessMap = "RoughnessTexture";
ConstantString FTextureNames::kRefractionMap = "RefractionTexture";
ConstantString FMaterialBuffer::kBufferName = "MaterialBuffer";

FInstanceBuffer FInstanceBuffer::Identity = FInstanceBuffer(glm::mat4(1.0f));