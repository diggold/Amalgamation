#pragma once
#include <d3d11.h>
#include <vector>

using namespace std;

class ShaderFactory;

// =======================================================================================
//                                      ShadowMapRender
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	Brief
///        
/// # ShadowMapRender
/// Detailed description.....
/// Created on: 31-1-2013 
///---------------------------------------------------------------------------------------

struct ShadowMap
{
	ID3D11DepthStencilView*		depthStencilView;
	ID3D11ShaderResourceView*	resourceView;
};

class ShadowMapRenderer
{
public:
	ShadowMapRenderer(ID3D11Device* p_device, ID3D11DeviceContext* p_deviceContext, 
		ShaderFactory* p_shaderFactory);
	~ShadowMapRenderer();
	void mapShadowMapToRenderTarget();
	void mapShadowMapToRenderTarget(unsigned int p_index);
	ID3D11ShaderResourceView* getShadowMap();
	ID3D11ShaderResourceView* getShadowMap(unsigned int p_index);
	unsigned int createANewShadowMap();
private:
	void generateShadowMap( ID3D11DepthStencilView** p_depthStencileView, 
		ID3D11ShaderResourceView** p_shaderResourceView);
	void initDepthStencil();
public:
	const static int SHADOWMAPSIZE = 512;
private:
	ID3D11Device*				m_device;
	ID3D11DeviceContext*		m_deviceContext;
	ID3D11DepthStencilView*		m_depthStencilView;
	ID3D11ShaderResourceView*	m_resourceView;

	ShaderFactory*		m_shaderFactory;

	vector<ShadowMap*>	m_shadowMaps;
};