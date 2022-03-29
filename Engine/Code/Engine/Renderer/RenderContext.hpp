#pragma once
#include <vector>
#include <map>
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec4.hpp"

struct AABB2;
struct OBB2;
struct Capsule2;
struct Vec4;
class Polygon2D;
struct ID3D11Device; 
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
class Texture;
class BitmapFont;
class Window;
class SwapChain;
class Shader;
class IndexBuffer;
class VertexBuffer;
class RenderBuffer;
class Sampler;
class GPUMesh;
class Clock;
struct IntVec2;
enum eCompareFunc
{
	COMPARE_FUNC_NEVER,           // D3D11_COMPARISON_NEVER
	COMPARE_FUNC_ALWAYS,          // D3D11_COMPARISON_ALWAYS
	COMPARE_FUNC_LEQUAL,          // D3D11_COMPARISON_LESS_EQUAL
	COMPARE_FUNC_GEQUAL,          // D3D11_COMPARISON_GRE	ATER_EQUAL
};

enum class BlendMode
{
	ALPHA,
	ADDITIVE,
	OPAQUEBLEND,
};

enum eBufferSlot
{
	UBO_FRAME_SLOT,
	UBO_CAMERA_SLOT,
	UBO_MODEL_SLOT,
	UBO_LIGHT_SLOT,
	UBO_MATERIAL_SLOT,
	UBO_PROJECT_SLOT,
	UBO_EFFECT_SLOT,
	UBO_GLITCH_SLOT
};

enum eFillMode
{
	WIREFRAME,
	SOLID
};

enum eCullMode
{
	NONE,
	FRONT,
	BACK
};
//data for frame ubo
struct frame_data_t
{
	float system_time;
	float system_delta_time;
	float padding[2];
};

struct camera_data_t
{
	Mat44 projection;
	Mat44 view;
	Vec3 position;
	float pad00 = 0.f;
};

struct light_t
{
	Vec3 world_position;
	float innerAngle = -1.f;

	Vec3 color = Vec3(1.0f, 1.0f, 1.0f);
	float intensity = 0.f;

	Vec3 attenuation = Vec3(0.0f, 1.0f, 0.0f);
	float outerAngle = -1.f;
	
	Vec3 specAttenuation = Vec3(0.0f, 1.0f, 0.0f);
	float light_pad00 = 0.f;

	Vec3 direction = Vec3(0.f, 0.f, -1.f);
	float directionFactor = 0.f;
};

struct light_info_t
{
	Vec4 ambient = Vec4(1.f, 1.f, 1.f, 1.f);

	Vec4 diffuse = Vec4(1.f, 1.f, 1.f, 1.f);

	float diffuse_factor = 1.0f;
	float emissive_factor = 1.0f;
	float fog_near_distance = 0.f;
	float fog_far_distance = 30.f;

	Vec3 FOG_NEAR_COLOR = Vec3(1.f, 1.f, 1.f);;
	float gamma = 1.f;

	Vec3 FOG_FAR_COLOR = Vec3(0.f, 0.f, 0.f);
	float light_info_pad00 = 0.f;

	light_t LIGHT[8];
};

struct material_t
{
	Vec3 burn_start_color = Vec3(1.f, 0.f, 0.f);
	float depth = 0.0f;

	Vec3 burn_end_color = Vec3(0.f, 1.f, 0.f);
	float range = 0.0f;

	unsigned int STEP_COUNT = 10;
	float DEPTH = 0.0f;
	Vec2 material_pad00;
};

struct project_t
{
	Mat44 PROJECTION_MATRIX;
	Vec3 PROJECTOR_POSITION;
	float PROJECTOR_STRENGTH = 1.f;
};

struct object_info_t
{
	Mat44 model = Mat44::IDENTITY;

	Vec4 tint = Vec4(1.f, 1.f, 1.f, 1.f);

	float specularFactor = 1.f;
	float specularPower = 20.f;
	Vec2 obj_pad00;
};

struct effect_t
{
	float grayScaleFactor = 1.f;
	float TintFactor = 1.f;
	Vec2 pad00;

	Vec4 tint;
};

