#include "Engine/Renderer/RenderContext.hpp"
#include <cstdio>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
//#include "ThirdParty/imgui/imgui_impl_dx11.h"
//HDC g_displayDeviceContext = nullptr;

#pragma comment( lib, "d3d11.lib" )         // needed a01
#pragma comment( lib, "dxgi.lib" )          // needed a01
#pragma comment( lib, "d3dcompiler.lib" )   // needed when we get to shaders

BitmapFont* g_bitMapFont = nullptr;

RenderContext::~RenderContext()
{
	delete m_frameUBO;
	delete m_cameraUBO;
	delete m_objUBO;
	delete m_lightUBO;
	delete m_materialUBO;
	delete m_projectUBO;
	delete m_effectUBO;
	delete m_glitchUBO;
	delete m_defaultSampler;
	delete m_projectSampler;
	delete m_defaultDepthBuffer;
	delete m_immediateMesh;
	delete m_effectCamera;

	m_frameUBO = nullptr;
	m_cameraUBO = nullptr;
	m_objUBO = nullptr;
	m_lightUBO = nullptr;
	m_materialUBO = nullptr;
	m_projectUBO = nullptr;
	m_effectUBO = nullptr;
	m_glitchUBO = nullptr;
	m_defaultSampler = nullptr;
	m_projectSampler = nullptr;
	m_defaultDepthBuffer = nullptr;
	m_immediateMesh = nullptr;
	m_effectCamera = nullptr;
	for (int textureIndex = 0; textureIndex < m_textures.size(); ++textureIndex)
	{
		delete m_textures[textureIndex];
		m_textures[textureIndex] = nullptr;
	}
	m_textures.clear();

	for (int fontIndex = 0; fontIndex < m_fonts.size(); ++fontIndex)
	{
		delete m_fonts[fontIndex];
		m_fonts[fontIndex] = nullptr;
	}
	m_fonts.clear();
	for (std::map<const char*, Shader*>::iterator itr = m_Shaders.begin(); itr != m_Shaders.end(); itr++)
	{
		delete itr->second;
		itr->second = nullptr;
	}
	m_Shaders.clear();
	if (m_clock != nullptr && m_clock != Clock::GetMaster())
	{
		delete m_clock;
		m_clock = nullptr;
	}

	DX_SAFE_RELEASE(m_additiveBlendState);
	DX_SAFE_RELEASE(m_alphaBlendState);
	DX_SAFE_RELEASE(m_opaqueBlendState);
	DX_SAFE_RELEASE(m_currentDepthStencilState);
	DX_SAFE_RELEASE(m_rasterState);
}

void RenderContext::Startup(Window* window)
{
	//ID3D11Device
	//ID3D11DeviceContext
	IDXGISwapChain* swapchain;
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
	#if defined(RENDER_DEBUG)
		flags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	DXGI_SWAP_CHAIN_DESC swapchainDesc;
	memset(&swapchainDesc, 0, sizeof(swapchainDesc));
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // on swap, the old buffer is discarded
	swapchainDesc.Flags = 0;
	HWND hwnd = (HWND)window->m_hwnd;
	swapchainDesc.OutputWindow = hwnd;//hwnd for window to be used

	swapchainDesc.SampleDesc.Count = 1;//how many samples per pixel(1 so no MSAA)
									   //if we are doing MSAA, we'll do it on a secondary target
	swapchainDesc.Windowed = true;
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapchainDesc.BufferDesc.Width = window->GetClientWidth();
	swapchainDesc.BufferDesc.Height = window->GetClientHeight();
	//create
	HRESULT result = D3D11CreateDeviceAndSwapChain( nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapchainDesc,
		&swapchain,
		&m_device,
		nullptr,
		&m_context
	);
	GUARANTEE_OR_DIE(SUCCEEDED(result), "Failed to create device and context");
	//DX_SAFE_RELEASE(swapchain);
	m_swapChain = new SwapChain(this, swapchain);
	//m_defaultShader = new Shader( this );
	m_defaultShader = GetOrCreateShader("Data/Shaders/default.hlsl");
	//m_Shaders["Data/Shaders/default.hlsl"] = m_defaultShader;

	m_frameUBO = new RenderBuffer(this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC);

	m_defaultSampler = new Sampler(this, SAMPLER_POINT);
	m_projectSampler = new Sampler(this, SAMPLER_POINT);
	m_whiteTexture = CreateTextureFromColor(Rgba8(255, 255, 255, 255));
	m_emptyTexture = CreateTextureFromColor(Rgba8(0, 0, 0, 0));
	m_defaultNormalTexture = CreateTextureFromColor(Rgba8(128, 128, 255, 255));
	m_defaultTexture = CreateOrGetTextureFromFile("Data/Images/Test_StbiFlippedAndOpenGL.png");
	//m_defaultTexture = CreateTextureFromColor(Rgba8(255, 255, 255, 255));

	m_defaultDepthBuffer = Texture::CreateDepthStencilBuffer(this, m_swapChain->GetBackBuffer()->GetTextureSize());
	m_immediateMesh = new GPUMesh(this);
	CreateBlendStates();

	m_effectCamera = new Camera();
	m_effectCamera->SetClearMode(1, Rgba8(0, 0, 0, 0));

	//ImGui_ImplDX11_Init(m_device, m_context);
}

void RenderContext::ClearScreen( Camera camera )
{
	Rgba8 clearColor = camera.GetClearColor();
	float clearFloats[4];
	clearFloats[0] = (float)clearColor.r / 255.f;
	clearFloats[1] = (float)clearColor.g / 255.f;
	clearFloats[2] = (float)clearColor.b / 255.f;
	clearFloats[3] = (float)clearColor.a / 255.f;

	Texture* backbuffer = camera.m_colorTarget[0];
	TextureView* backbuffer_rtv = backbuffer->GetRenderTargetView();

	ID3D11RenderTargetView* rtv = backbuffer_rtv->GetAsRTV();
	m_context->ClearRenderTargetView(rtv, clearFloats);

	Texture* backbuffer1 = camera.m_colorTarget[1];
	if (backbuffer1 == nullptr)
	{
		return;
	}
	TextureView* backbuffer1_rtv = backbuffer1->GetRenderTargetView();

	ID3D11RenderTargetView* rtv1 = backbuffer1_rtv->GetAsRTV();
	m_context->ClearRenderTargetView(rtv1, clearFloats);
}

