#pragma once
#include "Engine/Renderer/RenderContext.hpp"
class Game;
class Window;
class Texture;
class App
{
public:
	App();
	~App();
	void Startup();
	void Shutdown();
	void RunFrame();

	bool IsQuitting() const;
	bool HandleQuitRequested();

	void UpdateFromKeyboard();
private:
	void BeginFrame();
	void Update();
	void Render() const;
	void EndFrame();
	void ResetGame();
	//void UpdateShip(float deltaSeconds);
	//void RenderShip() const;
private:
	bool m_isQuitting = false;
	bool m_isPaused = false;
	bool m_isSlowMo = false;
	bool m_isFastMo = false;
	Camera* m_HUDCamera = nullptr;
};
//extern App* g_theApp;
//extern RenderContext* g_theRenderer;