struct glitch_t
{
	float strength = 0.f;
	Vec3 pad00;
};

class RenderContext
{
public:
	~RenderContext();
	void Startup(Window* window);
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void Draw(int numVertices, int vertexOffset = 0);
	void DrawIndexed(int numVertices, int vertexOffset = 0);
	void ClearScreen(Camera camera);
	void ClearDepth(Texture* depthTarget, float depth);
	void BeginCamera(const Camera& camera);
	void EndCamera(const Camera& camera);
	void DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void DrawCircle(const Vec2& center, float radius, float thickness, Rgba8 color);
	void DrawSegment(const Vec2& start, const Vec2& end, float thickness, Rgba8 color);
	void DrawSegment3D(const Vec3& start, const Vec3& end, float thickness, Rgba8 color);
	void DrawAABB2(const AABB2& bounds, const Rgba8& tint);
	void DrawAABB2(const AABB2& bounds, const Rgba8& tint, float z);
	void DrawAABB2(const AABB2& bounds, const Rgba8& tint, const Vec2 uvAtMins, const Vec2 uvAtMaxs);
	void DrawDisc(const Vec2& center, const Vec2& offset, float radius, float orientationDegree, Rgba8 color);
	void DrawSemiDisc(const Vec2& center, const Vec2& offset, float radius, float orientationDegree, Rgba8 color);
	void DrawOBB2(const OBB2& box, const Rgba8& tint);
	void DrawCapsule2(const Capsule2& capsule, const Rgba8& tint);//Bad one
	void DrawPolygon2(const Polygon2D& polygon, const Rgba8& tint);
	void BindTexture(const Texture* consttexture);
	void BindTexture(const Texture* consttexture, int index);
	void BindNormalTexture(const Texture* consttexture);
	void BindNormalTexture(const Texture* consttexture, int index);
	void BindSampler(const Sampler* sampler);
	void BindSampler(const Sampler* sampler, int index);
	void BindShader(Shader* shader);
	void BindShader(char const* filename);
	void BindVertexInput(VertexBuffer* vbo);
	void BindIndexBuffer(IndexBuffer* ibo);
	void BindUniformBuffer(unsigned int slot, RenderBuffer* ubo);//ubo uniform buffer object
	void SetBlendMode(BlendMode blendMode);
	void UpdateFrameTime();
	Texture* CreateOrGetTextureFromFile(const char* imageFilePath);
	Texture* CreateTextureFromColor(Rgba8 color);
	Texture* CreateRenderTarget(IntVec2 texelSize);
	Texture* AcquireRenderTargetMatching(Texture* texture);
	void ReleaseRenderTarget(Texture* tex);
	void BeginEffect(Texture* dst, Texture* src, Shader* shader);
	void EndEffect();
	int GetTotalRenderTargetPoolSize() { return (int)m_totalRenderTargetMade; }
	int GetTexturePoolFreeCount() { return (int)m_renderTargetPool.size(); }
	void CopyTexture(Texture* dst, Texture* src);
	BitmapFont* CreateOrGetBitmapFromFIle(const char* imageFilePath);
	Shader* GetOrCreateShader(char const* filename);
	void UpdateLayoutIfNeeded();
	void DrawMesh(GPUMesh* mesh);
	void SetDepthTest(eCompareFunc func, bool writeDepthOnPass);
	void SetModelMatrix(Mat44 mat);
	void SetShaderTint(Vec4 color);
	void SetGlitchEffectStrength(float strength);
	Texture* GetDefaultDepthBuffer() { return m_defaultDepthBuffer; }
	void Setup(Clock* game_clock);
	void CreateOrUpdateRasterizerState();
	void SetCullMode(eCullMode mode);
	void SetFillMode(eFillMode mode);
	void SetFrontFaceWindOrder(bool frontCounterClockwise);
	void ResetRasterizerState();
	Texture* GetDefaultColorTarget();

	void SetAmbientColor(Rgba8 color);
	void SetAmbientIntensity(float intensity);
	void SetAmbientLight(Rgba8 color, float intensity);

