
#include "AglParticleSystem.h"
#include "AntTweakBarWrapper.h"
#include "BufferFactory.h"
#include "D3DException.h"
#include "D3DUtil.h"
#include "DeferredBaseShader.h"
#include "DeferredComposeShader.h"
#include "DeferredRenderer.h"
#include "GraphicsWrapper.h"
#include "Mesh.h"
#include "Mesh.h"
#include "MeshLoadException.h"
#include "ParticleRenderer.h"
#include "Texture.h"
#include "TextureParser.h"
#include "TextureFactory.h"
#include <AglReader.h>
#include <FileCheck.h>
#include <LightInstanceData.h>
#include "ShaderFactory.h"
#include "GUIShader.h"
#include "LightMesh.h"
#include "DeferredBaseShader.h"
#include "DeferredAnimatedBaseShader.h"
#include "DeferredTessBaseShader.h"
#include "DeferredTessAnimatedBaseShader.h"
#include "ShadowMapRenderer.h"
#include "ShadowShader.h"
#include "GPUTimer.h"
#include "LightShader.h"
#include "ParticleSystemAndTexture.h"

GraphicsWrapper::GraphicsWrapper( HWND p_hWnd, int p_width, int p_height, bool p_windowed,
								 bool p_enableHdr, bool p_enableEffects, int p_vsync )
{
	m_device		= NULL;
	m_deviceContext = NULL;
	m_swapChain		= NULL;

	m_deferredBaseShader	= NULL;

	m_width	= p_width;
	m_height= p_height;
	m_lowResDivider = 4;
	m_windowed = p_windowed;
	m_enableHdr = p_enableHdr;
	m_enableEffects = p_enableEffects;
	m_wireframeMode = false;
	m_renderingShadows = false;
	m_vsync = p_vsync;

	initSwapChain(p_hWnd);
	initHardware();
	initBackBuffer();


	HRESULT hr = S_OK;
	IDXGIDevice * pDXGIDevice;
	hr = m_device->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);
	IDXGIAdapter * pDXGIAdapter;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);
	IDXGIFactory * pIDXGIFactory;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);

	pIDXGIFactory->MakeWindowAssociation(p_hWnd, DXGI_MWA_NO_ALT_ENTER);


	m_shaderFactory		= new ShaderFactory(m_device, m_deviceContext, 
		m_device->GetFeatureLevel());
	m_bufferFactory		= new BufferFactory(m_device,m_deviceContext);
	m_renderSceneInfoBuffer = m_bufferFactory->createRenderSceneInfoCBuffer();
	m_perShadowBuffer = m_bufferFactory->createPerShadowBuffer();
	m_meshManager		= new ResourceManager<Mesh>();
	m_textureManager	= new ResourceManager<Texture>();
	m_textureFactory	= new TextureFactory(m_device,m_deviceContext,m_textureManager);
	m_modelFactory		= new ModelExtendedFactory(m_device,m_bufferFactory,m_meshManager,
												   m_textureFactory);

	m_guiShader = m_shaderFactory->createGUIShader(
		L"Shaders/GUI/rocket.hlsl");
	m_shadowShader = m_shaderFactory->createShadowShader(
		L"Shaders/Game/shadowMap.hlsl");
	createTexture("mesherror.png",TEXTUREPATH);
	m_solidWhiteTexture = createTexture("1x1_solid_white.png", TEXTUREPATH);
	m_randomNormalTextures = createTexture("randNormals.jpg",TEXTUREPATH);
	m_deferredRenderer = new DeferredRenderer( m_device, m_deviceContext, 
							   m_width, m_height, m_enableHdr, m_enableEffects );
	m_particleRenderer = new ParticleRenderer( m_device, m_deviceContext);
	m_shadowMapRenderer = new ShadowMapRenderer(m_device, m_deviceContext, m_shaderFactory);
	m_gpuTimer = new GPUTimer(m_device,m_deviceContext);

	resetViewportToStdSize();
	clearRenderTargets();
	initBoneMatrixTexture();
}

