// =======================================================================================
//                                      GraphicsWrapper
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	A wrapper for Direct3D functionality, handles objects for creating and
/// rendering graphics data.
///        
/// # GraphicsWrapper
/// Detailed description.....
/// Created on: 27-11-2012 
///---------------------------------------------------------------------------------------
#pragma once

#include "Buffer.h"
#include "InstanceData.h"
#include "TextureParser.h"
#include "ModelExtendedFactory.h"
#include "TextureFactory.h"
#include "RenderSceneInfoCBuffer.h"
#include "RenderStateEnums.h"
#include "RendererSceneInfo.h"
#include <Globals.h>
#include <ResourceManager.h>
#include <d3d11.h>

class AglParticleSystem;
class BufferFactory;
class DeferredBaseShader;
class DeferredComposeShader;
class DeferredRenderer;
class Mesh;
class ParticleRenderer;
class ShaderFactory;
class TextureParser;
class TextureFactory;
class GUIShader;
class ShaderBase;
class ShadowMapRenderer;
class ShadowShader;
class GPUTimer;

struct LightInstanceData;
struct Model;
struct Texture;
struct AglMatrix;
struct PerShadowCBuffer;
struct ParticleSystemAndTexture;
struct MaterialInfo;

class GraphicsWrapper
{
public:
	GraphicsWrapper( HWND p_hWnd, int p_width, int p_height, bool p_windowed,
		bool p_useHdr, bool p_enableEffects, int p_vsync );
	virtual ~GraphicsWrapper();
	///-----------------------------------------------------------------------------------
	/// Clears the back buffer with a non black color
	/// \return void
	///-----------------------------------------------------------------------------------
	void clearRenderTargets();

	///-----------------------------------------------------------------------------------
	/// Sets up the frame, prepares the renderer for draw calls.
	///-----------------------------------------------------------------------------------
	void mapSceneInfo();

	void renderMesh(unsigned int p_meshId,vector<InstanceData>* p_instanceList,
		vector<AglMatrix>* p_boneMatrices, bool p_tesselate);

	///-----------------------------------------------------------------------------------
	/// Set the current rasterizer state. By default it will allow to be overriden by the 
	/// wireframe mode setting.
	/// \param p_state
	/// \param p_allowWireframeModeOverride If true: will ignore change if global 
	/// wireframe mode is on. Will ignore wireframe mode if set to false. True by default.
	///-----------------------------------------------------------------------------------
	void setRasterizerStateSettings(RasterizerState::Mode p_state, 
									bool p_allowWireframeModeOverride=true);
	void setBlendStateSettings( BlendState::Mode p_state );
	void setScissorRegion(int x, int y, int width, int height);
	void setPrimitiveTopology( PrimitiveTopology::Mode p_state);
	void setBaseRenderTargets();
	void setLightRenderTargets();
	void setDofRenderTargets();
	void setComposedRenderTargetWithNoDepthStencil();
	void setParticleRenderState();

	void setViewportToLowRes();
	void setViewportToShadowMapSize();
	void resetViewportToStdSize();

	void setShadowMapAsRenderTarget(unsigned int p_shadowMapIdx);
	void setShadowViewProjection(const AglMatrix& p_viewProj);
	void setShadowViewProjections( AglMatrix* p_viewProj );
	void setRenderingShadows();
	void stopedRenderingShadows();
	void setActiveShadow( int p_activeShadow );
	void unmapPerShadowBuffer();

	///-----------------------------------------------------------------------------------
	/// Render compiled rocket geometry. Use this with libRocket so that the correct
	/// shader is used.
	///-----------------------------------------------------------------------------------
	void renderGUIMeshList( unsigned int p_meshId, vector<InstanceData>* p_instanceList );

	//void mapDeferredBaseToShader();
	void mapDepthAndNormal();
	void unmapDepthAndNormal();
	void mapShadows( int* p_activeShadows );
	void unmapShadows( int* p_activeShadows );

	void mapDepth();
	void unmapDepth();
	void renderLights( LightMesh* p_mesh, vector<LightInstanceData>* p_instanceList );
	///-----------------------------------------------------------------------------------
	/// Switch the back buffer so that the current render target is presented
	///-----------------------------------------------------------------------------------
	void flipBackBuffer();


	void mapRandomVecTexture();

// 	ModelResource* createModelFromFile(const string& p_name,
// 							   const string* p_path);

	vector<ModelResource*>* createModelsFromFile(const string& p_name,
		const string* p_path,bool p_isPrimitive);

