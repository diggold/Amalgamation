#include "RenderStateHelper.h"
#include "D3DException.h"


void RenderStateHelper::fillBlendStateList(ID3D11Device* p_device,
										   vector<ID3D11BlendState*>& p_blendStateList )
{
	ID3D11BlendState* blendstate = NULL;
	ID3D11BlendState* defaultBlendstate = NULL;
	D3D11_BLEND_DESC blendStateDesc;
	// Always build default, so it can be used as fallback
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};

	blendStateDesc.AlphaToCoverageEnable = FALSE;
	blendStateDesc.IndependentBlendEnable = FALSE;
	blendStateDesc.RenderTarget[0].BlendEnable = FALSE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	p_device->CreateBlendState( &blendStateDesc, &blendstate ); 
	//
	defaultBlendstate = blendstate;
	p_blendStateList.push_back(blendstate);

	// The rest
	for (unsigned int i=BlendState::DEFAULT+1; i<BlendState::NUMBER_OF_MODES; i++)
	{
		switch(i)
		{
		case BlendState::ALPHA:
			{
				blendstate = NULL;
				ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
				float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};

				blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
				blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
				blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

				blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				p_device->CreateBlendState( &blendStateDesc, &blendstate ); 
				break;
			}
		case BlendState::MULTIPLY:
			{
				blendstate = NULL;
				ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
				float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};

				blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
				blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
				blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
				blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
				blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;
				blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

				blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				p_device->CreateBlendState( &blendStateDesc, &blendstate ); 
				break;
			}
		case BlendState::ADDITIVE:
			{
				blendstate = NULL;
				ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
				float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};

				blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
				blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
				blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
				blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

				blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				p_device->CreateBlendState( &blendStateDesc, &blendstate ); 
				break;
			}
		case BlendState::PARTICLE:
			{
				blendstate = NULL;
				ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
				float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};

				blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
				blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
				blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
				blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

				blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				p_device->CreateBlendState( &blendStateDesc, &blendstate ); 
				break;
			}
		case BlendState::LIGHT:
			{
				blendstate = NULL;
				ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
				float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};

				blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
				blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
				blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
				blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
				blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

				blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				p_device->CreateBlendState( &blendStateDesc, &blendstate ); 
				break;
			}
		case BlendState::SSAO:
			{
				blendstate = NULL;
				ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
				float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};

				blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
				blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
				blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
				blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
				blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

				blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				p_device->CreateBlendState( &blendStateDesc, &blendstate ); 
				break;
			}
		case BlendState::OVERWRITE:
			{
				blendstate = NULL;
				ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
				float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};

				blendStateDesc.RenderTarget[0].BlendEnable = FALSE;

				blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
				blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
				blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
				blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

				blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				p_device->CreateBlendState( &blendStateDesc, &blendstate ); 
				break;
			}

		//case BlendState::NORMAL:
		default:
			{
				blendstate=defaultBlendstate; // fallback
				break;
			}
		}
		p_blendStateList.push_back(blendstate);
	}
}