GraphicsWrapper::~GraphicsWrapper()
{
	SAFE_DELETE(m_guiShader);
	SAFE_DELETE(m_shadowShader);
	SAFE_DELETE(m_deferredRenderer);
	SAFE_DELETE(m_particleRenderer);
	SAFE_DELETE(m_shadowMapRenderer);
	SAFE_DELETE(m_deferredBaseShader);
	SAFE_DELETE(m_shaderFactory);
	SAFE_DELETE(m_bufferFactory);
	SAFE_DELETE(m_meshManager);
	SAFE_DELETE(m_textureManager);
	SAFE_DELETE(m_textureFactory);
	SAFE_DELETE(m_modelFactory);
	SAFE_DELETE(m_renderSceneInfoBuffer);
	SAFE_DELETE(m_perShadowBuffer);
	SAFE_DELETE(m_gpuTimer);
	SAFE_DELETE(m_boneMatrixTexture);
	SAFE_RELEASE(m_boneMatrixResource);

	changeToWindowed(true);

	SAFE_RELEASE(m_device);
	SAFE_RELEASE(m_deviceContext);
	SAFE_RELEASE(m_swapChain);
	releaseBackBuffer();
	
}

void GraphicsWrapper::initSwapChain(HWND p_hWnd)
{
	ZeroMemory( &m_swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC) );
	m_swapChainDesc.BufferCount = 1;
	m_swapChainDesc.BufferDesc.Width = m_width;
	m_swapChainDesc.BufferDesc.Height = m_height;
	m_swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	m_swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	m_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_swapChainDesc.OutputWindow = p_hWnd;
	m_swapChainDesc.SampleDesc.Count = 1;
	m_swapChainDesc.SampleDesc.Quality = 0;
	m_swapChainDesc.Windowed = m_windowed;
}

void GraphicsWrapper::initHardware()
{
	HRESULT hr = S_OK;
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_DRIVER_TYPE driverTypes[] = 
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	D3D_FEATURE_LEVEL featureLevelsToTry[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	D3D_FEATURE_LEVEL initiatedFeatureLevel;

	int selectedDriverType = -1;

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		D3D_DRIVER_TYPE driverType;
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			driverType,
			NULL,
			createDeviceFlags,
			featureLevelsToTry,
			ARRAYSIZE(featureLevelsToTry),
			D3D11_SDK_VERSION,
			&m_swapChainDesc,
			&m_swapChain,
			&m_device,
			&initiatedFeatureLevel,
			&m_deviceContext);
		
		if (hr == S_OK)
		{
			selectedDriverType = driverTypeIndex;
			break;
		}
	}
	if ( selectedDriverType > 0 )
		throw D3DException("Couldn't create a D3D Hardware-device, software render enabled."
		,__FILE__, __FUNCTION__, __LINE__);
}

void GraphicsWrapper::initBackBuffer()
{
	if( m_deviceContext == NULL ) {
		throw D3DException("DeviceContext not uninitialized.",__FILE__,
			__FUNCTION__,__LINE__);
	}

	if( m_device == NULL ) {
		throw D3DException("Device not uninitialized.",__FILE__,
			__FUNCTION__,__LINE__);
	}

	HRESULT hr = S_OK;
	ID3D11Texture2D* backBufferTexture;

	hr = m_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), 
		(LPVOID*)&backBufferTexture );

	if( FAILED(hr))
		throw D3DException("Failed to get backbuffer from swap chain.",__FILE__,
		__FUNCTION__,__LINE__);

	if( backBufferTexture == NULL) {
		throw D3DException("Failed to get backbuffer from swap chain. back buffer is NULL",
			__FILE__, __FUNCTION__,__LINE__);
	}

	hr = m_device->CreateRenderTargetView( backBufferTexture, NULL, &m_backBuffer );
	backBufferTexture->Release();
	if( FAILED(hr) )
		throw D3DException("Failed to create rendertargetview from back buffer.",
		__FILE__,__FUNCTION__,__LINE__);
}

void GraphicsWrapper::clearRenderTargets()
{
	m_deferredRenderer->clearRenderTargets();
	
	static float clearColor[4] = { 0.0, 0.0, 0.0f, 1.0f };
	m_deviceContext->ClearRenderTargetView( m_backBuffer,clearColor);
}

void GraphicsWrapper::updateRenderSceneInfo(const RendererSceneInfo& p_sceneInfo){
	m_renderSceneInfo = p_sceneInfo;
}

void GraphicsWrapper::mapSceneInfo(){
	m_renderSceneInfoBuffer->accessBuffer.setSceneInfo( m_renderSceneInfo );
	m_renderSceneInfoBuffer->update();
	m_renderSceneInfoBuffer->apply();
}