	// This is the preferred method for creating meshes from raw data
	unsigned int createMeshFromRaw(const string& p_name,
		int p_numVertices, PNTTBVertex* p_vertices, 
		int p_numIndices, DIndex* p_indices,
		int p_textureId);
	void releaseMesh(int p_meshId);
	void releaseMesh(const string& p_name);
	unsigned int createTexture(const string& p_name,
							   const string& p_path);
	unsigned int createTexture( const byte* p_source, int p_width, int p_height,
		int p_pitch, int p_bitLevel, TextureParser::TEXTURE_TYPE p_type );
	int getMeshId( const string& p_name );
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();
	void hookUpAntTweakBar();
	int getWindowWidth();
	int getWindowHeight();
	pair<int,int> getScreenPixelPosFromNDC(float p_x, float p_y);
	pair<int,int> getScreenPixelPosFromNDC(float p_x, float p_y, int p_width, int p_height);
	pair<float,float> getNDCPosFromScreenPixel(int p_x, int p_y);

	void changeBackbufferRes( int p_width, int p_height );

	void changeToWindowed(bool p_windowed);

	///-----------------------------------------------------------------------------------
	/// Sets the global wireframe mode. Causes everything to be displayed in wireframe.
	/// Separate rasterizer change calls can force to ignore global wireframe mode 
	/// explicitly though.
	///-----------------------------------------------------------------------------------
	void setWireframeMode(bool p_wireframe);

	void renderParticleSystem( ParticleSystemAndTexture* p_system, InstanceData p_transform );
	void generateEffects();
	void generateLowRes();
	void renderComposeStage();
	void updateRenderSceneInfo(const RendererSceneInfo& p_sceneInfo);
	void mapGbuffers();
	void unmapGbuffers();
	void mapDofBuffers();
	void unmapDofBuffers();
	void unmapVariousStagesAfterCompose();
	void isInEditMode(bool p_isInEditMode);
	unsigned int generateShadowMap();
	GPUTimer* getGPUTimer();
	int getEmptyTexture();
	MaterialInfo getMaterialInfoFromMeshID(unsigned int p_index);
	ResourceManager<Texture>* getTextureManager()
	{
		return m_textureManager;
	}
	void setDeferredDofValues(float p_startNear, float p_stopNear, float p_startFar,
		float p_stopFar);

private:
	void renderSingleGUIMesh(Mesh* p_mesh, Texture* p_texture);
	void initSwapChain(HWND p_hWnd);

	///-----------------------------------------------------------------------------------
	/// Initializes the graphics card. Needs an initiated SwapChainDesc with a HWND. Call initSwapChain() before calling this function
	///-----------------------------------------------------------------------------------
	void initHardware();

	void releaseBackBuffer();
	void initBackBuffer();
	void renderMeshInstanced(void* p_vertexBufferRef, UINT32 p_vertexSize,
		void* p_vertexAnimationBufferRef, UINT32 p_vertexAnimationSize,
		void* p_indexBufferRef, UINT32 p_indexElementCount,
		Texture** p_textureArray,
		unsigned int p_textureArraySize,
		UINT32 p_instanceDataSize, UINT32 p_instanceElementCount,
		void* p_instanceRef,
		ShaderBase* p_shader, Texture* p_boneMatrixTexture);

	void initBoneMatrixTexture();
	void updateBoneMatrixTexture(vector<AglMatrix>* p_data);
private:
	ID3D11Device*			m_device;
	ID3D11DeviceContext*	m_deviceContext;
	DXGI_SWAP_CHAIN_DESC	m_swapChainDesc;
	IDXGISwapChain*			m_swapChain;
	D3D_FEATURE_LEVEL		m_featureLevel;

	DeferredRenderer*		m_deferredRenderer;
	ParticleRenderer*		m_particleRenderer;
	ShadowMapRenderer*		m_shadowMapRenderer;

	ID3D11RenderTargetView* m_backBuffer;

	DeferredBaseShader*		m_deferredBaseShader;
	ShadowShader*			m_shadowShader;

	// Creation & storage
	ShaderFactory*			m_shaderFactory;
	BufferFactory*			m_bufferFactory;
	TextureFactory*			m_textureFactory;
	ModelExtendedFactory*	m_modelFactory;

	ResourceManager<Mesh>*		m_meshManager;
	ResourceManager<Texture>*	m_textureManager;


	RendererSceneInfo		m_renderSceneInfo;
	Buffer<RenderSceneInfoCBuffer>* m_renderSceneInfoBuffer;
	Buffer<PerShadowCBuffer>*		m_perShadowBuffer;

	GUIShader*				m_guiShader;

	GPUTimer*	m_gpuTimer;

	int m_height;
	int m_width;
	int m_lowResDivider;

	int m_randomNormalTextures;
	int m_solidWhiteTexture;

	int m_vsync;

	bool m_windowed;
	bool m_enableHdr;
	bool m_enableEffects;
	bool m_wireframeMode;
	bool m_renderingShadows;

	Texture* m_boneMatrixTexture;
	ID3D11Texture1D* m_boneMatrixResource;
};