void RenderStateHelper::fillRasterizerStateList(ID3D11Device* p_device,
												vector<ID3D11RasterizerState*>& p_rasterizerStateList )
{
	ID3D11RasterizerState* rasterizerstate = NULL;
	ID3D11RasterizerState* defaultRasterizerstate = NULL;
	D3D11_RASTERIZER_DESC rasterizerStateDesc;
	// Always build default, so it can be used as fallback
	ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerStateDesc.CullMode = D3D11_CULL_BACK;
	rasterizerStateDesc.FrontCounterClockwise = FALSE;
	rasterizerStateDesc.DepthClipEnable = TRUE;
	rasterizerStateDesc.AntialiasedLineEnable = FALSE;
	rasterizerStateDesc.MultisampleEnable = FALSE;
	rasterizerStateDesc.DepthBias = 0;
	rasterizerStateDesc.DepthBiasClamp = 0.0f;
	rasterizerStateDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerStateDesc.ScissorEnable = false;

	p_device->CreateRasterizerState( &rasterizerStateDesc, &rasterizerstate ); 
	//
	defaultRasterizerstate = rasterizerstate;
	p_rasterizerStateList.push_back(rasterizerstate);

	// The rest
	for (unsigned int i=RasterizerState::DEFAULT+1; i<RasterizerState::NUMBER_OF_MODES; i++)
	{
		switch(i)
		{
		case RasterizerState::FILLED_CCW:
			{
				rasterizerstate = NULL;
				ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

				rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
				rasterizerStateDesc.CullMode = D3D11_CULL_BACK;
				rasterizerStateDesc.FrontCounterClockwise = TRUE;
				rasterizerStateDesc.DepthClipEnable = TRUE;
				rasterizerStateDesc.AntialiasedLineEnable = FALSE;
				rasterizerStateDesc.MultisampleEnable = FALSE;
				rasterizerStateDesc.DepthBias = 0;
				rasterizerStateDesc.DepthBiasClamp = 0.0f;
				rasterizerStateDesc.SlopeScaledDepthBias = 0.0f;
				rasterizerStateDesc.ScissorEnable = false;

				p_device->CreateRasterizerState( &rasterizerStateDesc, &rasterizerstate ); 
				break;
			}
		case RasterizerState::FILLED_CW_FRONTCULL:
			{
				rasterizerstate = NULL;
				ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

				rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
				rasterizerStateDesc.CullMode = D3D11_CULL_FRONT;
				rasterizerStateDesc.FrontCounterClockwise = FALSE;
				rasterizerStateDesc.DepthClipEnable = TRUE;
				rasterizerStateDesc.AntialiasedLineEnable = FALSE;
				rasterizerStateDesc.MultisampleEnable = FALSE;
				rasterizerStateDesc.DepthBias = 0;
				rasterizerStateDesc.DepthBiasClamp = 0.0f;
				rasterizerStateDesc.SlopeScaledDepthBias = 0.0f;
				rasterizerStateDesc.ScissorEnable = false;

				p_device->CreateRasterizerState( &rasterizerStateDesc, &rasterizerstate ); 
				break;
			}
		case RasterizerState::FILLED_CW_SCISSOR:
			{
				rasterizerstate = NULL;
				ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

				rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
				rasterizerStateDesc.CullMode = D3D11_CULL_BACK;
				rasterizerStateDesc.FrontCounterClockwise = FALSE;
				rasterizerStateDesc.DepthClipEnable = TRUE;
				rasterizerStateDesc.AntialiasedLineEnable = FALSE;
				rasterizerStateDesc.MultisampleEnable = FALSE;
				rasterizerStateDesc.DepthBias = 0;
				rasterizerStateDesc.DepthBiasClamp = 0.0f;
				rasterizerStateDesc.SlopeScaledDepthBias = 0.0f;
				rasterizerStateDesc.ScissorEnable = true;

				p_device->CreateRasterizerState( &rasterizerStateDesc, &rasterizerstate ); 
				break;
			}
		case RasterizerState::FILLED_CCW_SCISSOR:
			{
				rasterizerstate = NULL;
				ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

				rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
				rasterizerStateDesc.CullMode = D3D11_CULL_BACK;
				rasterizerStateDesc.FrontCounterClockwise = TRUE;
				rasterizerStateDesc.DepthClipEnable = TRUE;
				rasterizerStateDesc.AntialiasedLineEnable = FALSE;
				rasterizerStateDesc.MultisampleEnable = FALSE;
				rasterizerStateDesc.DepthBias = 0;
				rasterizerStateDesc.DepthBiasClamp = 0.0f;
				rasterizerStateDesc.SlopeScaledDepthBias = 0.0f;
				rasterizerStateDesc.ScissorEnable = true;

				p_device->CreateRasterizerState( &rasterizerStateDesc, &rasterizerstate ); 
				break;
			}
		case RasterizerState::FILLED_NOCULL:
			{
				rasterizerstate = NULL;
				ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

				rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
				rasterizerStateDesc.CullMode = D3D11_CULL_NONE;
				rasterizerStateDesc.FrontCounterClockwise = FALSE;
				rasterizerStateDesc.DepthClipEnable = TRUE;
				rasterizerStateDesc.AntialiasedLineEnable = FALSE;
				rasterizerStateDesc.MultisampleEnable = FALSE;
				rasterizerStateDesc.DepthBias = 0;
				rasterizerStateDesc.DepthBiasClamp = 0.0f;
				rasterizerStateDesc.SlopeScaledDepthBias = 0.0f;
				rasterizerStateDesc.ScissorEnable = false;

				p_device->CreateRasterizerState( &rasterizerStateDesc, &rasterizerstate ); 
				break;
			}
		case RasterizerState::FILLED_NOCULL_NOCLIP:
			{
				rasterizerstate = NULL;
				ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

				rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
				rasterizerStateDesc.CullMode = D3D11_CULL_NONE;
				rasterizerStateDesc.FrontCounterClockwise = FALSE;
				rasterizerStateDesc.DepthClipEnable = FALSE;
				rasterizerStateDesc.AntialiasedLineEnable = FALSE;
				rasterizerStateDesc.MultisampleEnable = FALSE;
				rasterizerStateDesc.DepthBias = 0;
				rasterizerStateDesc.DepthBiasClamp = 0.0f;
				rasterizerStateDesc.DepthClipEnable = FALSE;
				rasterizerStateDesc.SlopeScaledDepthBias = 0.0f;
				rasterizerStateDesc.ScissorEnable = false;

				p_device->CreateRasterizerState( &rasterizerStateDesc, &rasterizerstate ); 
				break;
			}
		case RasterizerState::FILLED_CW_ALWAYSONTOP:
			{
				rasterizerstate = NULL;
				ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

				rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
				rasterizerStateDesc.CullMode = D3D11_CULL_BACK;
				rasterizerStateDesc.FrontCounterClockwise = FALSE;
				rasterizerStateDesc.DepthClipEnable = FALSE;
				rasterizerStateDesc.AntialiasedLineEnable = FALSE;
				rasterizerStateDesc.MultisampleEnable = FALSE;
				rasterizerStateDesc.DepthBias = FLT_MAX;
				rasterizerStateDesc.DepthBiasClamp = 0.0f;
				rasterizerStateDesc.SlopeScaledDepthBias = 0.0f;
				rasterizerStateDesc.ScissorEnable = false;

				p_device->CreateRasterizerState( &rasterizerStateDesc, &rasterizerstate ); 
				break;
			}
		case RasterizerState::WIREFRAME:
			{
				rasterizerstate = NULL;
				ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

				rasterizerStateDesc.FillMode = D3D11_FILL_WIREFRAME;
				rasterizerStateDesc.CullMode = D3D11_CULL_BACK;
				rasterizerStateDesc.FrontCounterClockwise = FALSE;
				rasterizerStateDesc.DepthClipEnable = TRUE;
				rasterizerStateDesc.AntialiasedLineEnable = FALSE;
				rasterizerStateDesc.MultisampleEnable = FALSE;
				rasterizerStateDesc.DepthBias = 0;
				rasterizerStateDesc.DepthBiasClamp = 0.0f;
				rasterizerStateDesc.SlopeScaledDepthBias = 0.0f;
				rasterizerStateDesc.ScissorEnable = false;

				p_device->CreateRasterizerState( &rasterizerStateDesc, &rasterizerstate ); 
				break;
			}
		case RasterizerState::WIREFRAME_FRONTCULL:
			{
				rasterizerstate = NULL;
				ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

				rasterizerStateDesc.FillMode = D3D11_FILL_WIREFRAME;
				rasterizerStateDesc.CullMode = D3D11_CULL_FRONT;
				rasterizerStateDesc.FrontCounterClockwise = FALSE;
				rasterizerStateDesc.DepthClipEnable = TRUE;
				rasterizerStateDesc.AntialiasedLineEnable = FALSE;
				rasterizerStateDesc.MultisampleEnable = FALSE;
				rasterizerStateDesc.DepthBias = 0;
				rasterizerStateDesc.DepthBiasClamp = 0.0f;
				rasterizerStateDesc.SlopeScaledDepthBias = 0.0f;
				rasterizerStateDesc.ScissorEnable = false;

				p_device->CreateRasterizerState( &rasterizerStateDesc, &rasterizerstate ); 
				break;
			}
		case RasterizerState::WIREFRAME_NOCULL:
			{
				rasterizerstate = NULL;
				ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

				rasterizerStateDesc.FillMode = D3D11_FILL_WIREFRAME;
				rasterizerStateDesc.CullMode = D3D11_CULL_NONE;
				rasterizerStateDesc.FrontCounterClockwise = FALSE;
				rasterizerStateDesc.DepthClipEnable = TRUE;
				rasterizerStateDesc.AntialiasedLineEnable = FALSE;
				rasterizerStateDesc.MultisampleEnable = FALSE;
				rasterizerStateDesc.DepthBias = 0;
				rasterizerStateDesc.DepthBiasClamp = 0.0f;
				rasterizerStateDesc.SlopeScaledDepthBias = 0.0f;
				rasterizerStateDesc.ScissorEnable = false;

				p_device->CreateRasterizerState( &rasterizerStateDesc, &rasterizerstate ); 
				break;
			}
		case RasterizerState::FILLED_CW:
		default:
			{
				rasterizerstate=defaultRasterizerstate; // fallback
				break;
			}
		}
		p_rasterizerStateList.push_back(rasterizerstate);
	}
}

