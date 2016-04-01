#ifndef Scene_h__
#define Scene_h__

class FScene
{
public:
	FScene();
	virtual ~FScene();

	virtual void Load();
	virtual void Unload();
	virtual void Update(float DeltaTime);
	virtual void Render(float DeltaTime);

protected:
	class FInput* InputManager;
	class FGraphicsContext* GraphicsContext;
	class FWindow* Window;
	class FMaterialManager* MaterialManager;
};

#endif // Scene_h__
