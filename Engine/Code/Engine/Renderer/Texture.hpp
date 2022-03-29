#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <string>
class Image;
class RenderContext;
struct ID3D11Texture2D;
class TextureView;
struct IntVec2;
class Texture
{
public:
	Texture(const char* imageFilePath);
	//explicit Texture(const Texture& anotherTexture);
	explicit Texture(RenderContext* ctx, ID3D11Texture2D* handle);
	explicit Texture(const char* filePath, RenderContext* ctx, ID3D11Texture2D* handle);
	~Texture();
	unsigned int GetTextureID() const { return m_textureID; }
	ID3D11Texture2D* GetHandler() const { return m_handle; }
	TextureView* GetRenderTargetView();//TODO::change tp get or create
	TextureView* GetOrCreateShaderResourceView();
	TextureView* CreateOrGetDepthStencilView();

	IntVec2 GetTextureSize() const;
	static Texture* CreateDepthStencilBuffer(RenderContext* ctx, IntVec2 resolution);
	RenderContext* GetOwner() { return m_owner; }
public:
	//static Texture* CreateFromFile(RenderContext* ctx, const char* filePath);
public:
	//Image* m_image;
	int m_imageTexelSizeX = 0;
	int m_imageTexelSizeY = 0;

	std::string m_imageFilePath;
	unsigned int m_textureID = 0;
private:
	RenderContext* m_owner = nullptr;
	ID3D11Texture2D* m_handle = nullptr;
	TextureView* m_renderTargetView = nullptr;
	TextureView* m_shaderResourceView = nullptr;
	TextureView* m_depthStencilView = nullptr;
};