void RenderContext::ClearDepth(Texture* depthStencilTexture, float depth)
{
	TextureView* view = depthStencilTexture->CreateOrGetDepthStencilView();
	ID3D11DepthStencilView* dsv = view->GetAsDSV();
	m_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, depth, 0);
}

void RenderContext::BeginCamera( const Camera& camera )
{
#if defined(RENDER_DEBUG)
	m_context->ClearState();
#endif
	Texture* colorTarget = camera.m_colorTarget[0];//Camera::GetColorTarget()
	Texture* colorTarget1 = camera.m_colorTarget[1];
	if (colorTarget == nullptr)
	{
		colorTarget = m_swapChain->GetBackBuffer();
	}
	if (colorTarget1 == nullptr)
	{
		colorTarget1 = AcquireRenderTargetMatching(colorTarget);
		ReleaseRenderTarget(colorTarget1);
	}
	
	//what are we drawing to?
	ID3D11RenderTargetView* rtvs[2];
	TextureView* view = colorTarget->GetRenderTargetView();
	rtvs[0] = view->GetAsRTV();
	TextureView* view1 = colorTarget1->GetRenderTargetView();
	rtvs[1] = view1->GetAsRTV();

	if (camera.m_depthBuffer)
	{
		TextureView* depthBufferView = camera.m_depthBuffer->CreateOrGetDepthStencilView();
		m_context->OMSetRenderTargets(2, &rtvs[0], depthBufferView->GetAsDSV());
	}
	else
	{
		m_context->OMSetRenderTargets(2, &rtvs[0], NULL);
	}
	
	D3D11_VIEWPORT viewport;
	IntVec2 textureSize = colorTarget->GetTextureSize();
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)textureSize.x;
	viewport.Height = (float)textureSize.y;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	m_context->RSSetViewports(1, &viewport);
	
	m_isDrawing = true;
	BindShader((Shader*)nullptr);
	m_lastBoundVBO = nullptr;
	m_lastBoundIBO = nullptr;
	//create cameraUBO
	if (m_cameraUBO == nullptr)
	{
		m_cameraUBO = new RenderBuffer(this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC);
	}
	if (m_objUBO == nullptr)
	{
		m_objUBO = new RenderBuffer(this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC);
	}
	if (m_lightUBO == nullptr)
	{
		m_lightUBO = new RenderBuffer(this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC);
	}
	if (m_materialUBO == nullptr)
	{
		m_materialUBO = new RenderBuffer(this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC);
	}
	if (m_projectUBO == nullptr)
	{
		m_projectUBO = new RenderBuffer(this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC);
	}
	if (m_effectUBO == nullptr)
	{
		m_effectUBO = new RenderBuffer(this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC);
	}
	if (m_glitchUBO == nullptr)
	{
		m_glitchUBO = new RenderBuffer(this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC);
	}
	//update cameraUBO
	camera_data_t cameraData;
	cameraData.projection = camera.GetProjection();
	cameraData.view = camera.GetView();
	cameraData.position = camera.GetPosition();
	//view is actually invert of this
	m_cameraUBO->Update(&cameraData, sizeof(cameraData), sizeof(cameraData));

	//Bind CameraUBO
	BindUniformBuffer(UBO_CAMERA_SLOT, m_cameraUBO);
	BindUniformBuffer(UBO_FRAME_SLOT, m_frameUBO);
	BindUniformBuffer(UBO_MODEL_SLOT, m_objUBO);
	BindUniformBuffer(UBO_LIGHT_SLOT, m_lightUBO);
	BindUniformBuffer(UBO_PROJECT_SLOT, m_projectUBO);
	BindUniformBuffer(UBO_EFFECT_SLOT, m_effectUBO);
	BindUniformBuffer(UBO_GLITCH_SLOT, m_glitchUBO);
	//Test 
	m_materialUBO->Update(&m_materialInfo, sizeof(material_t), sizeof(material_t));
	BindUniformBuffer(UBO_MATERIAL_SLOT, m_materialUBO);
	SetModelMatrix(Mat44::IDENTITY);
	SetShaderTint(Vec4(1.f, 1.f, 1.f, 1.f));
	BindTexture(nullptr);
	BindNormalTexture(nullptr);
	BindSampler(m_defaultSampler);
	BindSampler(m_projectSampler, 1);
	SetBlendMode(BlendMode::ALPHA);

	SetDepthTest(COMPARE_FUNC_LEQUAL, true);

	if ((camera.m_clearMode & CLEAR_DEPTH_BIT) && camera.m_depthBuffer != nullptr)
	{
		ClearDepth(m_defaultDepthBuffer, camera.m_depth);
	}

	if (camera.m_clearMode & CLEAR_COLOR_BIT)
	{
		Rgba8 clearColor = camera.m_color;
		ClearScreen(camera);
	}
	ResetRasterizerState();

	m_previouslyBoundLayout = nullptr;
	m_currentLayout = nullptr;
}
void RenderContext::Shutdown()
{
	if (m_device != nullptr)
	{
		DX_SAFE_RELEASE(m_device);
	}
	
	if (m_context != nullptr)
	{
		DX_SAFE_RELEASE(m_context);
	}
	if (m_swapChain != nullptr)
	{
		delete m_swapChain;
		m_swapChain = nullptr;
	}
	//ImGui_ImplDX11_Shutdown();
}

void RenderContext::BeginFrame()
{
	UpdateFrameTime();
	//ImGui_ImplDX11_NewFrame();
}

void RenderContext::UpdateFrameTime()
{
	frame_data_t frameData;
	frameData.system_time = (float)m_clock->GetTotalElapsedSeconds();
	frameData.system_delta_time = (float)m_clock->GetLastDeltaSeconds();

	m_frameUBO->Update(&frameData, sizeof(frameData), sizeof(frameData));
}

void RenderContext::EndCamera( const Camera& camera )
{
	UNUSED(camera);
	m_isDrawing = false;
	FinalizeLight();
}

void RenderContext::EndFrame()
{
	m_swapChain->Present();
	DX_SAFE_RELEASE(m_currentDepthStencilState);
	//SwapBuffers( g_displayDeviceContext );
}

