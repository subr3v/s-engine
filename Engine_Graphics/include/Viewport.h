#ifndef Viewport_h__
#define Viewport_h__

struct FViewport
{
	int X;
	int Y;
	int Width;
	int Height;

	FViewport(int X = 0, int Y = 0, int Width = 800, int Height = 600) : X(X), Y(Y), Width(Width), Height(Height)
	{

	}

	D3D11_VIEWPORT CreateRenderViewport() const
	{
		D3D11_VIEWPORT Viewport;
		Viewport.TopLeftX = (FLOAT)X;
		Viewport.TopLeftY = (FLOAT)Y;
		Viewport.Width = (FLOAT)Width;
		Viewport.Height = (FLOAT)Height;
		Viewport.MinDepth = 0;
		Viewport.MaxDepth = 1;
		return Viewport;
	}
};

#endif // Viewport_h__
