#include "ShaderFactory.h"
#include "BufferFactory.h"
#include "D3DException.h"
#include "DeferredBaseShader.h"
#include "DeferredComposeShader.h"
#include "GUIShader.h"
#include "ParticleShader.h"
#include "ShadowShader.h"
#include "LightShader.h"
#include "DeferredAnimatedBaseShader.h"
#include "DeferredTessBaseShader.h"
#include "DeferredTessAnimatedBaseShader.h"
#include "DeferredPostProcessing.h"

ShaderFactory::ShaderFactory(ID3D11Device* p_device, ID3D11DeviceContext* p_deviceContext, 
							 D3D_FEATURE_LEVEL p_featureLevel)
{
	m_device = p_device;
	m_deviceContext = p_deviceContext;
	switch(m_device->GetFeatureLevel())
	{
	case D3D_FEATURE_LEVEL_10_1:
	case D3D_FEATURE_LEVEL_10_0:
		m_shaderModelVersion = "4_0"; break;
	default:
		m_shaderModelVersion = "5_0"; break;
	}
	m_bufferFactory = new BufferFactory(m_device,m_deviceContext);

	switch( p_featureLevel )
	{
	case D3D_FEATURE_LEVEL_10_0:
		m_shaderModelVersion = "4_0";
		break;

	case D3D_FEATURE_LEVEL_10_1:
		m_shaderModelVersion = "4_0";
		break;

	case D3D_FEATURE_LEVEL_11_0:
		m_shaderModelVersion = "5_0";
		break;

	case D3D_FEATURE_LEVEL_11_1:
		m_shaderModelVersion = "5_0";
		break;
	}
}


ShaderFactory::~ShaderFactory()
{
	delete m_bufferFactory;
}

DeferredBaseShader* ShaderFactory::createDeferredBaseShader( const LPCWSTR& p_vertexPath, const LPCWSTR& p_pixelPath )
{
	DeferredBaseShader* newDeferredBaseShader = NULL;
	ID3D11SamplerState* samplerState = NULL;
	ID3D11InputLayout* inputLayout = NULL;

	VSData* vertexData = new VSData();
	PSData* pixelData = new PSData();

	vertexData->stageConfig = new ShaderStageConfig(p_vertexPath,"VS",m_shaderModelVersion);
	pixelData->stageConfig = new ShaderStageConfig(p_pixelPath,"PS",m_shaderModelVersion);

	createAllShaderStages(vertexData,pixelData);
	createInstancedPNTTBVertexInputLayout(vertexData,&inputLayout);

	ShaderVariableContainer shaderInitData;
	createShaderInitData(&shaderInitData,inputLayout,vertexData,pixelData);

	return new DeferredBaseShader(shaderInitData);
}

DeferredAnimatedBaseShader* ShaderFactory::createDeferredAnimatedShader(const LPCWSTR& p_vertexPath, const LPCWSTR& p_pixelPath)
{
	ID3D11SamplerState* samplerState = NULL;
	ID3D11InputLayout* inputLayout = NULL;

	VSData* vertexData = new VSData();
	PSData* pixelData = new PSData();

	vertexData->stageConfig = new ShaderStageConfig(p_vertexPath,"VS",m_shaderModelVersion);
	pixelData->stageConfig = new ShaderStageConfig(p_pixelPath,"PS",m_shaderModelVersion);

	createAllShaderStages(vertexData,pixelData);
	createInstancedAnimatedPNTTBVertexInputLayout(vertexData,&inputLayout);

	ShaderVariableContainer shaderInitData;
	createShaderInitData(&shaderInitData,inputLayout,vertexData,pixelData);

	return new DeferredAnimatedBaseShader(shaderInitData);
}

DeferredTessBaseShader* ShaderFactory::createDeferredTessBaseShader(const LPCWSTR& p_filePath)
{
	DeferredBaseShader* newDeferredBaseShader = NULL;
	ID3D11SamplerState* samplerState = NULL;
	ID3D11InputLayout* inputLayout = NULL;

	VSData* vertexData = new VSData();
	HSData* hullData	= new HSData();
	DSData* domainData	= new DSData();
	PSData* pixelData = new PSData();

	vertexData->stageConfig = new ShaderStageConfig(p_filePath,"VS",m_shaderModelVersion);
	hullData->stageConfig = new ShaderStageConfig(p_filePath,"HS",m_shaderModelVersion);
	domainData->stageConfig = new ShaderStageConfig(p_filePath,"DS",m_shaderModelVersion);
	pixelData->stageConfig = new ShaderStageConfig(p_filePath,"PS",m_shaderModelVersion);

	createAllShaderStages(vertexData,pixelData, NULL, hullData, domainData);
	createInstancedPNTTBVertexInputLayout(vertexData,&inputLayout);

	ShaderVariableContainer shaderInitData;
	createShaderInitData(&shaderInitData,inputLayout,vertexData,pixelData,
		NULL, hullData, domainData);

	return new DeferredTessBaseShader(shaderInitData);
}

