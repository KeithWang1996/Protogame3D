#define STB_IMAGE_IMPLEMENTATION
#include "Engine/Renderer/Texture.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Math/IntVec2.hpp"

Texture::Texture(const char* imageFilePath)
	: m_imageFilePath(imageFilePath)
{
	
}

// Texture::Texture(const Texture& anotherTexture)
// {
// 	m_imageTexelSizeX = anotherTexture.m_imageTexelSizeX;
// 	m_imageTexelSizeY = anotherTexture.m_imageTexelSizeY;
// 	m_imageFilePath = anotherTexture.m_imageFilePath;
// 	m_textureID = anotherTexture.m_textureID;
// 	m_owner = anotherTexture.m_owner;
// 	m_handle = anotherTexture.m_handle;
// 	m_renderTargetView = anotherTexture.m_renderTargetView;
// 	m_shaderResourceView = anotherTexture.m_shaderResourceView;
// 	m_depthStencilView = anotherTexture.m_depthStencilView;
// }

Texture::Texture(RenderContext* ctx, ID3D11Texture2D* handle)
	: m_owner(ctx)
	, m_handle(handle)
{
	D3D11_TEXTURE2D_DESC desc;
	handle->GetDesc(&desc);
	m_imageTexelSizeX = desc.Width;
	m_imageTexelSizeY = desc.Height;
}

Texture::Texture(const char* filePath, RenderContext* ctx, ID3D11Texture2D* handle)
	: Texture(ctx, handle)
{
	m_imageFilePath = filePath;
}

Texture::~Texture()
{
	DX_SAFE_RELEASE(m_handle);
	delete m_renderTargetView;
	m_renderTargetView = nullptr;
	delete m_shaderResourceView;
	m_shaderResourceView = nullptr;
	delete m_depthStencilView;
	m_depthStencilView = nullptr;
}

TextureView* Texture::GetRenderTargetView()
{
	if (m_renderTargetView)
	{
		return m_renderTargetView;
	}
	ID3D11Device* dev = m_owner->m_device;
	ID3D11RenderTargetView* rtv = nullptr;

	dev->CreateRenderTargetView(m_handle, nullptr, &rtv);

	if (rtv != nullptr)
	{
		m_renderTargetView = new TextureView();
		m_renderTargetView->m_rtv = rtv;
	}

	return m_renderTargetView;
}

TextureView* Texture::GetOrCreateShaderResourceView()
{
	if (m_shaderResourceView)
	{
		return m_shaderResourceView;
	}
	ID3D11Device* dev = m_owner->m_device;
	ID3D11ShaderResourceView* srv = nullptr;
	dev->CreateShaderResourceView(m_handle, nullptr, &srv);
	if (srv != nullptr)
	{
		m_shaderResourceView = new TextureView();
		m_shaderResourceView->m_srv = srv;
	}
	return m_shaderResourceView;
}

TextureView* Texture::CreateOrGetDepthStencilView()
{
	if (m_depthStencilView)
	{
		return m_depthStencilView;
	}
	ID3D11Device* dev = m_owner->m_device;
	ID3D11DepthStencilView* dsv = nullptr;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;

	dev->CreateDepthStencilView(m_handle, &descDSV, &dsv);
	if (dsv != nullptr)
	{
		m_depthStencilView = new TextureView();
		m_depthStencilView->m_dsv = dsv;
	}
	return m_depthStencilView;
}

IntVec2 Texture::GetTextureSize() const
{
	return IntVec2(m_imageTexelSizeX, m_imageTexelSizeY);
}

Texture* Texture::CreateDepthStencilBuffer(RenderContext* ctx, IntVec2 resolution)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = resolution.x;
	desc.Height = resolution.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT; //if we do mid chains, we need this to be GPU/DEFAULT
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = nullptr;
	initialData.SysMemPitch = resolution.x * 4;
	initialData.SysMemSlicePitch = 0;

	ID3D11Texture2D* texHandle = NULL;
	ctx->m_device->CreateTexture2D(&desc, NULL, &texHandle);
	Texture* texture = new Texture(ctx, texHandle);
	return texture;
}