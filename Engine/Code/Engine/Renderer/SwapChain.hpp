#pragma once

class RenderContext;
struct IDXGISwapChain;
class Texture;

class SwapChain
{
public:
	SwapChain(RenderContext* owner, IDXGISwapChain* handle);
	~SwapChain();
	void Present(int vsync = 0);

	Texture* GetBackBuffer();
public:
	RenderContext* m_owner;		// who made this(which context was the creator)
	IDXGISwapChain* m_handle;	//D3D11 created pointer, what we used when calling D3D11 calls

	Texture* m_backBuffer = nullptr;
};