Shader* RenderContext::GetOrCreateShader(char const* filename)
{
	std::map<const char*, Shader*>::iterator itr = m_Shaders.find(filename);
	if (itr != m_Shaders.end())
	{
		return m_Shaders[filename];
	}
	else
	{
		Shader* shader = new Shader(this);
		shader->CreateFromFile(filename);
		m_Shaders[filename] = shader;
		return shader;
	}
}

void RenderContext::BindShader(char const* filename)
{
	std::map<const char*, Shader*>::iterator itr = m_Shaders.find(filename);
	if (itr != m_Shaders.end())
	{
		BindShader(m_Shaders[filename]);
		m_shaderHasChanged = true;
	}
	else
	{
		BindShader((Shader*)nullptr);
	}
}

void RenderContext::BindShader(Shader* shader)
{
	ASSERT_OR_DIE(m_isDrawing, "Bind Shader without camera");
	m_currentShader = shader;
	if (m_currentShader == nullptr)
	{
		m_currentShader = m_defaultShader;
	}
	m_context->VSSetShader(m_currentShader->m_vertexStage.m_vs, nullptr, 0);
	m_context->PSSetShader(m_currentShader->m_fragmentStage.m_fs, nullptr, 0);
}
void RenderContext::BindVertexInput(VertexBuffer* vbo)
{
	m_currentLayout = vbo->m_layout;
	ID3D11Buffer* vboHandle = vbo->m_handle;
	UINT stride = (UINT)vbo->m_elementByteSize;//how far from one vertex to next
	UINT offsets = 0;//how far from buffer do we start;

	//no need to call this two line if never change
	if (m_lastBoundVBO != vboHandle)
	{
		m_context->IASetVertexBuffers(0, 1, &vboHandle, &stride, &offsets);
		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_lastBoundVBO = vboHandle;
	}
}

void RenderContext::BindIndexBuffer(IndexBuffer* ibo)
{
	ID3D11Buffer* iboHandle = ibo->m_handle;
	//UINT stride = sizeof(unsigned int);//how far from one vertex to next
	//UINT offsets = 0;//how far from buffer do we start;

	//no need to call this two line if never change
	if (m_lastBoundIBO != iboHandle)
	{
		m_context->IASetIndexBuffer(iboHandle, DXGI_FORMAT_R32_UINT, 0);
		m_lastBoundIBO = iboHandle;
	}
}

void RenderContext::BindUniformBuffer(unsigned int slot, RenderBuffer* ubo)
{
	ID3D11Buffer* uboHandle = ubo->GetHandle();
	m_context->VSSetConstantBuffers(slot, 1, &uboHandle);
	m_context->PSSetConstantBuffers(slot, 1, &uboHandle);
}

void RenderContext::DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes )
{
	if (numVertexes <= 0)
	{
		return;
	}
	size_t elementSize = sizeof(Vertex_PCU);
	m_immediateMesh->UpdateVertices((unsigned int)numVertexes, vertexes, (unsigned int)elementSize, Vertex_PCU::LAYOUT);
	m_immediateMesh->UpdateIndices(0, nullptr);
	DrawMesh(m_immediateMesh);
}

void RenderContext::DrawSegment(const Vec2& start, const Vec2& end, float thickness, Rgba8 color)
{
	float half_thickness = thickness * 0.5f;
	Vec2 forward = (end - start).GetNormalized() * half_thickness;
	Vec2 left = forward.GetRotated90Degrees();
	Vec2 endLeft = end + forward + left;
	Vec2 endRight = end + forward - left;
	Vec2 startLeft = start - forward + left;
	Vec2 startRight = start - forward - left;
	Rgba8 segColor = color;
	Vec2 segUV = Vec2();
	Vertex_PCU vertices[] = {
		Vertex_PCU(Vec3(endLeft, 0.f), segColor, segUV),//triangle 1
		Vertex_PCU(Vec3(startRight, 0.f), segColor, segUV),
		Vertex_PCU(Vec3(endRight, 0.f), segColor, segUV),

		Vertex_PCU(Vec3(endLeft, 0.f), segColor, segUV),//triangle 2
		Vertex_PCU(Vec3(startLeft, 0.f), segColor, segUV),
		Vertex_PCU(Vec3(startRight, 0.f), segColor, segUV)
	};
	DrawVertexArray(6, vertices);
}

void RenderContext::DrawSegment3D(const Vec3& start, const Vec3& end, float thickness, Rgba8 color)
{
	GPUMesh mesh(this);
	std::vector<unsigned int> indices;
	std::vector<Vertex_PCU> vertices;
	AddCylinderToIndexedVertexArray(vertices, indices, start, end, thickness, color, color);
	mesh.UpdateIndices(indices);
	mesh.UpdateVertices(vertices);
	DrawMesh(&mesh);
}

void RenderContext::DrawCircle(const Vec2& center, float radius, float thickness, Rgba8 color)
{
	float half_thinkness = thickness * 0.5f;
	constexpr float deltaDegree = 360.f / 64.f;
	constexpr int numVertices = 64 * 2 * 3;
	float innerRadius = radius - half_thinkness;
	float outerRadius = radius + half_thinkness;
	Vertex_PCU vertices[numVertices];
	Vec2 circeUV = Vec2();
	for (int indexAngle = 0; indexAngle < 64; ++indexAngle)
	{
		Vec2 innerPosition1 = Vec2::MakeFromPolarDegrees(deltaDegree * (float)indexAngle, innerRadius);
		Vec2 outerPosition1 = Vec2::MakeFromPolarDegrees(deltaDegree * (float)indexAngle, outerRadius);
		Vertex_PCU innerVertex1 = Vertex_PCU(Vec3(innerPosition1, 0.f), color, circeUV);
		Vertex_PCU outerVertex1 = Vertex_PCU(Vec3(outerPosition1, 0.f), color, circeUV);

		Vec2 innerPosition2 = Vec2::MakeFromPolarDegrees(deltaDegree * (float)(indexAngle + 1), innerRadius);
		Vec2 outerPosition2 = Vec2::MakeFromPolarDegrees(deltaDegree * (float)(indexAngle + 1), outerRadius);
		Vertex_PCU innerVertex2 = Vertex_PCU(Vec3(innerPosition2, 0.f), color, circeUV);
		Vertex_PCU outerVertex2 = Vertex_PCU(Vec3(outerPosition2, 0.f), color, circeUV);

		vertices[indexAngle * 6] = innerVertex1;//triangle1
		vertices[indexAngle * 6 + 1] = outerVertex1;
		vertices[indexAngle * 6 + 2] = innerVertex2;

		vertices[indexAngle * 6 + 3] = outerVertex1;//triangle2
		vertices[indexAngle * 6 + 4] = outerVertex2;
		vertices[indexAngle * 6 + 5] = innerVertex2;
	}
	TransformVertexArray(numVertices, vertices, 1.f, 0.f, center);
	DrawVertexArray(numVertices, vertices);
}

