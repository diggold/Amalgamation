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
#include "Buffer.h"
#include "Deferred.h"
#include "RendererMeshInfo.h"

class D3DRender
{
public:
	D3DRender(HWND p_hWnd, int p_width, int p_height, bool p_windowed);
	virtual ~D3DRender();
	///-----------------------------------------------------------------------------------
	/// Clears the back buffer with a non black color
	/// \returns void
	///-----------------------------------------------------------------------------------
	void clearRenderTargets();

	///-----------------------------------------------------------------------------------
	/// Sets up the frame, prepares the renderer for draw calls.
	/// \returns void
	///-----------------------------------------------------------------------------------
	void beginFrame();

	///-----------------------------------------------------------------------------------
	/// Renders the whole scene
	/// \returns void
	///-----------------------------------------------------------------------------------
	void renderMesh(const RendererMeshInfo& p_meshInfo);

	///-----------------------------------------------------------------------------------
	/// Finalizes the frame. For example; a deferred subsystem will
	/// render to backbuffer here.
	/// \returns void
	///-----------------------------------------------------------------------------------
	void finalizeFrame();

	///-----------------------------------------------------------------------------------
	/// Desc
	/// \returns void
	///-----------------------------------------------------------------------------------
	void flipBackBuffer();
private:

	///-----------------------------------------------------------------------------------
	/// Initialize the graphic card, need input is a win32 window used to present the 
	/// render result
	/// \param p_hWnd
	/// \param p_windowed
	/// \returns void
	///-----------------------------------------------------------------------------------
	void initHardware(HWND p_hWnd, bool p_windowed);

	///-----------------------------------------------------------------------------------
	/// Creates the true back buffer
	/// \returns void
	///-----------------------------------------------------------------------------------
	void initBackBuffer();

	///-----------------------------------------------------------------------------------
	/// Desc
	/// \returns void
	///-----------------------------------------------------------------------------------
	void initViewport();

private:
	ID3D11Device*			m_device;
	ID3D11DeviceContext*	m_deviceContext;
	IDXGISwapChain*			m_swapChain;
	D3D_FEATURE_LEVEL		m_featureLevel;

	Deferred*				m_deferred;

	ID3D11RenderTargetView* m_backBuffer;

	DeferredBaseShader*		m_deferredBaseShader;

	int m_height;
	int m_width;
};