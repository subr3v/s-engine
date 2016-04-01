#include "MaterialManager.h"
#ifndef ImGuiImpl_h__
#define ImGuiImpl_h__

class FGraphicsContext;
class FInput;

namespace ImGui
{
	void ShowProfiler();
}

class FImGuiImpl
{
public:
	FImGuiImpl(FGraphicsContext* GraphicsContext, FMaterialManager* MaterialManager, FInput* Input);
	~FImGuiImpl();

	void Init();
	void Update(float DeltaTime);
	void Render();
private:
	void SetUIStyle();

	FGraphicsContext* GraphicsContext;
	FMaterialManager* MaterialManager;
	FInput* Input;
	class FTexture* FontTexture = nullptr;

	class FGraphicsBuffer* VertexBuffer = nullptr;
	class FGraphicsBuffer* IndexBuffer = nullptr;
	bool bShowEngineVars = false;
};


#endif // ImGuiImpl_h__
