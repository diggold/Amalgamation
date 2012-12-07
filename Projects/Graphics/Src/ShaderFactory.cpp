#include "ShaderFactory.h"

ShaderFactory::ShaderFactory(ID3D11Device* p_device, ID3D11DeviceContext* p_deviceContext)
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
}


ShaderFactory::~ShaderFactory()
{
	delete m_bufferFactory;
}


DeferredBaseShader* ShaderFactory::createDeferredBaseShader(const LPCWSTR& p_filePath)
{
	DeferredBaseShader* newDeferredBaseShader = NULL;
	ID3D11SamplerState* samplerState = NULL;
	ID3D11InputLayout* inputLayout = NULL;

	VSData* vertexData = new VSData();
	PSData* pixelData = new PSData();
	vertexData->stageConfig = new ShaderStageConfig(p_filePath,"VS",m_shaderModelVersion);
	pixelData->stageConfig = new ShaderStageConfig(p_filePath,"PS",m_shaderModelVersion);

	createAllShaderStages(vertexData,pixelData);
	createSamplerState(&samplerState);
	createPTNVertexInputLayout(vertexData,&inputLayout);

	ShaderInitStruct shaderInitData;
	createShaderInitData(&shaderInitData,inputLayout,vertexData,pixelData,samplerState);

	newDeferredBaseShader = new DeferredBaseShader(shaderInitData,
												m_bufferFactory->createSimpleCBuffer());
	return newDeferredBaseShader;
}

DeferredComposeShader* ShaderFactory::createDeferredComposeShader(const LPCWSTR& p_filePath)
{
	DeferredComposeShader* newDeferredComposeShader = NULL;
	ID3D11SamplerState* samplerState = NULL;
	ID3D11InputLayout* inputLayout = NULL;

	BufferConfig* initConfig  = NULL;

	VSData* vertexData = new VSData();
	PSData* pixelData = new PSData();
	vertexData->stageConfig = new ShaderStageConfig(p_filePath, "VS", m_shaderModelVersion);
	pixelData->stageConfig = new ShaderStageConfig(p_filePath, "PS", m_shaderModelVersion);

	createAllShaderStages(vertexData,pixelData);
	createSamplerState(&samplerState);
	createPTVertexInputLayout(vertexData,&inputLayout);


	ShaderInitStruct shaderInitData;
	createShaderInitData(&shaderInitData,inputLayout,vertexData,pixelData,samplerState);


	newDeferredComposeShader = new DeferredComposeShader(shaderInitData);

	return newDeferredComposeShader;
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
	else
		throw D3DException("Missing pixel shader", __FILE__,__FUNCTION__,__LINE__);

	if(vertexCompiled && pixelCompiled)
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
	else
		throw D3DException("Either pixel or vertex shader failed to compile", __FILE__,
		__FUNCTION__,__LINE__);

}

void ShaderFactory::createSamplerState( ID3D11SamplerState** p_samplerState )
{
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc,sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_device->CreateSamplerState(&samplerDesc,p_samplerState );
	if(FAILED(hr))
		throw D3DException(hr,__FILE__,__FUNCTION__,__LINE__);
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
	constructInputLayout(input,sizeof(input)/sizeof(input[0]),p_vs,p_inputLayout);
}

void ShaderFactory::createPTNVertexInputLayout(VSData* p_vs, 
											   ID3D11InputLayout** p_inputLayout )
{
	D3D11_INPUT_ELEMENT_DESC input[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0,	DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	constructInputLayout(input,sizeof(input)/sizeof(input[0]),p_vs,p_inputLayout);
}

void ShaderFactory::createShaderInitData(ShaderInitStruct* p_shaderInitData, 
										 ID3D11InputLayout* p_inputLayout,
										 VSData* p_vsd, PSData* p_psd,
										 ID3D11SamplerState* p_samplerState/* =NULL */,
										 GSData* p_gsd/* =NULL */, 
										 HSData* p_hsd/* =NULL */, 
										 DSData* p_dsd/* =NULL */)
{
	p_shaderInitData->device = m_device;
	p_shaderInitData->deviceContext = m_deviceContext;
	p_shaderInitData->domainShader = p_dsd;
	p_shaderInitData->geometryShader = p_gsd;
	p_shaderInitData->hullShader = p_hsd;
	p_shaderInitData->vertexShader = p_vsd;
	p_shaderInitData->pixelShader = p_psd;
	p_shaderInitData->samplerState = p_samplerState;
	p_shaderInitData->inputLayout = p_inputLayout;
}

void ShaderFactory::constructInputLayout(const D3D11_INPUT_ELEMENT_DESC* p_inputDesc, 
										 UINT p_numberOfElements,
										 VSData* p_vs, ID3D11InputLayout** p_inputLayout )
{
	HRESULT hr = m_device->CreateInputLayout(p_inputDesc, 
		p_numberOfElements, 
		p_vs->compiledData->GetBufferPointer(),
		p_vs->compiledData->GetBufferSize(), p_inputLayout);
	if ( FAILED(hr) )
		throw D3DException(hr, __FILE__, __FUNCTION__, __LINE__);
}