DeferredTessBaseShader* ShaderFactory::createDeferredTesselationBaseShader( const LPCWSTR& p_vertexPath, 
																		   const LPCWSTR& p_hullPath, 
																		   const LPCWSTR& p_domainPath, 
																		   const LPCWSTR& p_pixelPath )
{
	DeferredBaseShader* newDeferredBaseShader = NULL;
	ID3D11SamplerState* samplerState = NULL;
	ID3D11InputLayout* inputLayout = NULL;

	VSData* vertexData = new VSData();
	HSData* hullData	= new HSData();
	DSData* domainData	= new DSData();
	PSData* pixelData = new PSData();

	vertexData->stageConfig = new ShaderStageConfig(p_vertexPath,"VS",m_shaderModelVersion);
	hullData->stageConfig = new ShaderStageConfig(p_hullPath,"HS",m_shaderModelVersion);
	domainData->stageConfig = new ShaderStageConfig(p_domainPath,"DS",m_shaderModelVersion);
	pixelData->stageConfig = new ShaderStageConfig(p_pixelPath,"PS",m_shaderModelVersion);

	createAllShaderStages(vertexData,pixelData, NULL, hullData, domainData);
	createInstancedPNTTBVertexInputLayout(vertexData,&inputLayout);

	ShaderVariableContainer shaderInitData;
	createShaderInitData(&shaderInitData,inputLayout,vertexData,pixelData,
		NULL, hullData, domainData);

	return new DeferredTessBaseShader(shaderInitData);
}


DeferredTessAnimatedBaseShader* ShaderFactory::createDeferredTessAnimatedShader(const LPCWSTR& p_filePath)
{
	ID3D11SamplerState* samplerState = NULL;
	ID3D11InputLayout* inputLayout = NULL;

	VSData* vertexData = new VSData();
	HSData* hullData	= new HSData();
	DSData* domainData	= new DSData();
	PSData* pixelData = new PSData();

	vertexData->stageConfig = new ShaderStageConfig(p_filePath,"VS",m_shaderModelVersion);
	hullData->stageConfig = new ShaderStageConfig(p_filePath,"HS",m_shaderModelVersion);
	domainData->stageConfig = new ShaderStageConfig(p_filePath,"DS",m_shaderModelVersion);
	pixelData->stageConfig = new ShaderStageConfig(p_filePath,"PS",m_shaderModelVersion);

	createAllShaderStages(vertexData,pixelData, NULL, hullData, domainData);
	createInstancedAnimatedPNTTBVertexInputLayout(vertexData,&inputLayout);

	ShaderVariableContainer shaderInitData;
	createShaderInitData(&shaderInitData,inputLayout,vertexData,pixelData, NULL, hullData, domainData);

	return new DeferredTessAnimatedBaseShader(shaderInitData);
}

DeferredTessAnimatedBaseShader* ShaderFactory::createDeferredTessAnimatedShader( const LPCWSTR& p_vertexPath, 
																				const LPCWSTR& p_hullPath, 
																				const LPCWSTR& p_dominPath, 
																				const LPCWSTR& p_pixelPath )
{
	ID3D11SamplerState* samplerState = NULL;
	ID3D11InputLayout* inputLayout = NULL;

	VSData* vertexData = new VSData();
	HSData* hullData	= new HSData();
	DSData* domainData	= new DSData();
	PSData* pixelData = new PSData();

	vertexData->stageConfig = new ShaderStageConfig(p_vertexPath,"VS",m_shaderModelVersion);
	hullData->stageConfig = new ShaderStageConfig(p_hullPath,"HS",m_shaderModelVersion);
	domainData->stageConfig = new ShaderStageConfig(p_dominPath,"DS",m_shaderModelVersion);
	pixelData->stageConfig = new ShaderStageConfig(p_pixelPath,"PS",m_shaderModelVersion);

	createAllShaderStages(vertexData,pixelData, NULL, hullData, domainData);
	createInstancedAnimatedPNTTBVertexInputLayout(vertexData,&inputLayout);

	ShaderVariableContainer shaderInitData;
	createShaderInitData(&shaderInitData,inputLayout,vertexData,pixelData, NULL, hullData, domainData);

	return new DeferredTessAnimatedBaseShader(shaderInitData);
}


