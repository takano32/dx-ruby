#pragma once


#define RELEASE(p) { if(p){(p)->Release();p=NULL;} }

//#include <d3d.h>
#include <d3d9.h>
#include <d3dx9.h>
//#include <dxerr9.h>

namespace base{
	class GraphicsSystem{
		HWND _hWnd;
		LPDIRECT3D9 _pGraphicsObject;
		LPDIRECT3DDEVICE9 _pDevice;
		LPD3DXSPRITE _pSprite;

	private:
		BOOL InitializeDevice();
	public:
		GraphicsSystem( HWND hWnd );
		~GraphicsSystem();
		LPDIRECT3DDEVICE9 GetDevice();
		LPD3DXSPRITE GetSprite();
		BOOL Clear( D3DCOLOR color );
		BOOL BeginScene();
		BOOL EndScene();
		BOOL Present();
	};
}