void RenderContext::DrawAABB2(const AABB2& bounds, const Rgba8& tint)
{
	Vertex_PCU vertices[] = {
		Vertex_PCU(Vec3(bounds.mins.x, bounds.mins.y, 0.f), tint, Vec2(0.f, 0.f)),//triangle 1
		Vertex_PCU(Vec3(bounds.maxs.x, bounds.mins.y, 0.f), tint, Vec2(1.f, 0.f)),
		Vertex_PCU(Vec3(bounds.mins.x, bounds.maxs.y, 0.f), tint, Vec2(0.f, 1.f)),

		Vertex_PCU(Vec3(bounds.mins.x, bounds.maxs.y, 0.f), tint, Vec2(0.f, 1.f)),//triangle 2
		Vertex_PCU(Vec3(bounds.maxs.x, bounds.mins.y, 0.f), tint, Vec2(1.f, 0.f)),
		Vertex_PCU(Vec3(bounds.maxs.x, bounds.maxs.y, 0.f), tint, Vec2(1.f, 1.f))
	};
	DrawVertexArray(6, vertices);
}

void RenderContext::DrawAABB2(const AABB2& bounds, const Rgba8& tint, float z)
{
	Vertex_PCU vertices[] = {
		Vertex_PCU(Vec3(bounds.mins.x, bounds.mins.y, z), tint, Vec2(0.f, 0.f)),//triangle 1
		Vertex_PCU(Vec3(bounds.maxs.x, bounds.mins.y, z), tint, Vec2(1.f, 0.f)),
		Vertex_PCU(Vec3(bounds.mins.x, bounds.maxs.y, z), tint, Vec2(0.f, 1.f)),

		Vertex_PCU(Vec3(bounds.mins.x, bounds.maxs.y, z), tint, Vec2(0.f, 1.f)),//triangle 2
		Vertex_PCU(Vec3(bounds.maxs.x, bounds.mins.y, z), tint, Vec2(1.f, 0.f)),
		Vertex_PCU(Vec3(bounds.maxs.x, bounds.maxs.y, z), tint, Vec2(1.f, 1.f))
	};
	DrawVertexArray(6, vertices);
}

void RenderContext::DrawAABB2(const AABB2& bounds, const Rgba8& tint, const Vec2 uvAtMins, const Vec2 uvAtMaxs)
{
	Vertex_PCU vertices[] = {
		Vertex_PCU(Vec3(bounds.mins.x, bounds.mins.y, 0.f), tint, uvAtMins),//triangle 1
		Vertex_PCU(Vec3(bounds.maxs.x, bounds.mins.y, 0.f), tint, Vec2(uvAtMaxs.x, uvAtMins.y)),
		Vertex_PCU(Vec3(bounds.mins.x, bounds.maxs.y, 0.f), tint, Vec2(uvAtMins.x, uvAtMaxs.y)),

		Vertex_PCU(Vec3(bounds.mins.x, bounds.maxs.y, 0.f), tint, Vec2(uvAtMins.x, uvAtMaxs.y)),//triangle 2
		Vertex_PCU(Vec3(bounds.maxs.x, bounds.mins.y, 0.f), tint, Vec2(uvAtMaxs.x, uvAtMins.y)),
				Vertex_PCU(Vec3(bounds.maxs.x, bounds.maxs.y, 0.f), tint, uvAtMaxs)
	};
	DrawVertexArray(6, vertices);
}

void RenderContext::DrawDisc(const Vec2& center, const Vec2& offset, float radius, float orientationDegree, Rgba8 color)
{
	constexpr float deltaTheta = 360.f / 48.f;
	Vertex_PCU vertices[144];
	//const Rgba8 asteroidColor = m_color;
	const Vec2 asteroidUV = Vec2();
	Vertex_PCU point1 = Vertex_PCU(Vec3(0.f, 0.f, 0.f), color, asteroidUV);
	for (int indexTriangle = 0; indexTriangle < 48; ++indexTriangle)
	{
		Vec3 pos2 = Vec3(Vec2::MakeFromPolarDegrees(deltaTheta * (float)indexTriangle, radius), 0.f);
		Vec3 pos3 = Vec3(Vec2::MakeFromPolarDegrees(deltaTheta * (float)(indexTriangle + 1), radius), 0.f);
		Vertex_PCU point2 = Vertex_PCU(pos2, color, asteroidUV);
		Vertex_PCU point3 = Vertex_PCU(pos3, color, asteroidUV);
		vertices[indexTriangle * 3] = point1;
		vertices[indexTriangle * 3 + 1] = point2;
		vertices[indexTriangle * 3 + 2] = point3;
	}
	TransformVertexArray(144, vertices, 1.f, orientationDegree, center, offset);
	DrawVertexArray(144, vertices);
}

void RenderContext::DrawSemiDisc(const Vec2& center, const Vec2& offset, float radius, float orientationDegree, Rgba8 color)
{
	constexpr float deltaTheta = 360.f / 48.f;
	Vertex_PCU vertices[72];
	//const Rgba8 asteroidColor = m_color;
	const Vec2 asteroidUV = Vec2();
	Vertex_PCU point1 = Vertex_PCU(Vec3(0.f, 0.f, 0.f), color, asteroidUV);
	for (int indexTriangle = 0; indexTriangle < 24; ++indexTriangle)
	{
		Vec3 pos2 = Vec3(Vec2::MakeFromPolarDegrees(deltaTheta * (float)indexTriangle, radius), 0.f);
		Vec3 pos3 = Vec3(Vec2::MakeFromPolarDegrees(deltaTheta * (float)(indexTriangle + 1), radius), 0.f);
		Vertex_PCU point2 = Vertex_PCU(pos2, color, asteroidUV);
		Vertex_PCU point3 = Vertex_PCU(pos3, color, asteroidUV);
		vertices[indexTriangle * 3] = point1;
		vertices[indexTriangle * 3 + 1] = point2;
		vertices[indexTriangle * 3 + 2] = point3;
	}
	TransformVertexArray(72, vertices, 1.f, orientationDegree, center, offset);
	DrawVertexArray(72, vertices);
}

