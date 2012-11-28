// =======================================================================================
//                                      D3DRender
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	Brief
///        
/// # D3DRender
/// Detailed description.....
/// Created on: 27-11-2012 
///---------------------------------------------------------------------------------------
#pragma once

#include <d3d11.h>
#include "D3DException.h"
#include "Util.h"
#include "Shader.h"

struct PTVertex
{
	float pos[3];
	float tex[2];
};

class D3DRender
{
public:
	D3DRender(HWND p_hWnd, int p_width, int p_height, bool p_windowed);
	virtual ~D3DRender();
	void clearRenderTargets();
	void render();
	void flipBackBuffer();
protected:
private:
	void initHardware(HWND p_hWnd, bool p_windowed);
	void initBuffers();
	void initViewport();
	void initFullScreenQuad();
private:
	ID3D11Device*			m_device;
	ID3D11DeviceContext*	m_deviceContext;
	IDXGISwapChain*			m_swapChain;
	D3D_FEATURE_LEVEL		m_featureLevel;
	
	ID3D11RenderTargetView* m_backBuffer;
	ID3D11Texture2D*		m_depthStencil;
	ID3D11DepthStencilView* m_depthStencilView;

	Shader*					m_shader;
	ID3D11Buffer*			m_vertexBuffer;

	int m_height;
	int m_width;
};