void GraphicsWrapper::setActiveShadow( int p_activeShadow ){
	m_perShadowBuffer->accessBuffer.currentShadow = p_activeShadow;
	m_perShadowBuffer->update();
	m_perShadowBuffer->apply();
}
void GraphicsWrapper::unmapPerShadowBuffer()
{
	m_perShadowBuffer->unApply();
}

void GraphicsWrapper::renderMesh(unsigned int p_meshId,
								 vector<InstanceData>* p_instanceList,
								 vector<AglMatrix>* p_boneMatrices, bool p_tesselate){
	Mesh* mesh = m_meshManager->getResource(p_meshId);

	unsigned int arraySize = 0;
	Texture** textureArray=NULL;

	if(!m_renderingShadows){
		arraySize = mesh->getMaterialInfo().SIZE;
		textureArray = new Texture*[arraySize];
		for (unsigned int i = 0; i < arraySize; i++)
		{
			unsigned int textureId;
			textureId = mesh->getMaterialInfo().getTextureType((MaterialInfo::TextureTypes)i);


			/************************************************************************/
			/* Check if the texture ID is active and get the texture resource or	*/
			/* set the value in the texture array to NULL							*/
			/************************************************************************/
			if(textureId >= 0)
				textureArray[i] = m_textureManager->getResource(textureId);
			else
				textureArray[i] = NULL;
		}
	}

	Buffer<InstanceData>* instanceBuffer;
	instanceBuffer = m_bufferFactory->createInstanceBuffer(&(*p_instanceList)[0],
														   p_instanceList->size());
	if (mesh->getSkeletonVertexBuffer() != NULL)
	{
		updateBoneMatrixTexture(p_boneMatrices);

		if (p_tesselate && mesh->getMaterialInfo().hasDisplacementMap)
		{
			setPrimitiveTopology(PrimitiveTopology::TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
			renderMeshInstanced( 
				mesh->getVertexBuffer()->getBufferPointer(),
				mesh->getVertexBuffer()->getElementSize(),
				mesh->getSkeletonVertexBuffer()->getBufferPointer(),
				mesh->getSkeletonVertexBuffer()->getElementSize(),
				mesh->getIndexBuffer()->getBufferPointer(),
				mesh->getIndexBuffer()->getElementCount(),
				textureArray, arraySize,
				instanceBuffer->getElementSize(), 
				instanceBuffer->getElementCount(),
				instanceBuffer->getBufferPointer(),
				m_deferredRenderer->getDeferredTessAnimatedBaseShader(), m_boneMatrixTexture);
		}
		else
		{
			setPrimitiveTopology(PrimitiveTopology::TRIANGLELIST);
			renderMeshInstanced( 
				mesh->getVertexBuffer()->getBufferPointer(),
				mesh->getVertexBuffer()->getElementSize(),
				mesh->getSkeletonVertexBuffer()->getBufferPointer(),
				mesh->getSkeletonVertexBuffer()->getElementSize(),
				mesh->getIndexBuffer()->getBufferPointer(),
				mesh->getIndexBuffer()->getElementCount(),
				textureArray, arraySize,
				instanceBuffer->getElementSize(), 
				instanceBuffer->getElementCount(),
				instanceBuffer->getBufferPointer(),
				m_deferredRenderer->getDeferredAnimatedBaseShader(), m_boneMatrixTexture);
		}
	}
	else
	{
		if (p_tesselate && mesh->getMaterialInfo().hasDisplacementMap)
		{
			setPrimitiveTopology(PrimitiveTopology::TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
			renderMeshInstanced( 
				mesh->getVertexBuffer()->getBufferPointer(),
				mesh->getVertexBuffer()->getElementSize(),
				NULL,
				0,
				mesh->getIndexBuffer()->getBufferPointer(),
				mesh->getIndexBuffer()->getElementCount(),
				textureArray, arraySize,
				instanceBuffer->getElementSize(), 
				instanceBuffer->getElementCount(),
				instanceBuffer->getBufferPointer(),
				m_deferredRenderer->getDeferredTessBaseShader(), NULL);
		}
		else
		{
			setPrimitiveTopology(PrimitiveTopology::TRIANGLELIST);
			renderMeshInstanced( 
				mesh->getVertexBuffer()->getBufferPointer(),
				mesh->getVertexBuffer()->getElementSize(),
				NULL,
				0,
				mesh->getIndexBuffer()->getBufferPointer(),
				mesh->getIndexBuffer()->getElementCount(),
				textureArray, arraySize,
				instanceBuffer->getElementSize(), 
				instanceBuffer->getElementCount(),
				instanceBuffer->getBufferPointer(),
				m_deferredRenderer->getDeferredBaseShader(), NULL);
		}
	}

	delete [] textureArray;
	delete [] instanceBuffer;
}
void GraphicsWrapper::renderLights( LightMesh* p_mesh,
								   vector<LightInstanceData>* p_instanceList )
{
	Buffer<LightInstanceData>* instanceBuffer;
	instanceBuffer = m_bufferFactory->createLightInstanceBuffer( &(*p_instanceList)[0],
		p_instanceList->size() );

	renderMeshInstanced( 
		p_mesh->getVertexBuffer()->getBufferPointer(),
		p_mesh->getVertexBuffer()->getElementSize(),
		NULL,
		NULL,
		p_mesh->getIndexBuffer()->getBufferPointer(),
		p_mesh->getIndexBuffer()->getElementCount(), 
		NULL, 0,
		instanceBuffer->getElementSize(),
		instanceBuffer->getElementCount(),
		instanceBuffer->getBufferPointer(),
		m_deferredRenderer->getDeferredLightShader(),
		NULL);

	delete instanceBuffer;
	instanceBuffer = NULL;
}

void GraphicsWrapper::setRasterizerStateSettings(RasterizerState::Mode p_state, 
												 bool p_allowWireframeModeOverride)
{
	RasterizerState::Mode state = m_deferredRenderer->getCurrentRasterizerStateType();
	// accept rasterizer state change if not in wireframe mode or 
	// if set to not allow wireframe mode
	if (!m_wireframeMode || !p_allowWireframeModeOverride)
		m_deferredRenderer->setRasterizerStateSettings(p_state);
	else if (state != RasterizerState::WIREFRAME) 
	{   
		// otherwise, force wireframe(if not already set)
		m_deferredRenderer->setRasterizerStateSettings(RasterizerState::WIREFRAME_NOCULL);
	}
}

void GraphicsWrapper::setBlendStateSettings( BlendState::Mode p_state ){
	m_deferredRenderer->setBlendState( p_state );
}

void GraphicsWrapper::setScissorRegion( int x, int y, int width, int height )
{
	D3D11_RECT scissor_rect;
	scissor_rect.left = x;
	scissor_rect.right = x + width;
	scissor_rect.top = y;
	scissor_rect.bottom = y + height;

	m_deviceContext->RSSetScissorRects(1, &scissor_rect);
}

void GraphicsWrapper::setPrimitiveTopology( PrimitiveTopology::Mode p_state ){
	m_deviceContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(p_state));
}

void GraphicsWrapper::setBaseRenderTargets(){
	m_deferredRenderer->setBasePassRenderTargets();
}

void GraphicsWrapper::setLightRenderTargets(){
	m_deferredRenderer->setLightRenderTargets();
}

void GraphicsWrapper::setDofRenderTargets(){
	m_deferredRenderer->setDofRenderTargets();
}

void GraphicsWrapper::setComposedRenderTargetWithNoDepthStencil(){
	m_deviceContext->OMSetRenderTargets( 1, &m_backBuffer, NULL );
}

void GraphicsWrapper::setShadowViewProjection( const AglMatrix& p_viewProj ){
	m_shadowShader->sendViewProjection(p_viewProj);
	m_shadowShader->apply();
}

void GraphicsWrapper::setShadowViewProjections( AglMatrix* p_viewProj ){
	m_shadowShader->sendViewProjections(p_viewProj);
	m_shadowShader->apply();
}

void GraphicsWrapper::mapDepthAndNormal(){
	m_deferredRenderer->mapNormal();
	m_deferredRenderer->mapDepth();
}

void GraphicsWrapper::unmapDepthAndNormal(){
	m_deferredRenderer->unmapNormal();
	m_deferredRenderer->unmapDepth();
}

void GraphicsWrapper::mapShadows( int* p_activeShadows ){
	int startSlot = 4;
	for(int i = 0; i < MAXSHADOWS; i++){
		if(p_activeShadows[i] != -1){
			m_deviceContext->PSSetShaderResources( startSlot, 1, m_shadowMapRenderer->getShadowMap(i));
		}
	}
}

void GraphicsWrapper::unmapShadows( int* p_activeShadows ){

	ID3D11ShaderResourceView* nulz = NULL;
	int startSlot = 4;
	for(int i = 0; i < MAXSHADOWS; i++){
		if(p_activeShadows[i] != -1){
			m_deviceContext->PSSetShaderResources( startSlot, 1, &nulz);
		}
	}
}

void GraphicsWrapper::renderGUIMeshList( unsigned int p_meshId, 
									 vector<InstanceData>* p_instanceList )
{
	Mesh* mesh = m_meshManager->getResource( p_meshId );
	Texture* tex = m_textureManager->getResource( 
		mesh->getMaterialInfo().getTextureType(MaterialInfo::DIFFUSEMAP) );

	Buffer<InstanceData>* instanceBuffer;
	instanceBuffer = m_bufferFactory->createInstanceBuffer( &(*p_instanceList)[0],
		p_instanceList->size() );

	m_guiShader->apply();
	renderSingleGUIMesh(mesh,tex);

	delete instanceBuffer;
}


void GraphicsWrapper::flipBackBuffer()
{
	m_swapChain->Present( m_vsync, 0);
}

void GraphicsWrapper::mapRandomVecTexture()
{
	m_deviceContext->PSSetShaderResources(4,1,
		&m_textureManager->getResource(m_randomNormalTextures)->data);
}


// ModelResource* GraphicsWrapper::createModelFromFile(const string& p_name,
// 						   const string* p_path,bool p_isPrimitive)
// {
// 	return m_modelFactory->createModelResource(p_name,p_path,p_isPrimitive);
// }

vector<ModelResource*>* GraphicsWrapper::createModelsFromFile(const string& p_name,
									 const string* p_path,bool p_isPrimitive)
{
	return m_modelFactory->createModelResources(p_name,p_path,p_isPrimitive);
}

unsigned int GraphicsWrapper::createMeshFromRaw( const string& p_name, 
												int p_numVertices, PNTTBVertex* p_vertices, 
												int p_numIndices, DIndex* p_indices, 
												int p_textureId )
{
	// check if resource already exists
	unsigned int meshResultId = 0;
	int meshFoundId = m_meshManager->getResourceId(p_name);
	if (meshFoundId==-1)  // if it does not exist, create new
	{
		Mesh* mesh = m_bufferFactory->createMeshFromPNTTBVerticesAndIndices( p_numVertices,
			p_vertices, p_numIndices, p_indices );
		meshResultId = (int)m_meshManager->addResource( p_name, mesh );

		if( p_textureId != -1 )
		{
			MaterialInfo materialInfo;
			materialInfo.setTextureId( MaterialInfo::DIFFUSEMAP, p_textureId);
			mesh->setMaterial( materialInfo );
		}

	}
	else // the mesh already exists
	{
		meshResultId = static_cast<unsigned int>(meshFoundId);
	}
	return meshResultId;
}


void GraphicsWrapper::releaseMesh( int p_meshId )
{
	if (m_meshManager)
		m_meshManager->removeResource(p_meshId);
}

void GraphicsWrapper::releaseMesh( const string& p_name )
{
	if (m_meshManager)
		m_meshManager->removeResource(p_name);
}


unsigned int GraphicsWrapper::createTexture( const string& p_name, 
											 const string& p_path)
{
	return m_textureFactory->createTexture(p_name,p_path);
}

unsigned int GraphicsWrapper::createTexture( const byte* p_source, int p_width,
	int p_height, int p_pitch, int p_bitLevel, TextureParser::TEXTURE_TYPE p_type )
{
	return m_textureFactory->createTexture(p_source,p_width,p_height,p_pitch, p_bitLevel,
		p_type);
}

int GraphicsWrapper::getMeshId( const string& p_name )
{
	return m_meshManager->getResourceId(p_name);
}

ID3D11Device* GraphicsWrapper::getDevice()
{
	return m_device;
}

ID3D11DeviceContext* GraphicsWrapper::getDeviceContext()
{
	return m_deviceContext;
}

void GraphicsWrapper::hookUpAntTweakBar()
{
	m_deferredRenderer->hookUpAntTweakBar();
}

int GraphicsWrapper::getWindowWidth()
{
	return m_width;
}

int GraphicsWrapper::getWindowHeight()
{
	return m_height;
}

pair<int,int> GraphicsWrapper::getScreenPixelPosFromNDC( float p_x, float p_y )
{
	return getScreenPixelPosFromNDC(p_x, p_y, m_width, m_height);
}

pair<int,int> GraphicsWrapper::getScreenPixelPosFromNDC( float p_x, float p_y,
														int p_width, int p_height )
{
	// NDC -> Screenspace
	int screenSpaceX = static_cast<int>((p_x+1.0f)/2 * (p_width));
	int screenSpaceY = static_cast<int>((p_y+1.0f)/2 * (p_height));
	return pair<int,int>(screenSpaceX,screenSpaceY);
}

pair<float,float> GraphicsWrapper::getNDCPosFromScreenPixel( int p_x, int p_y )
{
	// Screenspace -> NDC
	float ndcX = 2.0f * p_x / m_width - 1.0f;
	float ndcY = 2.0f * p_y / m_height - 1.0f;;
	return pair<float,float>(ndcX, ndcY);
}


void GraphicsWrapper::changeBackbufferRes( int p_width, int p_height ){
	m_width = p_width;
	m_height = p_height;

	m_deviceContext->OMSetRenderTargets(0, 0, 0);

	releaseBackBuffer();
	m_deferredRenderer->releaseRenderTargetsAndDepthStencil();

	HRESULT hr;
	// Resize swap chain to window's size.
	hr = m_swapChain->ResizeBuffers(0, p_width, p_height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	if(FAILED(hr))
		throw D3DException(hr,__FILE__,__FUNCTION__,__LINE__);

	initBackBuffer();
	resetViewportToStdSize();

	m_deferredRenderer->initRendertargetsAndDepthStencil( m_width, m_height );
}

void GraphicsWrapper::releaseBackBuffer()
{
	SAFE_RELEASE( m_backBuffer );
}
void GraphicsWrapper::changeToWindowed( bool p_windowed )
{
	HRESULT hr = S_OK;
	m_windowed = p_windowed;
	hr = m_swapChain->SetFullscreenState((BOOL)!m_windowed,nullptr);
	if( FAILED(hr) && hr!=DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
		throw D3DException(hr,__FILE__,__FUNCTION__,__LINE__);
}

void GraphicsWrapper::setWireframeMode( bool p_wireframe )
{
	m_wireframeMode = p_wireframe;
}

void GraphicsWrapper::renderParticleSystem( ParticleSystemAndTexture* p_system,
										   InstanceData p_transform )
{
	Texture* texture = m_textureManager->getResource( p_system->textureIdx );
	m_particleRenderer->renderParticleSystem( &p_system->particleSystem,
	 p_transform, texture,p_system->uvRect );
}

void GraphicsWrapper::setParticleRenderState()
{
	m_deviceContext->OMSetRenderTargets(1,&m_backBuffer,m_deferredRenderer->getDepthStencil());
}

void GraphicsWrapper::mapDepth(){
	m_deferredRenderer->mapDepth();
}

void GraphicsWrapper::unmapDepth(){
	m_deferredRenderer->unmapDepth();
}

void GraphicsWrapper::generateEffects(){
	m_deferredRenderer->generateEffects();
}

void GraphicsWrapper::generateLowRes(){
	m_deferredRenderer->generateLowRes();
}

void GraphicsWrapper::renderComposeStage(){
	m_deferredRenderer->renderComposeStage();
}

void GraphicsWrapper::mapGbuffers(){
	m_deferredRenderer->mapGbuffers();
}

void GraphicsWrapper::unmapGbuffers(){
	m_deferredRenderer->unmapGbuffers();
}

void GraphicsWrapper::mapDofBuffers(){
	m_deferredRenderer->mapDofBuffers();
}

void GraphicsWrapper::unmapDofBuffers(){
	m_deferredRenderer->unmapDofBuffers();
}


void GraphicsWrapper::unmapVariousStagesAfterCompose(){
	ID3D11ShaderResourceView* nulz = NULL;
	//m_deferredRenderer->unmapShaderResourcesForComposePass();
	//m_deferredRenderer->unmapDofShaderResources();
	unmapGbuffers();
	unmapDofBuffers();
	m_deviceContext->PSSetShaderResources(3,1,
		&m_textureManager->getResource(m_solidWhiteTexture)->data);
}

void GraphicsWrapper::renderSingleGUIMesh( Mesh* p_mesh, Texture* p_texture )
{
	p_mesh->getVertexBuffer()->apply();
	p_mesh->getIndexBuffer()->apply();

	// set texture
	//HACK: fix so that a placeholder tex is used instead of the last working one
	if( p_texture != NULL ){
		m_deviceContext->PSSetShaderResources(0,1,&(p_texture->data));
	}
	// Draw instanced data
	m_deviceContext->DrawIndexed(p_mesh->getIndexBuffer()->getElementCount(),0,0);
}

void GraphicsWrapper::renderMeshInstanced( void* p_vertexBufferRef, UINT32 p_vertexSize, 
										  void* p_vertexAnimationBufferRef, UINT32 p_vertexAnimationSize,
										  void* p_indexBufferRef, UINT32 p_indexElementCount, 
										  Texture** p_textureArray, 
										  unsigned int p_textureArraySize, 
										  UINT32 p_instanceDataSize,
										  UINT32 p_instanceElementCount,
										  void* p_instanceRef,
										  ShaderBase* p_shader,
											Texture* p_boneMatrixTexture)
{

	if(p_textureArray){
		UINT startSlot = 0;
		UINT numViews = 1;
		for (unsigned int i = 0; i < p_textureArraySize; i++)
		{
			if(p_textureArray[i] != NULL)
			{
				if(static_cast<MaterialInfo::TextureTypes>(i) != MaterialInfo::DISPLACEMENTMAP){
				// set textures
					m_deviceContext->PSSetShaderResources(startSlot , numViews, 
						&p_textureArray[i]->data );
				}
				else{
					m_deviceContext->DSSetShaderResources(startSlot , numViews, 
						&p_textureArray[i]->data );
				}
				startSlot++;

			}
		}
	}

	if (p_boneMatrixTexture)
	{
		m_deviceContext->VSSetShaderResources(0, 1, &p_boneMatrixTexture->data);
	}

	if (!p_vertexAnimationBufferRef)
	{
		UINT strides[2] = { p_vertexSize, p_instanceDataSize };
		UINT offsets[2] = { 0, 0 };
		// Set up an array of the buffers for the vertices
		ID3D11Buffer* buffers[2] = { 
			static_cast<ID3D11Buffer*>(p_vertexBufferRef), 
			static_cast<ID3D11Buffer*>(p_instanceRef) 
		};

		// Set array of buffers to context 
		m_deviceContext->IASetVertexBuffers(0, 2, buffers, strides, offsets);
	}
	else
	{
		UINT strides[3] = { p_vertexSize, p_vertexAnimationSize, p_instanceDataSize };
		UINT offsets[3] = { 0, 0, 0 };
		// Set up an array of the buffers for the vertices
		ID3D11Buffer* buffers[3] = { 
			static_cast<ID3D11Buffer*>(p_vertexBufferRef), 
			static_cast<ID3D11Buffer*>(p_vertexAnimationBufferRef),
			static_cast<ID3D11Buffer*>(p_instanceRef) 
		};

		// Set array of buffers to context 
		m_deviceContext->IASetVertexBuffers(0, 3, buffers, strides, offsets);
	}

	// And the index buffer
	m_deviceContext->IASetIndexBuffer(static_cast<ID3D11Buffer*>(p_indexBufferRef), 
		DXGI_FORMAT_R32_UINT, 0);

	if(!m_renderingShadows)
		p_shader->apply();

	// Draw instanced data
	m_deviceContext->DrawIndexedInstanced( p_indexElementCount, p_instanceElementCount, 0,0,0);
}

void GraphicsWrapper::setViewportToLowRes()
{
	m_deferredRenderer->setViewPortSize(
		m_width/m_renderSceneInfo.lowResDivider,
		m_height/m_renderSceneInfo.lowResDivider );
}

void GraphicsWrapper::setViewportToShadowMapSize()
{
	m_deferredRenderer->setViewPortSize(
		(float)(ShadowMapRenderer::SHADOWMAPSIZE),
		(float)(ShadowMapRenderer::SHADOWMAPSIZE));	
}

void GraphicsWrapper::resetViewportToStdSize()
{
	m_deferredRenderer->setViewPortSize( m_width, m_height );
}

void GraphicsWrapper::setShadowMapAsRenderTarget( unsigned int p_shadowMapIdx ){
	m_shadowMapRenderer->mapShadowMapToRenderTarget(p_shadowMapIdx);
}

void GraphicsWrapper::setRenderingShadows(){
	m_renderingShadows = true;
}

void GraphicsWrapper::stopedRenderingShadows(){
	m_renderingShadows = false;
}

unsigned int GraphicsWrapper::generateShadowMap()
{
	return m_shadowMapRenderer->createANewShadowMap();
}

GPUTimer* GraphicsWrapper::getGPUTimer()
{
	return m_gpuTimer;
}

int GraphicsWrapper::getEmptyTexture()
{
	return m_solidWhiteTexture;
}

void GraphicsWrapper::initBoneMatrixTexture()
{
	D3D11_TEXTURE1D_DESC desc;
	desc.MiscFlags = 0;
	desc.Width = 16384; // 2^14
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //One row in the matrix
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT res = m_device->CreateTexture1D(&desc, NULL, &m_boneMatrixResource );

	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	ID3D11ShaderResourceView* srv;
	m_device->CreateShaderResourceView(m_boneMatrixResource, &srDesc, &srv );
	m_boneMatrixTexture = new Texture(srv);

	D3D11_MAPPED_SUBRESOURCE mappedTex;
	res = m_deviceContext->Map(m_boneMatrixResource, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_DISCARD, 0, &mappedTex);

	float* values = (float*)mappedTex.pData;
	for( UINT col = 0; col < desc.Width; )
	{
		//Row1
		values[col++] = 1;
		values[col++] = 0;
		values[col++] = 0; 
		values[col++] = 0; 
		//Row2
		values[col++] = 0;
		values[col++] = 1;
		values[col++] = 0; 
		values[col++] = 0; 
		//Row3
		values[col++] = 0;
		values[col++] = 0;
		values[col++] = 1; 
		values[col++] = 0; 
		//Row4
		values[col++] = 0;
		values[col++] = 0;
		values[col++] = 0; 
		values[col++] = 1; 
	}
	m_deviceContext->Unmap(m_boneMatrixResource, D3D11CalcSubresource(0, 0, 1));
}	
void GraphicsWrapper::updateBoneMatrixTexture(vector<AglMatrix>* p_data)
{
	D3D11_MAPPED_SUBRESOURCE mappedTex;
	m_deviceContext->Map(m_boneMatrixResource, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_DISCARD, 0, &mappedTex);

	float* values = (float*)mappedTex.pData;

	for (unsigned int i = 0; i < p_data->size(); i++)
	{
		AglMatrix am = (*p_data)[i];
		int pos = i*16;

		//Row1
		values[pos] = am[0];
		values[pos+1] = am[1];
		values[pos+2] = am[2];
		values[pos+3] = am[3];
		//Row2
		values[pos+4] = am[4];
		values[pos+5] = am[5];
		values[pos+6] = am[6]; 
		values[pos+7] = am[7]; 
		//Row3
		values[pos+8] = am[8];
		values[pos+9] = am[9];
		values[pos+10] =am[10];
		values[pos+11] =am[11];
		//Row4
		values[pos+12] = am[12];
		values[pos+13] = am[13];
		values[pos+14] = am[14];
		values[pos+15] = am[15];
	}

	m_deviceContext->Unmap(m_boneMatrixResource, D3D11CalcSubresource(0, 0, 1));
}

MaterialInfo GraphicsWrapper::getMaterialInfoFromMeshID( unsigned int p_index )
{
	return m_meshManager->getResource(p_index)->getMaterialInfo();
}

void GraphicsWrapper::isInEditMode(bool p_isInEditMode)
{
	m_deferredRenderer->toggleEditMode(p_isInEditMode);
}

void GraphicsWrapper::setDeferredDofValues(float p_startNear, float p_stopNear,
	float p_startFar, float p_stopFar)
{
	m_deferredRenderer->setDofValues(p_startNear, p_stopNear, p_startFar, p_stopFar);
}