void RenderContext::DrawOBB2(const OBB2& box, const Rgba8& tint)
{
	Vec2 boxPoints[4];
	box.GetCornerPositions(boxPoints);
	Vertex_PCU vertices[] = {
		Vertex_PCU(Vec3(boxPoints[0].x, boxPoints[0].y, 0.f), tint, Vec2::ZERO),//triangle 1
		Vertex_PCU(Vec3(boxPoints[1].x, boxPoints[1].y, 0.f), tint, Vec2::ZERO),
		Vertex_PCU(Vec3(boxPoints[2].x, boxPoints[2].y, 0.f), tint, Vec2::ZERO),

		Vertex_PCU(Vec3(boxPoints[1].x, boxPoints[1].y, 0.f), tint, Vec2::ZERO),//triangle 2
		Vertex_PCU(Vec3(boxPoints[3].x, boxPoints[3].y, 0.f), tint, Vec2::ZERO),
		Vertex_PCU(Vec3(boxPoints[2].x, boxPoints[2].y, 0.f), tint, Vec2::ZERO),
	};
	DrawVertexArray(6, vertices);
}

void RenderContext::DrawCapsule2(const Capsule2& capsule, const Rgba8& tint)
{
	OBB2 box = capsule.GetOBB2AlongTheLine();
	DrawOBB2(box, tint);
	float orientationDegrees = box.GetIBasisNormal().GetAngleDegrees();
	float offset = box.GetDimensions().y * 0.5f;
	DrawSemiDisc(capsule.GetCenter(), Vec2(0.f, offset), capsule.m_radius, orientationDegrees, tint);
	DrawSemiDisc(capsule.GetCenter(), Vec2(0.f, offset), capsule.m_radius, orientationDegrees + 180.f, tint);
}

void RenderContext::DrawPolygon2(const Polygon2D& polygon, const Rgba8& tint)
{
	Vec2 center = polygon.GetCenter();
	int numPoints = polygon.GetVertexCount();

	std::vector<Vec2> points = polygon.GetPoints();
	std::vector<Vertex_PCU> vertices;
	for (int edgeIndex = 0; edgeIndex < numPoints - 1; ++edgeIndex)
	{
		vertices.push_back(Vertex_PCU(Vec3(center, 0.f), tint, Vec2::ZERO));
		vertices.push_back(Vertex_PCU(Vec3(points[edgeIndex], 0.f), tint, Vec2::ZERO));
		vertices.push_back(Vertex_PCU(Vec3(points[edgeIndex + 1], 0.f), tint, Vec2::ZERO));
	}
	vertices.push_back(Vertex_PCU(Vec3(center, 0.f), tint, Vec2::ZERO));
	vertices.push_back(Vertex_PCU(Vec3(points[numPoints - 1], 0.f), tint, Vec2::ZERO));
	vertices.push_back(Vertex_PCU(Vec3(points[0], 0.f), tint, Vec2::ZERO));

	DrawVertexArray((int)vertices.size(), vertices.data());
}

void RenderContext::BindTexture(const Texture* consttexture)
{
	if (consttexture)
	{
		Texture* texture = const_cast<Texture*>(consttexture);
		TextureView* shaderResourceView = texture->GetOrCreateShaderResourceView();
		ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetAsSRV();
		m_context->PSSetShaderResources(0, 1, &srvHandle);//srv
	}
	else
	{
		TextureView* shaderResourceView = m_whiteTexture->GetOrCreateShaderResourceView();
		ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetAsSRV();
		m_context->PSSetShaderResources(0, 1, &srvHandle);//srv
	}
}

void RenderContext::BindTexture(const Texture* consttexture, int index)
{
	if (consttexture)
	{
		Texture* texture = const_cast<Texture*>(consttexture);
		TextureView* shaderResourceView = texture->GetOrCreateShaderResourceView();
		ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetAsSRV();
		m_context->PSSetShaderResources(index, 1, &srvHandle);//srv
	}
	else
	{
		TextureView* shaderResourceView = m_whiteTexture->GetOrCreateShaderResourceView();
		ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetAsSRV();
		m_context->PSSetShaderResources(index, 1, &srvHandle);//srv
	}
}

void RenderContext::BindNormalTexture(const Texture* consttexture)
{
	if (consttexture)
	{
		Texture* texture = const_cast<Texture*>(consttexture);
		TextureView* shaderResourceView = texture->GetOrCreateShaderResourceView();
		ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetAsSRV();
		m_context->PSSetShaderResources(1, 1, &srvHandle);//srv
	}
	else
	{
		TextureView* shaderResourceView = m_defaultNormalTexture->GetOrCreateShaderResourceView();
		ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetAsSRV();
		m_context->PSSetShaderResources(1, 1, &srvHandle);//srv
	}
}

void RenderContext::BindNormalTexture(const Texture* consttexture, int index)
{
	if (consttexture)
	{
		Texture* texture = const_cast<Texture*>(consttexture);
		TextureView* shaderResourceView = texture->GetOrCreateShaderResourceView();
		ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetAsSRV();
		m_context->PSSetShaderResources(index, 1, &srvHandle);//srv
	}
	else
	{
		TextureView* shaderResourceView = m_defaultNormalTexture->GetOrCreateShaderResourceView();
		ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetAsSRV();
		m_context->PSSetShaderResources(index, 1, &srvHandle);//srv
	}
}

void RenderContext::BindSampler(const Sampler* sampler)
{
	ID3D11SamplerState* samplerHandle = sampler->GetHandle();
	m_context->PSSetSamplers(0, 1, &samplerHandle);
}

void RenderContext::BindSampler(const Sampler* sampler, int index)
{
	ID3D11SamplerState* samplerHandle = sampler->GetHandle();
	m_context->PSSetSamplers(index, 1, &samplerHandle);
}

