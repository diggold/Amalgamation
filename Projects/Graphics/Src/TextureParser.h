// =======================================================================================
//                                      TextureParseer
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	Texture Parser wrapps the functionality behind FreeImage into one light weight
/// class
///        
/// # TextureParseer
/// Detailed description.....
/// Created on: 5-12-2012 
///---------------------------------------------------------------------------------------
#pragma once
#include <FreeImage.h>
#include <d3d11.h>
#include <windows.h>
#include "D3DException.h"
#include "FreeImageException.h"

namespace TextureParser
{
	///-----------------------------------------------------------------------------------
	/// Called once to initialize Free Image properly
	/// \returns void
	///-----------------------------------------------------------------------------------
	static void init()
	{
		FreeImage_Initialise();
	}

	///-----------------------------------------------------------------------------------
	/// Handles the loading and creation of textures files. Supports various of types and
	/// throws exception if creation wasn't successfully.
	/// \param p_device
	/// \param p_filePath
	/// \returns ID3D11ShaderResourceView*
	///-----------------------------------------------------------------------------------
	static ID3D11ShaderResourceView* loadTexture(ID3D11Device* p_device, 
		const char* p_filePath)
	{
		FREE_IMAGE_FORMAT imageFormat;
		FIBITMAP* image;

		imageFormat = FreeImage_GetFIFFromFilename(p_filePath);
		if( imageFormat != FIF_UNKNOWN )
			image = FreeImage_Load(imageFormat, p_filePath);
		else
			throw FreeImageException("Unknown file format, cannot parse the file, " + 
			toString(p_filePath),__FILE__,__FUNCTION__,__LINE__);

		FreeImage_FlipVertical(image);

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = (void*)FreeImage_GetBits(image);
		data.SysMemPitch = FreeImage_GetPitch(image);

		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc,sizeof(D3D11_TEXTURE2D_DESC));
		texDesc.Width		= FreeImage_GetWidth(image);
		texDesc.Height		= FreeImage_GetHeight(image);
		texDesc.MipLevels	= 1;
		texDesc.ArraySize	= 1;
		texDesc.SampleDesc.Count	= 1;
		texDesc.SampleDesc.Quality	= 0;
		texDesc.Usage				= D3D11_USAGE_DEFAULT;
		texDesc.Format				= DXGI_FORMAT_B8G8R8A8_UNORM;
		texDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags		= 0;
		texDesc.MiscFlags			= 0;

		ID3D11Texture2D* texture = NULL;
		HRESULT hr = p_device->CreateTexture2D( &texDesc, &data, &texture);

		if (FAILED(hr))
			throw D3DException(hr, __FILE__,__FUNCTION__,__LINE__);

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderDesc;
		ZeroMemory(&shaderDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		shaderDesc.Format = texDesc.Format;
		shaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderDesc.Texture2D.MipLevels = 1;
		shaderDesc.Texture2D.MostDetailedMip = 0;

		ID3D11ShaderResourceView* newShaderResurceView;
		hr = p_device->CreateShaderResourceView( texture, &shaderDesc, 
			&newShaderResurceView);

		if (FAILED(hr))
			throw D3DException(hr, __FILE__,__FUNCTION__,__LINE__);

		/************************************************************************/
		/* Clean up the mess afterwards											*/
		/************************************************************************/
		FreeImage_Unload(image);
		texture->Release();

		return newShaderResurceView;
	}
}