void RenderStateHelper::fillSamplerStateList( ID3D11Device* p_device, ID3D11SamplerState* (&p_samplerStateList)[SamplerState::SamplerState_CNT] )
{
	createSamplerState( p_device, &p_samplerStateList[SamplerState::SamplerState_POINT_WRAP],
		D3D11_FILTER_MIN_MAG_MIP_POINT,		D3D11_TEXTURE_ADDRESS_WRAP );
	createSamplerState( p_device, &p_samplerStateList[SamplerState::SamplerState_LINEAR_WRAP],
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,	D3D11_TEXTURE_ADDRESS_WRAP );
	createSamplerState( p_device, &p_samplerStateList[SamplerState::SamplerState_ANISOTROPIC_WRAP],
		D3D11_FILTER_ANISOTROPIC,			D3D11_TEXTURE_ADDRESS_WRAP );
	createSamplerState( p_device, &p_samplerStateList[SamplerState::SamplerState_POINT_CLAMP],
		D3D11_FILTER_MIN_MAG_MIP_POINT,		D3D11_TEXTURE_ADDRESS_CLAMP );
	createSamplerState( p_device, &p_samplerStateList[SamplerState::SamplerState_LINEAR_CLAMP],
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,	D3D11_TEXTURE_ADDRESS_CLAMP );
	createSamplerState( p_device, &p_samplerStateList[SamplerState::SamplerState_ANISOTROPIC_CLAMP],
		D3D11_FILTER_ANISOTROPIC,			D3D11_TEXTURE_ADDRESS_CLAMP );
}

void RenderStateHelper::createSamplerState( ID3D11Device* p_device, ID3D11SamplerState** p_samplerState,
									   D3D11_FILTER p_filter, D3D11_TEXTURE_ADDRESS_MODE p_texture )
{
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc,sizeof(samplerDesc));
	samplerDesc.Filter = p_filter;
	samplerDesc.AddressU = p_texture;
	samplerDesc.AddressV = p_texture;
	samplerDesc.AddressW = p_texture;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = p_device->CreateSamplerState( &samplerDesc, p_samplerState );
	if( FAILED(hr) ) {
		throw D3DException( hr, __FILE__, __FUNCTION__, __LINE__ );
	}
}