Texture* RenderContext::CreateTextureFromFile(const char* imageFilePath)
{
	int m_imageTexelSizeX = 0; // This will be filled in for us to indicate image width
	int m_imageTexelSizeY = 0; // This will be filled in for us to indicate image height
	int m_numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int m_numComponentsRequested = 4; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)
	unsigned char* m_imageData = 0;

	stbi_set_flip_vertically_on_load(1);
	m_imageData = stbi_load(imageFilePath, &m_imageTexelSizeX, &m_imageTexelSizeY, &m_numComponents, m_numComponentsRequested);
	GUARANTEE_OR_DIE(m_imageData, Stringf("Failed to load image \"%s\"", imageFilePath));
	GUARANTEE_OR_DIE((m_numComponents == 4 || m_numComponents == 3 || m_numComponents == 1) && m_imageTexelSizeX > 0 && m_imageTexelSizeY > 0, Stringf("ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, m_numComponents, m_imageTexelSizeX, m_imageTexelSizeY));

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = m_imageTexelSizeX;
	desc.Height = m_imageTexelSizeY;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE; //if we do mid chains, we need this to be GPU/DEFAULT
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = m_imageData;
	initialData.SysMemPitch = m_imageTexelSizeX * 4;
	initialData.SysMemSlicePitch = 0;

	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D(&desc, &initialData, &texHandle);

	stbi_image_free(m_imageData);
	Texture* texture = new Texture(imageFilePath, this, texHandle);
	m_textures.push_back(texture);
	return texture;
}

Texture* RenderContext::CreateTextureFromColor(Rgba8 color)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = 1;
	desc.Height = 1;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE; //if we do mid chains, we need this to be GPU/DEFAULT
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	unsigned char imageData[4];
	imageData[0] = color.r;
	imageData[1] = color.g;
	imageData[2] = color.b;
	imageData[3] = color.a;
	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = imageData;
	initialData.SysMemPitch = 4;
	initialData.SysMemSlicePitch = 0;

	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D(&desc, &initialData, &texHandle);

	Texture* texture = new Texture(this, texHandle);
	m_textures.push_back(texture);
	return texture;
}

Texture* RenderContext::CreateRenderTarget(IntVec2 texelSize)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = texelSize.x;
	desc.Height = texelSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT; //if we do mid chains, we need this to be GPU/DEFAULT
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D(&desc, nullptr, &texHandle);

	Texture* texture = new Texture(this, texHandle);
	m_textures.push_back(texture);//leak if delete inside game
	return texture;
}

Texture* RenderContext::AcquireRenderTargetMatching(Texture* texture)
{
	IntVec2 size = texture->GetTextureSize();
	for (int i = 0; i < m_renderTargetPool.size(); ++i)
	{
		Texture* rt = m_renderTargetPool[i];
		if (rt->GetTextureSize() == size)
		{
			m_renderTargetPool[i] = m_renderTargetPool[m_renderTargetPool.size() - 1];
			m_renderTargetPool.pop_back();

			return rt;
		}
	}
	//in case nothing in pool
	
	++m_totalRenderTargetMade;
	Texture* nrt = CreateRenderTarget(size);
	return nrt;
}

void RenderContext::ReleaseRenderTarget(Texture* tex)
{
	m_renderTargetPool.push_back(tex);
}

void RenderContext::BeginEffect(Texture* dst, Texture* src, Shader* shader)
{
	//identity project view and model
	m_effectCamera->SetColorTarget(dst);
	BeginCamera(*m_effectCamera);
	BindShader(shader);
	BindTexture(src);
}

void RenderContext::EndEffect()
{
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->Draw(3, 0);
	EndCamera(*m_effectCamera);
}

void RenderContext::CopyTexture(Texture* dst, Texture* src)
{
	m_context->CopyResource(dst->GetHandler(), src->GetHandler());
}

Texture* RenderContext::CreateOrGetTextureFromFile(const char* imageFilePath)
{
	for (int textureIndex = 0; textureIndex < m_textures.size(); ++textureIndex)
	{
		if (m_textures[textureIndex]->m_imageFilePath == imageFilePath)
		{
			return m_textures[textureIndex];
		}
	}
	return CreateTextureFromFile(imageFilePath);
}

void RenderContext::UpdateLayoutIfNeeded()
{
	if ((m_previouslyBoundLayout != m_currentLayout)
		|| (m_shaderHasChanged)) {

		ID3D11InputLayout* layout = m_currentShader->GetOrCreateInputLayout(m_currentLayout);
		m_context->IASetInputLayout(layout);

		m_previouslyBoundLayout = m_currentLayout;
		m_shaderHasChanged = false;
	}
}

void RenderContext::DrawMesh(GPUMesh* mesh)
{
	BindVertexInput(mesh->GetVertexBuffer());

	// m_context->IASetInputLayout
	// this needs both a shader and a vertex format
	//m_currentLayout = mesh->m_vertices->m_layout;

	bool hasIndices = mesh->GetIndexCount() > 0;

	if (hasIndices) {
		BindIndexBuffer(mesh->GetIndexBuffer());
		DrawIndexed(mesh->GetIndexCount(), 0);
	}
	else {
		Draw(mesh->GetVertexCount(), 0);
	}
}

void RenderContext::SetDepthTest(eCompareFunc func, bool writeDepthOnPass)
{
	if (m_currentDepthStencilState)
	{
		DX_SAFE_RELEASE(m_currentDepthStencilState);
	}
	D3D11_DEPTH_STENCIL_DESC desc;
	// Depth test parameters
	desc.DepthEnable = true;
	desc.DepthWriteMask = desc.DepthWriteMask = writeDepthOnPass ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	switch (func)
	{
	case COMPARE_FUNC_NEVER:
		desc.DepthFunc = D3D11_COMPARISON_NEVER;
		break;
	case COMPARE_FUNC_ALWAYS:
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		break;
	case COMPARE_FUNC_LEQUAL:
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		break;
	case COMPARE_FUNC_GEQUAL:
		desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
		break;
	default:
		break;
	}
	// Stencil test parameters
	desc.StencilEnable = false;
	desc.StencilReadMask = 0xFF;
	desc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	m_device->CreateDepthStencilState(&desc, &m_currentDepthStencilState);
	m_context->OMSetDepthStencilState(m_currentDepthStencilState, 1);
}