LightShader* ShaderFactory::createLightShader( const LPCWSTR& p_filePath )
{
	VSData* vertexData = new VSData();
	vertexData->stageConfig = new ShaderStageConfig(p_filePath, "VS", m_shaderModelVersion);

	PSData* pixelData = new PSData();
	pixelData->stageConfig = new ShaderStageConfig(p_filePath, "PS", m_shaderModelVersion);
	createAllShaderStages(vertexData,pixelData);

	ID3D11InputLayout* inputLayout = NULL;
	//createPTVertexInputLayout(vertexData,&inputLayout);
	createInstancedLightInputLayout( vertexData, &inputLayout );

	ShaderVariableContainer shaderInitData;
	createShaderInitData( &shaderInitData, inputLayout, vertexData, pixelData );

	//DeferredComposeShader* newDeferredComposeShader = NULL;
	//newDeferredComposeShader = new DeferredComposeShader(shaderInitData);
	//return newDeferredComposeShader;

	LightShader* newLightShader = NULL;
	newLightShader = new LightShader(shaderInitData);
	return newLightShader;
}

DeferredComposeShader* ShaderFactory::createDeferredComposeShader( const LPCWSTR& p_filePath )
{
	DeferredComposeShader*	newComposeShader = NULL;
	ID3D11SamplerState*		samplerState = NULL;
	ID3D11InputLayout*		inputLayout = NULL;
	ShaderVariableContainer shaderVariables;

	VSData* vertexData	= new VSData();
	PSData* pixelData	= new PSData();
	
	vertexData->stageConfig = new ShaderStageConfig(p_filePath,"VS",m_shaderModelVersion);
	pixelData->stageConfig = new ShaderStageConfig(p_filePath,"PS", m_shaderModelVersion);

	createAllShaderStages(vertexData, pixelData);
	createPTVertexInputLayout(vertexData,&inputLayout);
	createShaderInitData(&shaderVariables,inputLayout,vertexData,pixelData,NULL);

	return new DeferredComposeShader(m_bufferFactory->createComposeBuffer(), shaderVariables);
}

GUIShader* ShaderFactory::createGUIShader( const LPCWSTR& p_filePath )
{
	GUIShader* guiShader = NULL;
	ID3D11SamplerState* samplerState = NULL;
	ID3D11InputLayout* inputLayout = NULL;

	VSData* vertexData = new VSData();
	PSData* pixelData = new PSData();

	vertexData->stageConfig = new ShaderStageConfig(p_filePath,"VS",m_shaderModelVersion);
	pixelData->stageConfig = new ShaderStageConfig(p_filePath,"PS",m_shaderModelVersion);

	createAllShaderStages(vertexData,pixelData);
	createInstancedPNTTBVertexInputLayout(vertexData,&inputLayout);

	ShaderVariableContainer shaderInitData;
	createShaderInitData(&shaderInitData,inputLayout,vertexData,pixelData);

	guiShader = new GUIShader(shaderInitData);
	return guiShader;
}

ParticleShader* ShaderFactory::createParticleShader( const LPCWSTR& p_filePath )
{
	ID3D11SamplerState* samplerState = NULL;
	ID3D11InputLayout* inputLayout = NULL;
	ShaderVariableContainer shaderInitData;

	VSData* vertexD		= new VSData();
	GSData* geometryD	= new GSData();
	PSData* pixelD		= new PSData();

	vertexD->stageConfig = new ShaderStageConfig(p_filePath,"VS",m_shaderModelVersion);
	geometryD->stageConfig = new ShaderStageConfig(p_filePath,"GS",m_shaderModelVersion);
	pixelD->stageConfig = new ShaderStageConfig(p_filePath,"PS",m_shaderModelVersion);

	createAllShaderStages(vertexD,pixelD,geometryD);
	createParticleInputLayout(vertexD,&inputLayout);

	createShaderInitData(&shaderInitData, inputLayout, vertexD, pixelD, 
		geometryD);

	return new ParticleShader(shaderInitData, m_bufferFactory->createParticleCBuffer());
}