	void SetAttenuation(Vec3 att, unsigned int lightIndex = 0);
	void SetLightPosition(Vec3 pos, unsigned int lightIndex = 0);
	void SetLightColor(Rgba8 color, unsigned int lightIndex = 0);
	void SetLightColor(Vec3 color, unsigned int lightIndex = 0);
	void SetLightIntensity(float intensity, unsigned int lightIndex = 0);
	void SetSpecularFactor(float factor);
	void SetSpecularPower(float power);

	void EnableLight(light_t const& lightInfo, unsigned int lightIndex = 0);
	void DisableLight(unsigned int lightIndex = 0);
	void FinalizeLight();
	void EnableFog(float nearFog, float farFog, Rgba8 nearFogColor, Rgba8 farFogColor);
	void DisableFog();

	void SetDissolveDepth(float depth);
	void SetDissolveRange(float range);
	void SetDissolveStartColor(Rgba8 color);
	void SetDissolveEndColor(Rgba8 color);
	void SetParallexDepth(float depth);
	void SetParallexStep(int step);
	void FinalizeMaterial();

	void SetProject(project_t project);

	void SetGrayScaleForce(float strength);
	void SetTintForce(float strength);
	void SetGrayScaleTint(Vec4 color);
private:
	//private functions
	Texture* CreateTextureFromFile(const char* imageFilePath);
	BitmapFont* CreateBitmapFont(const char* imageFilePath);
	void CreateBlendStates();
private:
	std::vector< Texture* > m_textures;
	std::vector< Texture* > m_renderTargetPool;
	int m_totalRenderTargetMade = 0; //debug purpose

	std::map< char const*, Shader*> m_Shaders;
	std::vector< BitmapFont* > m_fonts;
	bool m_isDrawing = false;
	bool m_shaderHasChanged = false;
	buffer_attribute_t const* m_currentLayout = nullptr; 
	buffer_attribute_t const* m_previouslyBoundLayout = nullptr;
	ID3D11DepthStencilState* m_currentDepthStencilState = nullptr;

	object_info_t m_objectInfo;
	light_info_t m_lightInfo;
	material_t m_materialInfo;
	project_t m_projectInfo;
	effect_t m_effectInfo;
	bool m_hasLightChanged = false;
	bool m_hasMaterialChanged = false;

	Camera* m_effectCamera = nullptr;
	//private variables

public:
	ID3D11Device* m_device = nullptr;//reference to GPU
	ID3D11DeviceContext* m_context = nullptr;//immediate context
	SwapChain* m_swapChain = nullptr;

	Shader* m_currentShader = nullptr;
	Shader* m_defaultShader = nullptr;
	ID3D11Buffer* m_lastBoundVBO = nullptr;

	GPUMesh* m_immediateMesh = nullptr;
	ID3D11Buffer* m_lastBoundIBO = nullptr;
	buffer_attribute_t* m_lastLayoutAttribs = nullptr;

	RenderBuffer* m_frameUBO = nullptr;
	RenderBuffer* m_cameraUBO = nullptr;
	RenderBuffer* m_objUBO = nullptr;
	RenderBuffer* m_lightUBO = nullptr;
	RenderBuffer* m_materialUBO = nullptr;
	RenderBuffer* m_projectUBO = nullptr;
	RenderBuffer* m_effectUBO = nullptr;
	RenderBuffer* m_glitchUBO = nullptr;

	Sampler* m_defaultSampler = nullptr;
	Sampler* m_projectSampler = nullptr;
	Texture* m_defaultTexture = nullptr;
	Texture* m_whiteTexture = nullptr;
	Texture* m_emptyTexture = nullptr;
	Texture* m_defaultNormalTexture = nullptr;
	Texture* m_defaultDepthBuffer = nullptr;
	ID3D11BlendState* m_alphaBlendState = nullptr;
	ID3D11BlendState* m_additiveBlendState = nullptr;
	ID3D11BlendState* m_opaqueBlendState = nullptr;
	ID3D11RasterizerState* m_rasterState = nullptr;
	eFillMode m_fillMode = SOLID;
	eCullMode m_cullMode = BACK;
	bool m_frontCounterClockWise = true;
	Clock* m_clock = nullptr;
};