void RenderContext::SetModelMatrix(Mat44 mat)
{
	m_objectInfo.model = mat;
	m_objUBO->Update(&m_objectInfo, sizeof(object_info_t), sizeof(object_info_t));
	BindUniformBuffer(UBO_MODEL_SLOT, m_objUBO);
}

void RenderContext::SetShaderTint(Vec4 color)
{
	m_objectInfo.tint = color;
	m_objUBO->Update(&m_objectInfo, sizeof(object_info_t), sizeof(object_info_t));
	BindUniformBuffer(UBO_MODEL_SLOT, m_objUBO);
}

void RenderContext::SetGlitchEffectStrength(float strength)
{
	glitch_t newGlitch;
	newGlitch.strength = strength;
	m_glitchUBO->Update(&newGlitch, sizeof(glitch_t), sizeof(glitch_t));
	BindUniformBuffer(UBO_GLITCH_SLOT, m_glitchUBO);
}

void RenderContext::Setup(Clock* game_clock)
{
	m_clock = game_clock;
	if (m_clock == nullptr) {
		m_clock = Clock::GetMaster();
	}
}

void RenderContext::CreateOrUpdateRasterizerState()
{
	DX_SAFE_RELEASE(m_rasterState);
	D3D11_RASTERIZER_DESC desc;
	switch (m_fillMode)
	{
	case WIREFRAME:
		desc.FillMode = D3D11_FILL_WIREFRAME;
		break;
	case SOLID:
		desc.FillMode = D3D11_FILL_SOLID;
		break;
	default:
		desc.FillMode = D3D11_FILL_SOLID;
		break;
	}
	switch (m_cullMode)
	{
	case NONE:
		desc.CullMode = D3D11_CULL_NONE;
		break;
	case FRONT:
		desc.CullMode = D3D11_CULL_FRONT;
		break;
	case BACK:
		desc.CullMode = D3D11_CULL_BACK;
		break;
	default:
		desc.CullMode = D3D11_CULL_NONE;
		break;
	}
	desc.FrontCounterClockwise = m_frontCounterClockWise; // the only reason we're doing this; 
	desc.DepthBias = 0U;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	m_device->CreateRasterizerState(&desc, &m_rasterState);
}

void RenderContext::SetCullMode(eCullMode mode)
{
	m_cullMode = mode;
	CreateOrUpdateRasterizerState();
	m_context->RSSetState(m_rasterState);
}

void RenderContext::SetFillMode(eFillMode mode)
{
	m_fillMode = mode;
	CreateOrUpdateRasterizerState();
	m_context->RSSetState(m_rasterState);
}

void RenderContext::SetFrontFaceWindOrder(bool frontCounterClockwise)
{
	m_frontCounterClockWise = frontCounterClockwise;
	CreateOrUpdateRasterizerState();
	m_context->RSSetState(m_rasterState);
}

void RenderContext::ResetRasterizerState()
{
	m_cullMode = BACK;
	m_fillMode = SOLID;
	m_frontCounterClockWise = true;
	CreateOrUpdateRasterizerState();
	m_context->RSSetState(m_rasterState);
}

Texture* RenderContext::GetDefaultColorTarget()
{
	return m_swapChain->GetBackBuffer();
}

void RenderContext::SetAmbientColor(Rgba8 color)
{
	Vec3 newColor = color.GetAsVec4().XYZ();
	m_lightInfo.ambient.x = newColor.x;
	m_lightInfo.ambient.y = newColor.y;
	m_lightInfo.ambient.z = newColor.z;
	m_hasLightChanged = true;
}

void RenderContext::SetAmbientIntensity(float intensity)
{
	m_lightInfo.ambient.w = intensity;
	m_hasLightChanged = true;
}

void RenderContext::SetAmbientLight(Rgba8 color, float intensity)
{
	SetAmbientColor(color);
	SetAmbientIntensity(intensity);
}

void RenderContext::SetAttenuation(Vec3 att, unsigned int lightIndex)
{
	m_lightInfo.LIGHT[lightIndex].attenuation = att;
	m_hasLightChanged = true;
}

void RenderContext::SetLightPosition(Vec3 pos, unsigned int lightIndex)
{
	m_lightInfo.LIGHT[lightIndex].world_position = pos;
	m_hasLightChanged = true;
}

void RenderContext::SetLightColor(Rgba8 color, unsigned int lightIndex)
{
	m_lightInfo.LIGHT[lightIndex].color = color.GetAsVec4().XYZ();
	m_hasLightChanged = true;
}

void RenderContext::SetLightColor(Vec3 color, unsigned int lightIndex)
{
	m_lightInfo.LIGHT[lightIndex].color = color;
	m_hasLightChanged = true;
}

void RenderContext::SetLightIntensity(float intensity, unsigned int lightIndex)
{
	m_lightInfo.LIGHT[lightIndex].intensity = intensity;
	m_hasLightChanged = true;
}

void RenderContext::SetSpecularFactor(float factor)
{
	m_objectInfo.specularFactor = factor;
	m_objUBO->Update(&m_objectInfo, sizeof(object_info_t), sizeof(object_info_t));
	BindUniformBuffer(UBO_MODEL_SLOT, m_objUBO);
}

void RenderContext::SetSpecularPower(float power)
{
	m_objectInfo.specularPower = power;
	m_objUBO->Update(&m_objectInfo, sizeof(object_info_t), sizeof(object_info_t));
	BindUniformBuffer(UBO_MODEL_SLOT, m_objUBO);
}

void RenderContext::EnableLight(light_t const& lightInfo, unsigned int lightIndex)
{
	m_lightInfo.LIGHT[lightIndex] = lightInfo;
	m_hasLightChanged = true;
}

void RenderContext::DisableLight(unsigned int lightIndex)
{
	m_lightInfo.LIGHT[lightIndex].intensity = 0;
	m_hasLightChanged = true;
}

void RenderContext::FinalizeLight()
{
	if (m_hasLightChanged)
	{
		m_lightUBO->Update(&m_lightInfo, sizeof(light_info_t), sizeof(light_info_t));
		BindUniformBuffer(UBO_LIGHT_SLOT, m_lightUBO);
	}
}