ShadowShader* ShaderFactory::createShadowShader( const LPCWSTR& p_filePath ){
	ID3D11SamplerState* samplerState = NULL;
	ID3D11InputLayout* inputLayout = NULL;
	ShaderVariableContainer shaderInitData;

	VSData* vertexData = new VSData();

	vertexData->stageConfig = new ShaderStageConfig(p_filePath,"VS",m_shaderModelVersion);

	createAllShaderStages(vertexData);
	createInstancedPNTTBVertexInputLayout(vertexData, &inputLayout);
	createShaderInitData(&shaderInitData, inputLayout, vertexData, NULL );

	return new ShadowShader(shaderInitData, m_bufferFactory->createShadowBuffer());
}

DeferredPostProcessing* ShaderFactory::createDeferredPostProcessingShader( 
	const LPCWSTR& p_filePath )
{
	DeferredPostProcessing*	newShader = NULL;
	ID3D11SamplerState*		samplerState = NULL;
	ID3D11InputLayout*		inputLayout = NULL;
	ShaderVariableContainer shaderVariables;

	VSData* vertexData	= new VSData();
	PSData* pixelData	= new PSData();

	vertexData->stageConfig = new ShaderStageConfig(p_filePath,"VS",m_shaderModelVersion);
	pixelData->stageConfig = new ShaderStageConfig(p_filePath,"PS", m_shaderModelVersion);

	createAllShaderStages(vertexData, pixelData);
	createPTVertexInputLayout(vertexData,&inputLayout);
	createShaderInitData(&shaderVariables,inputLayout,vertexData,pixelData,NULL);

	return new DeferredPostProcessing(m_bufferFactory->createPostProcessingBuffer(), 
		shaderVariables);
}

void ShaderFactory::compileShaderStage( const LPCWSTR &p_sourceFile, 
									    const string &p_entryPoint, 
										const string &p_profile, ID3DBlob** p_blob )
{
	HRESULT res = S_OK;

	ID3DBlob*	blobError  = NULL;
	ID3DBlob*	shaderBlob = NULL;

	*p_blob = NULL;

	DWORD compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	compileFlags |= D3DCOMPILE_DEBUG; 
	compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	//compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#else
	compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3; 
#endif

	// Compile the programs
	// vertex
	res = D3DCompileFromFile(p_sourceFile, 0, 
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		(LPCTSTR)p_entryPoint.c_str(), (LPCTSTR)p_profile.c_str(), 
		compileFlags, 0, 
		&shaderBlob, &blobError);
	if ( FAILED(res) )
	{
		if (blobError!=NULL)
			throw D3DException(blobError,__FILE__,__FUNCTION__,__LINE__);
		else
			throw D3DException(res,__FILE__,__FUNCTION__,__LINE__);

		return;
	}

	*p_blob = shaderBlob;
}

