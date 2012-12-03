// =======================================================================================
//                                      Deferred
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	Deferred Pipeline
///        
/// # Deferred
/// The Geometry buffers (gBuffers) order is, depth, diffuse, normal.
/// Created on: 29-11-2012 
///---------------------------------------------------------------------------------------
#pragma once

#include <d3d11.h>
#include "D3DException.h"
#include "Buffer.h"
#include "ShaderFactory.h"
#include "PTVertex.h"

const static int NUMBUFFERS = 3;
const static int DEPTH = 2;
const static int NORMAL = 1;
const static int DIFFUSE = 0;

class Deferred
{
public:
	Deferred(ID3D11Device* p_device, ID3D11DeviceContext* p_deviceContext, 
		int p_width, int p_height);
	virtual ~Deferred();
	void clearBuffers();
	void deferredBasePass();
	void renderComposedImage();
protected:
private:
	void initDepthStencil();
	void initGeomtryBuffers();
	void createFullScreenQuad();
	void unMapGBuffers();
	void initTestShaders();
private:
	ID3D11Device*			m_device;
	ID3D11DeviceContext*	m_deviceContext;

	ShaderFactory*			m_shaderFactory;

	ID3D11RenderTargetView*		m_gBuffers[NUMBUFFERS];
	ID3D11RenderTargetView*		m_backBuffer;
	ID3D11ShaderResourceView*	m_gBuffersShaderResource[NUMBUFFERS];
	ID3D11DepthStencilView*		m_depthStencilView;

	DeferredBaseShader* m_baseShader;
	DeferredComposeShader* m_composeShader;

	Buffer<PTVertex>* m_vertexBuffer;

	int m_width;
	int m_height;
};