void RenderContext::SetDissolveDepth(float depth)
{
	m_materialInfo.depth = depth;
	m_hasMaterialChanged = true;
}
void RenderContext::SetDissolveRange(float range)
{
	m_materialInfo.range = range;
	m_hasMaterialChanged = true;
}
void RenderContext::SetDissolveStartColor(Rgba8 color)
{
	m_materialInfo.burn_start_color = color.GetAsVec4().XYZ();
	m_hasMaterialChanged = true;
}
void RenderContext::SetDissolveEndColor(Rgba8 color)
{
	m_materialInfo.burn_end_color = color.GetAsVec4().XYZ();
	m_hasMaterialChanged = true;
}

void RenderContext::SetParallexDepth(float depth)
{
	m_materialInfo.DEPTH = depth;
	m_hasMaterialChanged = true;
}

void RenderContext::SetParallexStep(int step)
{
	m_materialInfo.STEP_COUNT = step;
	m_hasMaterialChanged = true;
}

void RenderContext::FinalizeMaterial()
{
	if (m_hasMaterialChanged)
	{
		m_materialUBO->Update(&m_materialInfo, sizeof(material_t), sizeof(material_t));
		BindUniformBuffer(UBO_MATERIAL_SLOT, m_materialUBO);
	}
}

void RenderContext::SetProject(project_t project)
{
	m_projectInfo = project;
	m_projectUBO->Update(&m_projectInfo, sizeof(project_t), sizeof(project_t));
	BindUniformBuffer(UBO_PROJECT_SLOT, m_projectUBO);
}

void RenderContext::SetGrayScaleForce(float strength)
{
	m_effectInfo.grayScaleFactor = strength;
	m_effectUBO->Update(&m_effectInfo, sizeof(effect_t), sizeof(effect_t));
	BindUniformBuffer(UBO_EFFECT_SLOT, m_effectUBO);
}

void RenderContext::SetTintForce(float strength)
{
	m_effectInfo.TintFactor = strength;
	m_effectUBO->Update(&m_effectInfo, sizeof(effect_t), sizeof(effect_t));
	BindUniformBuffer(UBO_EFFECT_SLOT, m_effectUBO);
}

void RenderContext::SetGrayScaleTint(Vec4 color)
{
	m_effectInfo.tint = color;
	m_effectUBO->Update(&m_effectInfo, sizeof(effect_t), sizeof(effect_t));
	BindUniformBuffer(UBO_EFFECT_SLOT, m_effectUBO);
}

void RenderContext::EnableFog(float nearFog, float farFog, Rgba8 nearFogColor, Rgba8 farFogColor)
{
	m_lightInfo.fog_near_distance = nearFog;
	m_lightInfo.fog_far_distance = farFog;
	m_lightInfo.FOG_NEAR_COLOR = nearFogColor.GetAsVec4().XYZ();
	m_lightInfo.FOG_FAR_COLOR = farFogColor.GetAsVec4().XYZ();
	m_hasLightChanged = true;
}

void RenderContext::DisableFog()
{
	m_lightInfo.fog_near_distance = 0.f;
	m_lightInfo.fog_far_distance = 10000.f;
	m_hasLightChanged = true;
}

void RenderContext::CreateBlendStates()
{
	D3D11_BLEND_DESC desc;
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;
	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_device->CreateBlendState(&desc, &m_alphaBlendState);


	D3D11_BLEND_DESC additiveDesc;
	additiveDesc.AlphaToCoverageEnable = false;
	additiveDesc.IndependentBlendEnable = false;
	additiveDesc.RenderTarget[0].BlendEnable = true;
	additiveDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	additiveDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	additiveDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	additiveDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	additiveDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	additiveDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	additiveDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_device->CreateBlendState(&additiveDesc, &m_additiveBlendState);

	D3D11_BLEND_DESC opaqueDesc;
	opaqueDesc.AlphaToCoverageEnable = false;
	opaqueDesc.IndependentBlendEnable = false;
	opaqueDesc.RenderTarget[0].BlendEnable = false;
	opaqueDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_device->CreateBlendState(&opaqueDesc, &m_opaqueBlendState);
}

void RenderContext::SetBlendMode(BlendMode blendMode)
{
	float const zeroes[] = {0, 0, 0, 0};
	
	switch (blendMode)
	{
	case BlendMode::ALPHA: m_context->OMSetBlendState(m_alphaBlendState, zeroes, (UINT)~0); break;
	case BlendMode::ADDITIVE: m_context->OMSetBlendState(m_additiveBlendState, zeroes, (UINT)~0); break;
	case BlendMode::OPAQUEBLEND: m_context->OMSetBlendState(m_opaqueBlendState, zeroes, (UINT)~0); break;
	}
}

BitmapFont* RenderContext::CreateOrGetBitmapFromFIle(const char* imageFilePath)
{
	for (int fontIndex = 0; fontIndex < m_fonts.size(); ++fontIndex)
	{
		if (m_fonts[fontIndex]->m_fontName == imageFilePath)
		{
			return m_fonts[fontIndex];
		}
	}
	return CreateBitmapFont(imageFilePath);

}

BitmapFont* RenderContext::CreateBitmapFont(const char* imageFilePath)
{
	//g_theConsole->PrintString(Rgba8(255, 0, 0, 255), "Loading Font");
	std::string imageFilePathWithExtension = Stringf("%s.png", imageFilePath);
	Texture* fontTexture = CreateOrGetTextureFromFile(imageFilePathWithExtension.c_str());
	BitmapFont* bitmapFont = new BitmapFont(imageFilePath, fontTexture);
	m_fonts.push_back(bitmapFont);
	return bitmapFont;
}

void RenderContext::Draw(int numVertices, int vertexOffset)
{
	UpdateLayoutIfNeeded();
// 	ID3D11InputLayout* inputLayout = m_currentShader->GetOrCreateInputLayout(m_currentLayout);
// 	m_context->IASetInputLayout(inputLayout);
	m_context->Draw(numVertices, vertexOffset);
}

void RenderContext::DrawIndexed(int numVertices, int vertexOffset)
{
	//TODO, input layout instead
	UpdateLayoutIfNeeded();
// 	ID3D11InputLayout* inputLayout = m_currentShader->GetOrCreateInputLayout(m_currentLayout);
// 	m_context->IASetInputLayout(inputLayout);
	m_context->DrawIndexed(numVertices, vertexOffset, 0);
}