void ShaderFactory::createAllShaderStages(VSData* p_vs/* =NULL */, 
										  PSData* p_ps/* =NULL */, 
										  GSData* p_gs/* =NULL */, 
										  HSData* p_hs/* =NULL */, 
										  DSData* p_ds/* =NULL */)
{
	bool pixelCompiled	= false;
	bool vertexCompiled = false;
	bool geometryCompiled = false;
	bool hullCompiled	= false;
	bool domainCompiled = false;

	if (p_vs)
	{
		HRESULT hr = S_OK;
		compileShaderStage(p_vs->stageConfig->filePath,p_vs->stageConfig->entryPoint,
			string("vs_")+p_vs->stageConfig->version,&p_vs->compiledData);

		hr = m_device->CreateVertexShader(p_vs->compiledData->GetBufferPointer(),
			p_vs->compiledData->GetBufferSize(), NULL, &p_vs->data);
		if(FAILED(hr))
			throw D3DException(hr,__FILE__,__FUNCTION__,__LINE__);
		else
			vertexCompiled = true;
	}
	else
		throw D3DException("Missing vertex shader", __FILE__,__FUNCTION__,__LINE__);

	if(p_ps)
	{
		HRESULT hr = S_OK;
		compileShaderStage(p_ps->stageConfig->filePath,p_ps->stageConfig->entryPoint,
			string("ps_")+p_ps->stageConfig->version,&p_ps->compiledData);

		hr = m_device->CreatePixelShader(p_ps->compiledData->GetBufferPointer(),
			p_ps->compiledData->GetBufferSize(), NULL, &p_ps->data);
		if(FAILED(hr))
			throw D3DException(hr,__FILE__,__FUNCTION__,__LINE__);
		else
			pixelCompiled = true;
	}

	if(vertexCompiled)
	{
		if (p_gs)
		{
			HRESULT hr = S_OK;
			compileShaderStage(p_gs->stageConfig->filePath,p_gs->stageConfig->entryPoint,
				string("gs_")+p_gs->stageConfig->version,&p_gs->compiledData);

			hr = m_device->CreateGeometryShader(p_gs->compiledData->GetBufferPointer(),
				p_gs->compiledData->GetBufferSize(), NULL, &p_gs->data);
			if(FAILED(hr))
				throw D3DException(hr,__FILE__,__FUNCTION__,__LINE__);
			else
				geometryCompiled = true;
		}

		if (p_hs)
		{
			HRESULT hr = S_OK;
			compileShaderStage(p_hs->stageConfig->filePath,p_hs->stageConfig->entryPoint,
				string("hs_")+p_hs->stageConfig->version,&p_hs->compiledData);

			hr = m_device->CreateHullShader(p_hs->compiledData->GetBufferPointer(),
				p_hs->compiledData->GetBufferSize(), NULL, &p_hs->data);
			if(FAILED(hr))
				throw D3DException(hr,__FILE__,__FUNCTION__,__LINE__);
			else
				hullCompiled = true;
		}
	
		if(p_ds && p_hs)
		{
			HRESULT hr = S_OK;
			compileShaderStage(p_ds->stageConfig->filePath,p_ds->stageConfig->entryPoint,
				string("ds_")+p_ds->stageConfig->version,&p_ds->compiledData);

			hr = m_device->CreateDomainShader(p_ds->compiledData->GetBufferPointer(),
				p_ds->compiledData->GetBufferSize(), NULL, &p_ds->data);
			if(FAILED(hr))
				throw D3DException(hr,__FILE__,__FUNCTION__,__LINE__);
			else 
				domainCompiled = true;
		}
		else if(hullCompiled)
			throw D3DException("Invalid shader stage config",__FILE__,__FUNCTION__,
			__LINE__);
	}
}

void ShaderFactory::createShaderInitData(ShaderVariableContainer* p_shaderInitData, 
										 ID3D11InputLayout* p_inputLayout,
										 VSData* p_vsd, PSData* p_psd/* =NULL */,
										 GSData* p_gsd/* =NULL */, 
										 HSData* p_hsd/* =NULL */, 
										 DSData* p_dsd/* =NULL */)
{
	p_shaderInitData->deviceContext = m_deviceContext;
	p_shaderInitData->inputLayout	= p_inputLayout;
	p_shaderInitData->vertexShader	= p_vsd;
	p_shaderInitData->hullShader	= p_hsd;
	p_shaderInitData->domainShader	= p_dsd;
	p_shaderInitData->geometryShader = p_gsd;
	p_shaderInitData->pixelShader	= p_psd;
}


void ShaderFactory::createPTVertexInputLayout(VSData* p_vs, 
											  ID3D11InputLayout** p_inputLayout )
{
	D3D11_INPUT_ELEMENT_DESC input[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0,	DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 
		D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	int cntCalc = sizeof(input)/sizeof(input[0]);
	int elementCnt = 2;
	constructInputLayout(input, elementCnt, p_vs, p_inputLayout);
}

void ShaderFactory::createPNTVertexInputLayout(VSData* p_vs, 
											   ID3D11InputLayout** p_inputLayout )
{
	D3D11_INPUT_ELEMENT_DESC input[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},		
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0,	DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 
		D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	//constructInputLayout(input,sizeof(input)/sizeof(input[0]),p_vs,p_inputLayout);
	int cntCalc = sizeof(input)/sizeof(input[0]);
	int elementCnt = 3;
	constructInputLayout(input, elementCnt, p_vs, p_inputLayout);
}

void ShaderFactory::createInstancedPNTVertexInputLayout( VSData* p_vs, 
														ID3D11InputLayout** p_inputLayout)
{
	D3D11_INPUT_ELEMENT_DESC input[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},		
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0,	DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 
		D3D11_INPUT_PER_VERTEX_DATA, 0},


		{"INSTANCETRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};
	//constructInputLayout(input,sizeof(input)/sizeof(input[0]),p_vs,p_inputLayout);
	int cntCalc = sizeof(input)/sizeof(input[0]);
	int elementCnt = 7;
	constructInputLayout(input, elementCnt, p_vs, p_inputLayout);
}


void ShaderFactory::createPNTTBVertexInputLayout( VSData* p_vs, 
												 ID3D11InputLayout** p_inputLayout )
{
	D3D11_INPUT_ELEMENT_DESC input[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},		
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0,	DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	//constructInputLayout(input,sizeof(input)/sizeof(input[0]),p_vs,p_inputLayout);
	int cntCalc = sizeof(input)/sizeof(input[0]);
	int elementCnt = 5;
	constructInputLayout(input, elementCnt, p_vs, p_inputLayout);
}

void ShaderFactory::createInstancedPNTTBVertexInputLayout( VSData* p_vs, 
														  ID3D11InputLayout** p_inputLayout )
{
	D3D11_INPUT_ELEMENT_DESC input[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},		
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0,	DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},


		{"INSTANCETRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},


		{"GRADIENTCOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"GRADIENTCOLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"GRADIENTCOLOR", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"GRADIENTCOLOR", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"FLAGS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,1, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA,1},

		{"OVERLAY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,1, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA,1},
	};
	//constructInputLayout(input,sizeof(input)/sizeof(input[0]),p_vs,p_inputLayout);
	int cntCalc = sizeof(input)/sizeof(input[0]);
	int elementCnt = 15;
	constructInputLayout(input, elementCnt, p_vs, p_inputLayout);
}
void ShaderFactory::createInstancedAnimatedPNTTBVertexInputLayout( VSData* p_vs, 
														  ID3D11InputLayout** p_inputLayout )
{
	D3D11_INPUT_ELEMENT_DESC input[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},		
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0,	DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"AWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT, 
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT, 
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"AINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT, 
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT, 
		D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"INSTANCETRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"GRADIENTCOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"GRADIENTCOLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"GRADIENTCOLOR", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"GRADIENTCOLOR", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"FLAGS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA,1},

		{"OVERLAY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,2, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA,1},
	};
	//constructInputLayout(input,sizeof(input)/sizeof(input[0]),p_vs,p_inputLayout);
	int cntCalc = sizeof(input)/sizeof(input[0]);
	int elementCnt = 19;
	constructInputLayout(input, elementCnt, p_vs, p_inputLayout);
}

void ShaderFactory::createInstancedLightInputLayout( VSData* p_vertexShader,
													ID3D11InputLayout** p_inputLayout)
{
	D3D11_INPUT_ELEMENT_DESC input[] = {
		// Per vertex
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},

		// Per instance
		{"INSTANCETRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"LIGHTDIR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"RANGE", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"ATTENUATION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"SPOTLIGHTCONESIZEASPOW", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"LIGHTENERGY", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"ENABLED", 0, DXGI_FORMAT_R32_SINT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TYPE", 0, DXGI_FORMAT_R32_SINT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"SHADOWIDX", 0, DXGI_FORMAT_R32_SINT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};

	//int elementCnt = sizeof(input)/sizeof(input[0]) ; //Will this work for both RGB and RGBA? Mattias L
	int cntCalc = sizeof(input)/sizeof(input[0]);
	int elementCnt = 14;
	constructInputLayout(input, elementCnt, p_vertexShader, p_inputLayout);
}

void ShaderFactory::createParticleInputLayout( VSData* p_vs, 
											  ID3D11InputLayout** p_inputLayout )
{
	D3D11_INPUT_ELEMENT_DESC input[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"AGE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"ANGULARVELOCITY", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"ROTATION", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"PAD", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	//int elementCnt = sizeof(input)/sizeof(input[0]) ; //Will this work for both RGB and RGBA? Mattias L
	int elementCnt = 7;
	constructInputLayout(input, elementCnt, p_vs, p_inputLayout);
}

void ShaderFactory::constructInputLayout(const D3D11_INPUT_ELEMENT_DESC* p_inputDesc, 
										 UINT p_numberOfElements,
										 VSData* p_vs, ID3D11InputLayout** p_inputLayout )
{
	HRESULT hr = m_device->CreateInputLayout(
		p_inputDesc, 
		p_numberOfElements, 
		p_vs->compiledData->GetBufferPointer(),
		p_vs->compiledData->GetBufferSize(),
		p_inputLayout);

	if ( FAILED(hr) )
		throw D3DException(hr, __FILE__, __FUNCTION__, __LINE__);
}