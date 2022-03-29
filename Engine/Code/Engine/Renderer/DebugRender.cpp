#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <vector>
#include <stdarg.h>

class DebugRenderObject
{
public:
	bool IsReadyToBeCulled() const { return m_isGarbage; }
	void MarkAsGarbage() { m_isGarbage = true; }
	bool IsOld() const;
	virtual void Update(float deltaSeconds);
	eDebugRenderMode m_renderMode = DEBUG_RENDER_USE_DEPTH;
	eDebugFillMode m_fillMode = DEBUG_FILL_NORMAL;
public:
	std::vector<Vertex_PCU> m_vertices;
	std::vector<unsigned int> m_indices;
	float m_totalSeconds = 0.f;
	float m_remainSeconds = 0.f;
	bool m_isGarbage = false;
};

void DebugRenderObject::Update(float deltaSeconds)
{
	m_remainSeconds -= deltaSeconds;
}

class DebugRenderPointOrQuad : public DebugRenderObject
{
public:
	virtual void Update(float deltaSeconds) override;
public:
	Rgba8 m_startColor;
	Rgba8 m_endColor;
};

void DebugRenderPointOrQuad::Update(float deltaSeconds)
{
	DebugRenderObject::Update(deltaSeconds);
	unsigned char currR = (unsigned char)RangeMapFloat(m_totalSeconds, 0.f, (float)m_startColor.r, (float)m_endColor.r, m_remainSeconds);
	unsigned char currG = (unsigned char)RangeMapFloat(m_totalSeconds, 0.f, (float)m_startColor.g, (float)m_endColor.g, m_remainSeconds);
	unsigned char currB = (unsigned char)RangeMapFloat(m_totalSeconds, 0.f, (float)m_startColor.b, (float)m_endColor.b, m_remainSeconds);
	unsigned char currA = (unsigned char)RangeMapFloat(m_totalSeconds, 0.f, (float)m_startColor.a, (float)m_endColor.a, m_remainSeconds);
	Rgba8 currColor = Rgba8(currR, currG, currB, currA);
	for (int verticesIndex = 0; verticesIndex < m_vertices.size(); ++verticesIndex)
	{
		m_vertices[verticesIndex].m_color = currColor;
	}
}

class DebugRenderLineOrArrow : public DebugRenderObject
{
public:
	virtual void Update(float deltaSeconds) override;
public:
	Rgba8 m_p0StartColor;
	Rgba8 m_p0EndColor;
	Rgba8 m_p1StartColor;
	Rgba8 m_p1EndColor;
};

void DebugRenderLineOrArrow::Update(float deltaSeconds)
{
	DebugRenderObject::Update(deltaSeconds);
	unsigned char currR0 = (unsigned char)RangeMapFloat(m_totalSeconds, 0.f, (float)m_p0StartColor.r, (float)m_p0EndColor.r, m_remainSeconds);
	unsigned char currG0 = (unsigned char)RangeMapFloat(m_totalSeconds, 0.f, (float)m_p0StartColor.g, (float)m_p0EndColor.g, m_remainSeconds);
	unsigned char currB0 = (unsigned char)RangeMapFloat(m_totalSeconds, 0.f, (float)m_p0StartColor.b, (float)m_p0EndColor.b, m_remainSeconds);
	unsigned char currA0 = (unsigned char)RangeMapFloat(m_totalSeconds, 0.f, (float)m_p0StartColor.a, (float)m_p0EndColor.a, m_remainSeconds);
	Rgba8 currColor0 = Rgba8(currR0, currG0, currB0, currA0);
	for (int verticesIndex = 0; verticesIndex < 49; ++verticesIndex)
	{
		m_vertices[verticesIndex].m_color = currColor0;
	}

	unsigned char currR1 = (unsigned char)RangeMapFloat(m_totalSeconds, 0.f, (float)m_p1StartColor.r, (float)m_p1EndColor.r, m_remainSeconds);
	unsigned char currG1 = (unsigned char)RangeMapFloat(m_totalSeconds, 0.f, (float)m_p1StartColor.g, (float)m_p1EndColor.g, m_remainSeconds);
	unsigned char currB1 = (unsigned char)RangeMapFloat(m_totalSeconds, 0.f, (float)m_p1StartColor.b, (float)m_p1EndColor.b, m_remainSeconds);
	unsigned char currA1 = (unsigned char)RangeMapFloat(m_totalSeconds, 0.f, (float)m_p1StartColor.a, (float)m_p1EndColor.a, m_remainSeconds);
	Rgba8 currColor1 = Rgba8(currR1, currG1, currB1, currA1);
	for (int verticesIndex = 50; verticesIndex < m_vertices.size(); ++verticesIndex)
	{
		m_vertices[verticesIndex].m_color = currColor1;
	}
}

class DebugRenderBasis : public DebugRenderObject
{
public:
	virtual void Update(float deltaSeconds) override;
public:
	Rgba8 m_startTint = Rgba8::WHITE;
	Rgba8 m_endTint = Rgba8::WHITE;
};

void DebugRenderBasis::Update(float deltaSeconds)
{
	DebugRenderObject::Update(deltaSeconds);
	float RRatio = RangeMapFloat(m_totalSeconds, 0.f, (float)m_startTint.r, (float)m_endTint.r, m_remainSeconds) / 255.f;
	float GRatio = RangeMapFloat(m_totalSeconds, 0.f, (float)m_startTint.g, (float)m_endTint.g, m_remainSeconds) / 255.f;
	float BRatio = RangeMapFloat(m_totalSeconds, 0.f, (float)m_startTint.b, (float)m_endTint.b, m_remainSeconds) / 255.f;
	float ARatio = RangeMapFloat(m_totalSeconds, 0.f, (float)m_startTint.a, (float)m_endTint.a, m_remainSeconds) / 255.f;
	int numVertsEachArrow = (int)m_vertices.size() / 3;
	for (int verticesIndex = 0; verticesIndex < numVertsEachArrow; ++verticesIndex)
	{
		unsigned char currR = (unsigned char)(255.f * RRatio);
		unsigned char currG = (unsigned char)(255.f * GRatio);
		unsigned char currB = (unsigned char)(255.f * BRatio);
		unsigned char currA = (unsigned char)(255.f * ARatio);
		
		m_vertices[verticesIndex].m_color = Rgba8(currR, 0, 0, currA);
		m_vertices[verticesIndex + numVertsEachArrow].m_color = Rgba8(0, currG, 0, currA);;
		m_vertices[verticesIndex + 2 * numVertsEachArrow].m_color = Rgba8(0, 0, currB, currA);;
	}
}

class DebugRenderTextOrTexture : public DebugRenderObject
{
public:
	virtual void Update(float deltaSeconds) override;
public:
	Rgba8 m_startTint = Rgba8::WHITE;
	Rgba8 m_endTint = Rgba8::WHITE;
	Mat44 m_transMat;
	bool m_isBillboard = false;
	Texture* m_texture = nullptr;
};

void DebugRenderTextOrTexture::Update(float deltaSeconds)
{
	DebugRenderObject::Update(deltaSeconds);
	float RRatio = RangeMapFloat(m_totalSeconds, 0.f, (float)m_startTint.r, (float)m_endTint.r, m_remainSeconds) / 255.f;
	float GRatio = RangeMapFloat(m_totalSeconds, 0.f, (float)m_startTint.g, (float)m_endTint.g, m_remainSeconds) / 255.f;
	float BRatio = RangeMapFloat(m_totalSeconds, 0.f, (float)m_startTint.b, (float)m_endTint.b, m_remainSeconds) / 255.f;
	float ARatio = RangeMapFloat(m_totalSeconds, 0.f, (float)m_startTint.a, (float)m_endTint.a, m_remainSeconds) / 255.f;
	for (int verticesIndex = 0; verticesIndex < m_vertices.size(); ++verticesIndex)
	{
		unsigned char currR = (unsigned char)(255.f * RRatio);
		unsigned char currG = (unsigned char)(255.f * GRatio);
		unsigned char currB = (unsigned char)(255.f * BRatio);
		unsigned char currA = (unsigned char)(255.f * ARatio);

		m_vertices[verticesIndex].m_color = Rgba8(currR, currG, currB, currA);
	}
}

class DebugRenderSystem
{
public:
	RenderContext* m_context = nullptr;
	Camera* m_camera = nullptr;
	float m_screenHeight = 1080.f;
	AABB2 m_bound;
	std::vector<DebugRenderObject*> m_objects;
	std::vector<DebugRenderObject*> m_screenObjects;
	std::vector<DebugRenderObject*> m_screenObjectsTextures;
	bool m_isEnabled = true;
	// ... whatever you need
public:
	void Finalize(RenderContext* context) { m_context = context; }
	Camera* GetCamera() { return m_camera; }
	RenderContext* GetRenderContext() { return m_context; }
};

// static variables up top
static RenderContext* gDRContext = nullptr;
static Camera* gDebugCamera = nullptr;

static DebugRenderSystem* gDebugRenderSystem = nullptr;

//append object to list
void AppendObjectToVertexArray(std::vector<Vertex_PCU>& vertices, std::vector<unsigned int>& indices, DebugRenderObject* object)
{
	int startIndex = (int)vertices.size();
	for (int verticesIndex = 0; verticesIndex < object->m_vertices.size(); ++verticesIndex)
	{
		vertices.push_back(object->m_vertices[verticesIndex]);
	}
	for (int indicesIndex = 0; indicesIndex < object->m_indices.size(); ++indicesIndex)
	{
		indices.push_back(object->m_indices[indicesIndex] + startIndex);
	}
}

void DebugRenderSystemStartup(RenderContext* context)
{
	gDebugRenderSystem = new DebugRenderSystem();
	gDebugRenderSystem->Finalize(context);
	gDebugRenderSystem->m_camera = new Camera();
}

void DebugRenderSystemShutdown()
{
	ClearDebugRendering();
	delete gDebugRenderSystem->m_camera;
	gDebugRenderSystem->m_camera = nullptr;
	delete gDebugRenderSystem;
	gDebugRenderSystem = nullptr;
}

void EnableDebugRendering()
{
	gDebugRenderSystem->m_isEnabled = true;
}

void DisableDebugRendering()
{
	gDebugRenderSystem->m_isEnabled = false;
}

void ClearDebugRendering()
{
	for (int objectIndex = 0; objectIndex < gDebugRenderSystem->m_objects.size(); ++objectIndex)
	{
		DebugRenderObject* obj = gDebugRenderSystem->m_objects[objectIndex];
		delete obj;
	}
	gDebugRenderSystem->m_objects.clear();

	for (int objectIndex = 0; objectIndex < gDebugRenderSystem->m_screenObjects.size(); ++objectIndex)
	{
		DebugRenderObject* obj = gDebugRenderSystem->m_screenObjects[objectIndex];
		delete obj;
	}
	gDebugRenderSystem->m_screenObjects.clear();

	for (int objectIndex = 0; objectIndex < gDebugRenderSystem->m_screenObjectsTextures.size(); ++objectIndex)
	{
		DebugRenderObject* obj = gDebugRenderSystem->m_screenObjectsTextures[objectIndex];
		delete obj;
	}
	gDebugRenderSystem->m_screenObjectsTextures.clear();
}

void DebugRenderBeginFrame()
{}

void DebugRenderWorldToCamera(Camera* cam)
{
	if (!gDebugRenderSystem->m_isEnabled || gDebugRenderSystem->m_objects.size() == 0)
	{
		return;
	}
	// .. do other stuff
	Camera* debugCamera = gDebugRenderSystem->GetCamera();

	// copy all relevant settings from passed in camera
	debugCamera->m_transform = cam->m_transform;
	debugCamera->SetColorTarget(cam->m_colorTarget[0]);
	debugCamera->SetColorTarget(cam->m_colorTarget[1], 1);
	debugCamera->SetDefaultDepthBuffer(cam->m_depthBuffer);
	debugCamera->SetProjection(cam->GetProjection());
	debugCamera->SetClearMode(0, Rgba8(0, 0, 0, 255));
	RenderContext* debugRC = gDebugRenderSystem->GetRenderContext();

	std::vector<Vertex_PCU> verticesList[DEBUG_MODE_NUM];
	std::vector<unsigned int> indicesList[DEBUG_MODE_NUM];

// 	std::vector<Vertex_PCU> vertices;
// 	std::vector<unsigned int> indices;
	debugRC->BeginCamera(*debugCamera);
	for (int objectIndex = 0; objectIndex < gDebugRenderSystem->m_objects.size(); ++objectIndex)
	{
		DebugRenderObject* obj = gDebugRenderSystem->m_objects[objectIndex];
		if (obj->IsOld())
		{
			obj->MarkAsGarbage();
		}
		if (obj->m_renderMode == DEBUG_RENDER_ALWAYS)
		{
			if (obj->m_fillMode == DEBUG_FILL_NORMAL)
			{
				AppendObjectToVertexArray(verticesList[DEBUG_ALWAYS_NORMAL], indicesList[DEBUG_ALWAYS_NORMAL], obj);
			}
			if (obj->m_fillMode == DEBUG_FILL_TEXTURE)
			{
				DebugRenderTextOrTexture* textObj = (DebugRenderTextOrTexture*)obj;
				if (textObj->m_isBillboard)
				{
					Mat44 billboardTransMat = debugCamera->GetModel();
					Vec3 translation = textObj->m_transMat.GetTranslation3D();
					billboardTransMat.SetTranslation3D(translation);
					std::vector<Vertex_PCU> copyVertices = textObj->m_vertices;
					TransformVerticesWithMatrix(copyVertices, billboardTransMat);
					AddVerticeAndIndicesToIndexedVertexArray(verticesList[DEBUG_ALWAYS_TEXTURE], indicesList[DEBUG_ALWAYS_TEXTURE], copyVertices, textObj->m_indices);
				}
				else
				{
					AppendObjectToVertexArray(verticesList[DEBUG_ALWAYS_TEXTURE], indicesList[DEBUG_ALWAYS_TEXTURE], obj);
				}
			}
			if (obj->m_fillMode == DEBUG_FILL_WIREFRAME)
			{
				AppendObjectToVertexArray(verticesList[DEBUG_ALWAYS_WIREFRAME], indicesList[DEBUG_ALWAYS_WIREFRAME], obj);
			}
		}

		if (obj->m_renderMode == DEBUG_RENDER_USE_DEPTH)
		{
			if (obj->m_fillMode == DEBUG_FILL_NORMAL)
			{
				AppendObjectToVertexArray(verticesList[DEBUG_DEPTH_NORMAL], indicesList[DEBUG_DEPTH_NORMAL], obj);
			}
			if (obj->m_fillMode == DEBUG_FILL_TEXTURE)
			{
				DebugRenderTextOrTexture* textObj = (DebugRenderTextOrTexture*)obj;
				if (textObj->m_isBillboard)
				{
					Mat44 billboardTransMat = debugCamera->GetModel();
					Vec3 translation = textObj->m_transMat.GetTranslation3D();
					billboardTransMat.SetTranslation3D(translation);
					std::vector<Vertex_PCU> copyVertices = textObj->m_vertices;
					TransformVerticesWithMatrix(copyVertices, billboardTransMat);
					AddVerticeAndIndicesToIndexedVertexArray(verticesList[DEBUG_ALWAYS_TEXTURE], indicesList[DEBUG_ALWAYS_TEXTURE], copyVertices, textObj->m_indices);
				}
				else
				{
					AppendObjectToVertexArray(verticesList[DEBUG_DEPTH_TEXTURE], indicesList[DEBUG_DEPTH_TEXTURE], obj);
				}
			}
			if (obj->m_fillMode == DEBUG_FILL_WIREFRAME)
			{
				AppendObjectToVertexArray(verticesList[DEBUG_DEPTH_WIREFRAME], indicesList[DEBUG_DEPTH_WIREFRAME], obj);
			}
		}

		if (obj->m_renderMode == DEBUG_RENDER_XRAY)
		{
			if (obj->m_fillMode == DEBUG_FILL_NORMAL)
			{
				AppendObjectToVertexArray(verticesList[DEBUG_XRAY_NORMAL], indicesList[DEBUG_XRAY_NORMAL], obj);
			}
			if (obj->m_fillMode == DEBUG_FILL_TEXTURE)
			{
				DebugRenderTextOrTexture* textObj = (DebugRenderTextOrTexture*)obj;
				if (textObj->m_isBillboard)
				{
					Mat44 billboardTransMat = debugCamera->GetModel();
					Vec3 translation = textObj->m_transMat.GetTranslation3D();
					billboardTransMat.SetTranslation3D(translation);
					std::vector<Vertex_PCU> copyVertices = textObj->m_vertices;
					TransformVerticesWithMatrix(copyVertices, billboardTransMat);
					AddVerticeAndIndicesToIndexedVertexArray(verticesList[DEBUG_ALWAYS_TEXTURE], indicesList[DEBUG_ALWAYS_TEXTURE], copyVertices, textObj->m_indices);
				}
				else
				{
					AppendObjectToVertexArray(verticesList[DEBUG_XRAY_TEXTURE], indicesList[DEBUG_XRAY_TEXTURE], obj);
				}	
			}
			if (obj->m_fillMode == DEBUG_FILL_WIREFRAME)
			{
				AppendObjectToVertexArray(verticesList[DEBUG_XRAY_WIREFRAME], indicesList[DEBUG_XRAY_WIREFRAME], obj);
			}
		}
	}

	//0
	if (indicesList[DEBUG_ALWAYS_NORMAL].size() >= 3)
	{
		GPUMesh meshAN(debugRC);
		meshAN.UpdateIndices(indicesList[DEBUG_ALWAYS_NORMAL]);
		meshAN.UpdateVertices(verticesList[DEBUG_ALWAYS_NORMAL]);
		debugRC->BindTexture(debugRC->m_whiteTexture);
		debugRC->SetDepthTest(COMPARE_FUNC_ALWAYS, true);
		debugRC->DrawMesh(&meshAN);
	}
	
	//1
	if (indicesList[DEBUG_ALWAYS_TEXTURE].size() >= 3)
	{
		GPUMesh meshAT(debugRC);
		meshAT.UpdateIndices(indicesList[DEBUG_ALWAYS_TEXTURE]);
		meshAT.UpdateVertices(verticesList[DEBUG_ALWAYS_TEXTURE]);
		debugRC->BindTexture(g_theFont->GetTexture());
		debugRC->DrawMesh(&meshAT);
	}

	//2
	if (indicesList[DEBUG_ALWAYS_WIREFRAME].size() >= 3)
	{
		GPUMesh meshAW(debugRC);
		meshAW.UpdateIndices(indicesList[DEBUG_ALWAYS_WIREFRAME]);
		meshAW.UpdateVertices(verticesList[DEBUG_ALWAYS_WIREFRAME]);
		debugRC->BindTexture(debugRC->m_whiteTexture);
		debugRC->SetFillMode(WIREFRAME);
		debugRC->SetCullMode(NONE);
		debugRC->DrawMesh(&meshAW);
	}

	//3
	if (indicesList[DEBUG_DEPTH_NORMAL].size() >= 3)
	{
		GPUMesh meshDN(debugRC);
		meshDN.UpdateIndices(indicesList[DEBUG_DEPTH_NORMAL]);
		meshDN.UpdateVertices(verticesList[DEBUG_DEPTH_NORMAL]);
		debugRC->BindTexture(debugRC->m_whiteTexture);
		debugRC->SetFillMode(SOLID);
		debugRC->SetDepthTest(COMPARE_FUNC_LEQUAL, true);
		debugRC->SetCullMode(BACK);
		debugRC->DrawMesh(&meshDN);
	}

	//4
	if (indicesList[DEBUG_DEPTH_TEXTURE].size() >= 3)
	{
		GPUMesh meshDT(debugRC);
		meshDT.UpdateIndices(indicesList[DEBUG_DEPTH_TEXTURE]);
		meshDT.UpdateVertices(verticesList[DEBUG_DEPTH_TEXTURE]);
		debugRC->BindTexture(g_theFont->GetTexture());
		debugRC->DrawMesh(&meshDT);
	}

	//5
	if (indicesList[DEBUG_DEPTH_WIREFRAME].size() >= 3)
	{
		GPUMesh meshDW(debugRC);
		meshDW.UpdateIndices(indicesList[DEBUG_DEPTH_WIREFRAME]);
		meshDW.UpdateVertices(verticesList[DEBUG_DEPTH_WIREFRAME]);
		debugRC->BindTexture(debugRC->m_whiteTexture);
		debugRC->SetFillMode(WIREFRAME);
		debugRC->SetCullMode(NONE);
		debugRC->DrawMesh(&meshDW);
	}
	
	//6
	if (indicesList[DEBUG_XRAY_NORMAL].size() >= 3)
	{
		GPUMesh meshXN(debugRC);
		meshXN.UpdateIndices(indicesList[DEBUG_XRAY_NORMAL]);
		meshXN.UpdateVertices(verticesList[DEBUG_XRAY_NORMAL]);
		debugRC->BindTexture(debugRC->m_whiteTexture);
		debugRC->SetCullMode(BACK);
		debugRC->SetFillMode(SOLID);
		
		debugRC->SetShaderTint(Vec4(0.5f, 0.5f, 0.5f, 1.f));
		debugRC->SetDepthTest(COMPARE_FUNC_GEQUAL, false);
		debugRC->DrawMesh(&meshXN);

		debugRC->SetShaderTint(Vec4(1.f, 1.f, 1.f, 1.f));
		debugRC->SetDepthTest(COMPARE_FUNC_LEQUAL, true);
		debugRC->DrawMesh(&meshXN);
	}

	//7
	if (indicesList[DEBUG_XRAY_TEXTURE].size() >= 3)
	{
		GPUMesh meshXT(debugRC);
		meshXT.UpdateIndices(indicesList[DEBUG_XRAY_TEXTURE]);
		meshXT.UpdateVertices(verticesList[DEBUG_XRAY_TEXTURE]);
		debugRC->BindTexture(g_theFont->GetTexture());

		debugRC->SetShaderTint(Vec4(0.5f, 0.5f, 0.5f, 1.f));
		debugRC->SetDepthTest(COMPARE_FUNC_GEQUAL, false);
		debugRC->DrawMesh(&meshXT);

		debugRC->SetShaderTint(Vec4(1.f, 1.f, 1.f, 1.f));
		debugRC->SetDepthTest(COMPARE_FUNC_LEQUAL, true);
		debugRC->DrawMesh(&meshXT);
	}

	//8
	if (indicesList[DEBUG_XRAY_WIREFRAME].size() >= 3)
	{
		GPUMesh meshXW(debugRC);
		meshXW.UpdateIndices(indicesList[DEBUG_XRAY_WIREFRAME]);
		meshXW.UpdateVertices(verticesList[DEBUG_XRAY_WIREFRAME]);
		debugRC->BindTexture(debugRC->m_whiteTexture);
		debugRC->SetFillMode(WIREFRAME);
		debugRC->SetCullMode(NONE);

		debugRC->SetShaderTint(Vec4(0.5f, 0.5f, 0.5f, 1.f));
		debugRC->SetDepthTest(COMPARE_FUNC_GEQUAL, false);
		debugRC->DrawMesh(&meshXW);

		debugRC->SetShaderTint(Vec4(1.f, 1.f, 1.f, 1.f));
		debugRC->SetDepthTest(COMPARE_FUNC_LEQUAL, true);
		debugRC->DrawMesh(&meshXW);
	}

	debugRC->EndCamera(*debugCamera);
}


void DebugRenderScreenTo(Texture* tex)
{
	if (!gDebugRenderSystem->m_isEnabled)
	{
		return;
	}

	RenderContext* debugRC = gDebugRenderSystem->GetRenderContext();

	Camera camera;
	camera.SetCameraMode(CAMERA_MODE_ORTHOGRAPHIC);
	camera.SetColorTarget(tex);
	IntVec2 max = tex->GetTextureSize();
	camera.SetOutputSize(Vec2((float)max.x, (float)max.y));
	camera.SetProjectionOrthographic(gDebugRenderSystem->m_screenHeight, -100.f, 100.f);
	gDebugRenderSystem->m_bound = AABB2(Vec2::ZERO, camera.m_outputSize);

	camera.SetClearMode(0, Rgba8(0, 0, 0, 255));
	debugRC->BeginCamera(camera);
	GPUMesh screenMesh(debugRC);
	std::vector<Vertex_PCU> vertices;
	std::vector<unsigned int> indices;

	GPUMesh screenMeshText(debugRC);
	std::vector<Vertex_PCU> verticesText;
	std::vector<unsigned int> indicesText;


	for (int objectIndex = 0; objectIndex < gDebugRenderSystem->m_screenObjects.size(); ++objectIndex)
	{
		DebugRenderObject* obj = gDebugRenderSystem->m_screenObjects[objectIndex];
		if (obj->m_fillMode == DEBUG_FILL_TEXTURE)
		{
			AppendObjectToVertexArray(verticesText, indicesText, obj);
		}
		else
		{
			AppendObjectToVertexArray(vertices, indices, obj);
		}
		if (obj->IsOld())
		{
			obj->MarkAsGarbage();
		}
	}
	//gDRContext->DrawVertexArray(vertices.size(), vertices.data());
	if (vertices.size() > 0)
	{
		screenMesh.UpdateVertices(vertices);
		screenMesh.UpdateIndices(indices);
		debugRC->BindTexture(debugRC->m_whiteTexture);
		debugRC->DrawMesh(&screenMesh);
	}
	if (verticesText.size() > 0)
	{
		screenMeshText.UpdateVertices(verticesText);
		screenMeshText.UpdateIndices(indicesText);
		debugRC->BindTexture(g_theFont->GetTexture());
		debugRC->DrawMesh(&screenMeshText);
	}

	for (int objectIndex = 0; objectIndex < gDebugRenderSystem->m_screenObjectsTextures.size(); ++objectIndex)
	{
		DebugRenderTextOrTexture* obj = (DebugRenderTextOrTexture*)gDebugRenderSystem->m_screenObjectsTextures[objectIndex];
		GPUMesh textureMesh(debugRC);
		textureMesh.UpdateIndices(obj->m_indices);
		textureMesh.UpdateVertices(obj->m_vertices);
		debugRC->BindTexture(obj->m_texture);
		debugRC->DrawMesh(&textureMesh);
		if (obj->IsOld())
		{
			obj->MarkAsGarbage();
		}
	}
	debugRC->EndCamera(camera);
}

void DebugAddWorldPoint(Vec3 pos, float size, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode)
{
	DebugRenderPointOrQuad* point = new DebugRenderPointOrQuad();
	point->m_startColor = start_color;
	point->m_endColor = end_color;
	point->m_totalSeconds = duration;
	point->m_remainSeconds = duration;
	point->m_renderMode = mode;
	Vec3 mins = pos - Vec3(size * .5f, size * .5f, size * .5f);
	Vec3 maxs = pos + Vec3(size * .5f, size * .5f, size * .5f);
	AddAABB3ToIndexedVertexArray(point->m_vertices, point->m_indices, AABB3(mins, maxs), start_color);

	gDebugRenderSystem->m_objects.push_back(point);
}

void DebugAddWorldPoint(Vec3 pos, float size, Rgba8 color, float duration, eDebugRenderMode mode)
{
	DebugAddWorldPoint(pos, size, color, color, duration, mode);
}

void DebugAddWorldPoint(Vec3 pos, Rgba8 color, float duration, eDebugRenderMode mode)
{
	DebugAddWorldPoint(pos, .1f, color, duration, mode);
}

void DebugAddWorldLine(Vec3 p0, Rgba8 p0_start_color, Rgba8 p0_end_color,
	Vec3 p1, Rgba8 p1_start_color, Rgba8 p1_end_color,
	float duration,
	eDebugRenderMode mode)
{
	DebugRenderLineOrArrow* line = new DebugRenderLineOrArrow();
	line->m_p0StartColor = p0_start_color;
	line->m_p0EndColor = p0_end_color;
	line->m_p1StartColor = p1_start_color;
	line->m_p1EndColor = p1_end_color;
	line->m_totalSeconds = duration;
	line->m_remainSeconds = duration;
	AddCylinderToIndexedVertexArray(line->m_vertices, line->m_indices, p0, p1, 0.05f, p0_start_color, p1_start_color);
	line->m_renderMode = mode;
	gDebugRenderSystem->m_objects.push_back(line);
}

void DebugAddWorldLine(Vec3 start, Vec3 end, Rgba8 color, float duration, eDebugRenderMode mode)
{
	DebugAddWorldLine(start, color, color, end, color, color, duration, mode);
}

void DebugAddWorldArrow(Vec3 p0, Rgba8 p0_start_color, Rgba8 p0_end_color,
	Vec3 p1, Rgba8 p1_start_color, Rgba8 p1_end_color,
	float duration,
	eDebugRenderMode mode)
{
	Vec3 mid = p0 + (p1 - p0) * 0.9f;
	DebugRenderLineOrArrow* arrow = new DebugRenderLineOrArrow();
	arrow->m_p0StartColor = p0_start_color;
	arrow->m_p0EndColor = p0_end_color;
	arrow->m_p1StartColor = p1_start_color;
	arrow->m_p1EndColor = p1_end_color;
	arrow->m_totalSeconds = duration;
	arrow->m_remainSeconds = duration;
	AddCylinderToIndexedVertexArray(arrow->m_vertices, arrow->m_indices, p0, mid, 1.f, p0_start_color, p1_start_color);
	AddConeToIndexedVertexArray(arrow->m_vertices, arrow->m_indices, mid, p1, 2.f, p1_start_color);
	arrow->m_renderMode = mode;
	gDebugRenderSystem->m_objects.push_back(arrow);
}

void DebugAddWorldArrow(Vec3 start, Vec3 end, Rgba8 color, float duration, eDebugRenderMode mode)
{
	DebugAddWorldArrow(start, color, color, end, color, color, duration, mode);
}

void DebugAddWorldQuad(Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode)
{
	DebugRenderPointOrQuad* quad = new DebugRenderPointOrQuad();
	quad->m_startColor = start_color;
	quad->m_endColor = end_color;
	quad->m_totalSeconds = duration;
	quad->m_remainSeconds = duration;
	AddQuadToIndexedVertexArray(quad->m_vertices, quad->m_indices,p0, p1, p2, p3, start_color);
	quad->m_renderMode = mode;
	gDebugRenderSystem->m_objects.push_back(quad);
}

void DebugAddWorldWireBounds(AABB3 bounds, Mat44 TransformMat, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode)
{
	DebugRenderPointOrQuad* cube = new DebugRenderPointOrQuad();
	cube->m_startColor = start_color;
	cube->m_endColor = end_color;
	cube->m_totalSeconds = duration;
	cube->m_remainSeconds = duration;
	AddAABB3ToIndexedVertexArray(cube->m_vertices, cube->m_indices, bounds, start_color);
	cube->m_renderMode = mode;
	cube->m_fillMode = DEBUG_FILL_WIREFRAME;
	TransformVerticesWithMatrix(cube->m_vertices, TransformMat);
	gDebugRenderSystem->m_objects.push_back(cube);
}

void DebugAddWorldWireBounds(AABB3 bounds, Mat44 TransformMat, Rgba8 color, float duration, eDebugRenderMode mode)
{
	DebugAddWorldWireBounds(bounds, TransformMat, color, color, duration, mode);
}

void DebugAddWorldWireSphere(Vec3 pos, float radius, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode)
{
	DebugRenderPointOrQuad* sphere = new DebugRenderPointOrQuad();
	sphere->m_startColor = start_color;
	sphere->m_endColor = end_color;
	sphere->m_totalSeconds = duration;
	sphere->m_remainSeconds = duration;
	AddUVSphereToIndexedVertexArray(sphere->m_vertices, sphere->m_indices, pos, radius, 24, 12, start_color);
	sphere->m_renderMode = mode;
	sphere->m_fillMode = DEBUG_FILL_WIREFRAME;
	gDebugRenderSystem->m_objects.push_back(sphere);
}
void DebugAddWorldWireSphere(Vec3 pos, float radius, Rgba8 color, float duration, eDebugRenderMode mode)
{
	DebugAddWorldWireSphere(pos, radius, color, color, duration, mode);
}

void DebugAddWorldBasis(Mat44 basis, Rgba8 start_tint, Rgba8 end_tint, float duration, eDebugRenderMode mode)
{
	DebugRenderBasis* basisObj = new DebugRenderBasis();
	basisObj->m_startTint = start_tint;
	basisObj->m_endTint = end_tint;
	basisObj->m_totalSeconds = duration;
	basisObj->m_remainSeconds = duration;
	basisObj->m_renderMode = mode;
	Vec3 origin = basis.GetTranslation3D();
	Vec3 iBasis = basis.GetIBasis3D();
	Vec3 jBasis = basis.GetJBasis3D();
	Vec3 kBasis = basis.GetKBasis3D();
	Vec3 iMid = origin + iBasis * 0.9f;
	Vec3 jMid = origin + jBasis * 0.9f;
	Vec3 kMid = origin + kBasis * 0.9f;

	AddCylinderToIndexedVertexArray(basisObj->m_vertices, basisObj->m_indices, origin, iMid, 0.05f, Rgba8(255, 0, 0, 255), Rgba8(255, 0, 0, 255));
	AddConeToIndexedVertexArray(basisObj->m_vertices, basisObj->m_indices, iMid, iBasis + origin, 0.1f, Rgba8(255, 0, 0, 255));

	AddCylinderToIndexedVertexArray(basisObj->m_vertices, basisObj->m_indices, origin, jMid, 0.05f, Rgba8(0, 255, 0, 255), Rgba8(0, 255, 0, 255));
	AddConeToIndexedVertexArray(basisObj->m_vertices, basisObj->m_indices, jMid, jBasis + origin, 0.1f, Rgba8(0, 255, 0, 255));

	AddCylinderToIndexedVertexArray(basisObj->m_vertices, basisObj->m_indices, origin, kMid, 0.05f, Rgba8(0, 0, 255, 255), Rgba8(0, 0, 255, 255));
	AddConeToIndexedVertexArray(basisObj->m_vertices, basisObj->m_indices, kMid, kBasis + origin, 0.1f, Rgba8(0, 0, 255, 255));
	basisObj->m_renderMode = mode;
	gDebugRenderSystem->m_objects.push_back(basisObj);
}

void DebugAddWorldBasis(Mat44 basis, float duration, eDebugRenderMode mode)
{
	DebugAddWorldBasis(basis, Rgba8::WHITE, Rgba8(0, 0, 0,255), duration, mode);
}

void DebugAddWorldText(Mat44 basis, Vec2 pivot, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode, char const* text)
{
	DebugRenderTextOrTexture* textObj = new DebugRenderTextOrTexture();
	textObj->m_startTint = start_color;
	textObj->m_endTint = end_color;
	textObj->m_totalSeconds = duration;
	textObj->m_remainSeconds = duration;
	textObj->m_renderMode = mode;
	textObj->m_fillMode = DEBUG_FILL_TEXTURE;
	textObj->m_transMat = basis;
	float height = 1.f;
	std::string textString(text);
	float width = (float)textString.size();
	Vec2 textMins = Vec2(-pivot.x * width, -pivot.y * height);
	g_theFont->AddVertsForText2D(textObj->m_vertices, textMins, 1.f, textString, start_color, 1.f);
	for (int indicesIndex = 0; indicesIndex < textObj->m_vertices.size(); ++indicesIndex)//just for safety, will change in the future
	{
		textObj->m_indices.push_back((unsigned int)indicesIndex);
	}
	TransformVerticesWithMatrix(textObj->m_vertices, basis);
	gDebugRenderSystem->m_objects.push_back(textObj);
}

void DebugAddWorldTextf(Mat44 basis, Vec2 pivot, Rgba8 color, float duration, eDebugRenderMode mode, char const* text, ...)
{
	DebugRenderTextOrTexture* textObj = new DebugRenderTextOrTexture();
	textObj->m_startTint = color;
	textObj->m_endTint = color;
	textObj->m_totalSeconds = duration;
	textObj->m_remainSeconds = duration;
	textObj->m_renderMode = mode;
	textObj->m_fillMode = DEBUG_FILL_TEXTURE;
	textObj->m_transMat = basis;
	float height = 1.f;
	va_list args;
	va_start(args, text);
	std::string textString = Stringv(text, args);
	float width = (float)textString.size();
	Vec2 textMins = Vec2(-pivot.x * width, -pivot.y * height);
	g_theFont->AddVertsForText2D(textObj->m_vertices, textMins, 1.f, textString, color, 1.f);
	for (int indicesIndex = 0; indicesIndex < textObj->m_vertices.size(); ++indicesIndex)//just for safety, will change in the future
	{
		textObj->m_indices.push_back((unsigned int)indicesIndex);
	}
	TransformVerticesWithMatrix(textObj->m_vertices, basis);
	gDebugRenderSystem->m_objects.push_back(textObj);
}

void DebugAddWorldTextf(Mat44 basis, Vec2 pivot, Rgba8 color, char const* text, ...)
{
	va_list args;
	va_start(args, text);
	std::string textString = Stringv(text, args);
	DebugAddWorldTextf(basis, pivot, color, 100000.f, DEBUG_RENDER_USE_DEPTH, textString.c_str());
}

void DebugAddWorldBillboardText(Vec3 origin, Vec2 pivot, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode, char const* text)
{
	DebugRenderTextOrTexture* textObj = new DebugRenderTextOrTexture();
	textObj->m_startTint = start_color;
	textObj->m_endTint = end_color;
	textObj->m_totalSeconds = duration;
	textObj->m_remainSeconds = duration;
	textObj->m_renderMode = mode;
	textObj->m_fillMode = DEBUG_FILL_TEXTURE;
	textObj->m_transMat = Mat44::CreateTranslation3D(origin);
	textObj->m_isBillboard = true;
	float height = 1.f;
	std::string textString(text);
	float width = (float)textString.size();
	Vec2 textMins = Vec2(-pivot.x * width, -pivot.y * height);
	g_theFont->AddVertsForText2D(textObj->m_vertices, textMins, 1.f, textString, start_color, 1.f);
	for (int indicesIndex = 0; indicesIndex < textObj->m_vertices.size(); ++indicesIndex)//just for safety, will change in the future
	{
		textObj->m_indices.push_back((unsigned int)indicesIndex);
	}
	gDebugRenderSystem->m_objects.push_back(textObj);
}

void DebugAddWorldBillboardTextf(Vec3 origin, Vec2 pivot, Rgba8 color, float duration, eDebugRenderMode mode, char const* format, ...)
{
	DebugRenderTextOrTexture* textObj = new DebugRenderTextOrTexture();
	textObj->m_startTint = color;
	textObj->m_endTint = color;
	textObj->m_totalSeconds = duration;
	textObj->m_remainSeconds = duration;
	textObj->m_renderMode = mode;
	textObj->m_fillMode = DEBUG_FILL_TEXTURE;
	textObj->m_transMat = Mat44::CreateTranslation3D(origin);
	textObj->m_isBillboard = true;
	float height = 1.f;
	va_list args;
	va_start(args, format);
	std::string textString = Stringv(format, args);
	float width = (float)textString.size();
	Vec2 textMins = Vec2(-pivot.x * width, -pivot.y * height);
	g_theFont->AddVertsForText2D(textObj->m_vertices, textMins, 1.f, textString, color, 1.f);
	for (int indicesIndex = 0; indicesIndex < textObj->m_vertices.size(); ++indicesIndex)//just for safety, will change in the future
	{
		textObj->m_indices.push_back((unsigned int)indicesIndex);
	}
	gDebugRenderSystem->m_objects.push_back(textObj);
}

void DebugAddWorldBillboardTextf(Vec3 origin, Vec2 pivot, Rgba8 color, char const* format, ...)
{
	va_list args;
	va_start(args, format);
	std::string textString = Stringv(format, args);
	DebugAddWorldBillboardText(origin, pivot, color, color, 100000.f, DEBUG_RENDER_USE_DEPTH, textString.c_str());
}

void DebugRenderSetScreenHeight(float height)
{
	gDebugRenderSystem->m_screenHeight = height;
}

AABB2 DebugGetScreenBounds()
{
	float cameraAspect = gDebugRenderSystem->GetCamera()->GetAspectRatio();
	float height = gDebugRenderSystem->m_screenHeight;
	return AABB2(0.f, 0.f, height * cameraAspect, height);
}

void DebugAddScreenPoint(Vec2 pos, float size, Rgba8 start_color, Rgba8 end_color, float duration)
{
	Vec2 mins = pos - Vec2(0.5f * size, 0.5f * size);
	Vec2 maxs = pos + Vec2(0.5f * size, 0.5f * size);
	DebugAddScreenQuad(AABB2(mins, maxs), start_color, end_color, duration);
}

void DebugAddScreenPoint(Vec2 pos, float size, Rgba8 color, float duration)
{
	DebugAddScreenPoint(pos, size, color, color, duration);
}

void DebugAddScreenPoint(Vec2 pos, Rgba8 color)
{
	DebugAddScreenPoint(pos, 10.f, color, 100000.f);
}

void DebugAddScreenLine(Vec2 p0, Rgba8 p0_start_color, Rgba8 p0_end_color,
	Vec2 p1, Rgba8 p1_start_color, Rgba8 p1_end_color,
	float duration)
{
	DebugRenderLineOrArrow* line = new DebugRenderLineOrArrow();
	line->m_p0StartColor = p0_start_color;
	line->m_p0EndColor = p0_end_color;
	line->m_p1StartColor = p1_start_color;
	line->m_p1EndColor = p1_end_color;
	line->m_totalSeconds = duration;
	line->m_remainSeconds = duration;
	AABB2 screenBound = DebugGetScreenBounds();
	p0 -= screenBound.maxs * 0.5f;
	p1 -= screenBound.maxs * 0.5f;
	AddCylinderToIndexedVertexArray(line->m_vertices, line->m_indices, Vec3(p0, 0.f), Vec3(p1, 0.f), 1.f, p0_start_color, p1_start_color);
	gDebugRenderSystem->m_screenObjects.push_back(line);
}

void DebugAddScreenLine(Vec2 p0, Vec2 p1, Rgba8 color, float duration)
{
	DebugAddScreenLine(p0, color, color, p1, color, color, duration);
}

void DebugAddScreenArrow(Vec2 p0, Rgba8 p0_start_color, Rgba8 p0_end_color,
	Vec2 p1, Rgba8 p1_start_color, Rgba8 p1_end_color,
	float duration)
{
	AABB2 screenBound = DebugGetScreenBounds();
	p0 -= screenBound.maxs * 0.5f;
	p1 -= screenBound.maxs * 0.5f;
	Vec2 mid = p0 + (p1 - p0) * 0.9f;
	DebugRenderLineOrArrow* arrow = new DebugRenderLineOrArrow();
	arrow->m_p0StartColor = p0_start_color;
	arrow->m_p0EndColor = p0_end_color;
	arrow->m_p1StartColor = p1_start_color;
	arrow->m_p1EndColor = p1_end_color;
	arrow->m_totalSeconds = duration;
	arrow->m_remainSeconds = duration;
	AddCylinderToIndexedVertexArray(arrow->m_vertices, arrow->m_indices, Vec3(p0, 0.f), Vec3(mid, 0.f), 10.f, p0_start_color, p1_start_color);
	AddConeToIndexedVertexArray(arrow->m_vertices, arrow->m_indices, Vec3(mid, 0.f), Vec3(p1, 0.f), 20.f, p1_start_color);
	gDebugRenderSystem->m_screenObjects.push_back(arrow);
}

void DebugAddScreenArrow(Vec2 p0, Vec2 p1, Rgba8 color, float duration)
{
	DebugAddScreenArrow(p0, color, color, p1, color, color, duration);
}

void DebugAddScreenQuad(AABB2 bounds, Rgba8 start_color, Rgba8 end_color, float duration)
{
	DebugRenderPointOrQuad* quad = new DebugRenderPointOrQuad();
	quad->m_startColor = start_color;
	quad->m_endColor = end_color;
	quad->m_totalSeconds = duration;
	quad->m_remainSeconds = duration;
	AABB2 screenBound = DebugGetScreenBounds();
	bounds.maxs -= screenBound.maxs * 0.5f;
	bounds.mins -= screenBound.maxs * 0.5f;
	AddAABB2ToIndexedVertexArray(quad->m_vertices, quad->m_indices, bounds, 0.f, start_color);
	gDebugRenderSystem->m_screenObjects.push_back(quad);
}

void DebugAddScreenQuad(AABB2 bounds, Rgba8 color, float duration)
{
	DebugAddScreenQuad(bounds, color, color, duration);
}

void DebugAddScreenTexturedQuad(AABB2 bounds, Texture* tex, AABB2 uvs, Rgba8 start_tint, Rgba8 end_tint, float duration)
{
	DebugRenderTextOrTexture* quad = new DebugRenderTextOrTexture();
	quad->m_startTint = start_tint;
	quad->m_endTint = end_tint;
	quad->m_totalSeconds = duration;
	quad->m_remainSeconds = duration;
	quad->m_texture = tex;
	AABB2 screenBound = DebugGetScreenBounds();
	bounds.maxs -= screenBound.maxs * 0.5f;
	bounds.mins -= screenBound.maxs * 0.5f;
	AddAABB2ToIndexedVertexArray(quad->m_vertices, quad->m_indices, bounds, uvs, 0.f, Rgba8::WHITE);
	gDebugRenderSystem->m_screenObjectsTextures.push_back(quad);
}

void DebugAddScreenTexturedQuad(AABB2 bounds, Texture* tex, AABB2 uvs, Rgba8 tint, float duration)
{
	DebugAddScreenTexturedQuad(bounds, tex, uvs, tint, tint, duration);
}

void DebugAddScreenTexturedQuad(AABB2 bounds, Texture* tex, Rgba8 tint, float duration)
{
	DebugAddScreenTexturedQuad(bounds, tex, AABB2(0.f, 0.f, 1.f, 1.f), tint, duration);
}

void DebugAddScreenText(Vec4 pos, Vec2 pivot, float size, Rgba8 start_color, Rgba8 end_color, float duration, char const* text)
{
	DebugRenderTextOrTexture* textObj = new DebugRenderTextOrTexture();
	textObj->m_startTint = start_color;
	textObj->m_endTint = end_color;
	textObj->m_totalSeconds = duration;
	textObj->m_remainSeconds = duration;
	textObj->m_fillMode = DEBUG_FILL_TEXTURE;
	float height = size;
	std::string textString(text);
	float width = (float)textString.size() * size;
	Vec2 textMins = Vec2(-pivot.x * width, -pivot.y * height);
	g_theFont->AddVertsForText2D(textObj->m_vertices, textMins, size, textString, start_color, 1.f);
	for (int indicesIndex = 0; indicesIndex < textObj->m_vertices.size(); ++indicesIndex)//just for safety, will change in the future
	{
		textObj->m_indices.push_back((unsigned int)indicesIndex);
	}

	AABB2 screenBound = DebugGetScreenBounds();
	Vec2 uvPosition = Vec2(pos.x, pos.y) - Vec2(0.5f, 0.5f);
	Vec2 absolutePosition = Vec2(pos.z, pos.w);
	float uvXToX = RangeMapFloat(0.f, 1.f, screenBound.mins.x, screenBound.maxs.x, uvPosition.x);
	float uvYToY = RangeMapFloat(0.f, 1.f, screenBound.mins.y, screenBound.maxs.y, uvPosition.y);
	Vec2 xyTranslation = Vec2(uvXToX, uvYToY) + absolutePosition;
	TransformVertexArray((int)textObj->m_vertices.size(), textObj->m_vertices.data(), 1.f, 0.f, xyTranslation);
	gDebugRenderSystem->m_screenObjects.push_back(textObj);
}

void DebugAddScreenTextf(Vec4 pos, Vec2 pivot, float size, Rgba8 start_color, Rgba8 end_color, float duration, char const* format, ...)
{
	DebugRenderTextOrTexture* textObj = new DebugRenderTextOrTexture();
	textObj->m_startTint = start_color;
	textObj->m_endTint = end_color;
	textObj->m_totalSeconds = duration;
	textObj->m_remainSeconds = duration;
	textObj->m_fillMode = DEBUG_FILL_TEXTURE;
	float height = size;
	va_list args;
	va_start(args, format);
	std::string textString = Stringv(format, args);
	float width = (float)textString.size() * size;
	Vec2 textMins = Vec2(-pivot.x * width, -pivot.y * height);
	g_theFont->AddVertsForText2D(textObj->m_vertices, textMins, size, textString, start_color, 1.f);
	for (int indicesIndex = 0; indicesIndex < textObj->m_vertices.size(); ++indicesIndex)//just for safety, will change in the future
	{
		textObj->m_indices.push_back((unsigned int)indicesIndex);
	}

	AABB2 screenBound = DebugGetScreenBounds();
	Vec2 uvPosition = Vec2(pos.x, pos.y) - Vec2(0.5f, 0.5f);
	Vec2 absolutePosition = Vec2(pos.z, pos.w);
	float uvXToX = RangeMapFloat(0.f, 1.f, screenBound.mins.x, screenBound.maxs.x, uvPosition.x);
	float uvYToY = RangeMapFloat(0.f, 1.f, screenBound.mins.y, screenBound.maxs.y, uvPosition.y);
	Vec2 xyTranslation = Vec2(uvXToX, uvYToY) + absolutePosition;
	TransformVertexArray((int)textObj->m_vertices.size(), textObj->m_vertices.data(), 1.f, 0.f, xyTranslation);
	gDebugRenderSystem->m_screenObjects.push_back(textObj);
}

void DebugAddScreenTextf(Vec4 pos, Vec2 pivot, float size, Rgba8 color, float duration, char const* format, ...)
{
	va_list args;
	va_start(args, format);
	std::string textString = Stringv(format, args);
	DebugAddScreenText(pos, pivot, size, color, color, duration, textString.c_str());
}

void DebugAddScreenTextf(Vec4 pos, Vec2 pivot, float size, Rgba8 color, char const* format, ...)
{
	va_list args;
	va_start(args, format);
	std::string textString = Stringv(format, args);
	DebugAddScreenText(pos, pivot, size, color, color, 100000.f, textString.c_str());
}

void DebugAddScreenTextf(Vec4 pos, Vec2 pivot, Rgba8 color, char const* format, ...)
{
	va_list args;
	va_start(args, format);
	std::string textString = Stringv(format, args);
	DebugAddScreenText(pos, pivot, 100.f, color, color, 100000.f, textString.c_str());
}

void DebugAddScreenBasis(Vec2 screen_origin_location, Mat44 basis_to_render, Rgba8 start_tint, Rgba8 end_tint, float duration)
{
	DebugRenderBasis* basisObj = new DebugRenderBasis();
	basisObj->m_startTint = start_tint;
	basisObj->m_endTint = end_tint;
	basisObj->m_totalSeconds = duration;
	basisObj->m_remainSeconds = duration;
	Vec3 origin = Vec3(screen_origin_location, 0.f);
	Vec3 iBasis = basis_to_render.GetIBasis3D() * 100.f;
	Vec3 jBasis = basis_to_render.GetJBasis3D() * 100.f;
	Vec3 kBasis = basis_to_render.GetKBasis3D() * 100.f;
	Vec3 iMid = origin + iBasis * 0.9f;
	Vec3 jMid = origin + jBasis * 0.9f;
	Vec3 kMid = origin + kBasis * 0.9f;

	AddCylinderToIndexedVertexArray(basisObj->m_vertices, basisObj->m_indices, origin, iMid, 10.f, Rgba8(255, 0, 0, 255), Rgba8(255, 0, 0, 255));
	AddConeToIndexedVertexArray(basisObj->m_vertices, basisObj->m_indices, iMid, iBasis + origin, 20.f, Rgba8(255, 0, 0, 255));

	AddCylinderToIndexedVertexArray(basisObj->m_vertices, basisObj->m_indices, origin, jMid, 10.f, Rgba8(0, 255, 0, 255), Rgba8(0, 255, 0, 255));
	AddConeToIndexedVertexArray(basisObj->m_vertices, basisObj->m_indices, jMid, jBasis + origin, 20.f, Rgba8(0, 255, 0, 255));

	AddCylinderToIndexedVertexArray(basisObj->m_vertices, basisObj->m_indices, origin, kMid, 10.f, Rgba8(0, 0, 255, 255), Rgba8(0, 0, 255, 255));
	AddConeToIndexedVertexArray(basisObj->m_vertices, basisObj->m_indices, kMid, kBasis + origin, 20.f, Rgba8(0, 0, 255, 255));

	gDebugRenderSystem->m_screenObjects.push_back(basisObj);
}

void DebugAddScreenBasis(Vec2 screen_origin_location, Mat44 basis_to_render, Rgba8 tint, float duration)
{
	DebugAddScreenBasis(screen_origin_location, basis_to_render, tint, tint, duration);
}

void DebugRenderUpdate()
{
	Clock* clock = gDebugRenderSystem->m_context->m_clock;
	float deltaSeconds = (float)clock->GetLastDeltaSeconds();
	for (int objectIndex = 0; objectIndex < gDebugRenderSystem->m_objects.size(); ++objectIndex)
	{
		gDebugRenderSystem->m_objects[objectIndex]->Update(deltaSeconds);
	}

	for (int objectIndex = 0; objectIndex < gDebugRenderSystem->m_screenObjects.size(); ++objectIndex)
	{
		gDebugRenderSystem->m_screenObjects[objectIndex]->Update(deltaSeconds);
	}

	for (int objectIndex = 0; objectIndex < gDebugRenderSystem->m_screenObjectsTextures.size(); ++objectIndex)
	{
		gDebugRenderSystem->m_screenObjectsTextures[objectIndex]->Update(deltaSeconds);
	}
}

void DebugRenderEndFrame()
{
	// for each object, if ready to culled, remove and delete
	for (int objectIndex = 0; objectIndex < gDebugRenderSystem->m_objects.size(); ++objectIndex)
	{
		DebugRenderObject* obj = gDebugRenderSystem->m_objects[objectIndex];
		if (obj->IsReadyToBeCulled())
		{
			delete obj;
			gDebugRenderSystem->m_objects.erase(gDebugRenderSystem->m_objects.begin() + objectIndex);
			objectIndex--;
		}
	}

	for (int objectIndex = 0; objectIndex < gDebugRenderSystem->m_screenObjects.size(); ++objectIndex)
	{
		DebugRenderObject* obj = gDebugRenderSystem->m_screenObjects[objectIndex];
		if (obj->IsReadyToBeCulled())
		{
			delete obj;
			gDebugRenderSystem->m_screenObjects.erase(gDebugRenderSystem->m_screenObjects.begin() + objectIndex);
			objectIndex--;
		}
	}

	for (int objectIndex = 0; objectIndex < gDebugRenderSystem->m_screenObjectsTextures.size(); ++objectIndex)
	{
		DebugRenderObject* obj = gDebugRenderSystem->m_screenObjectsTextures[objectIndex];
		if (obj->IsReadyToBeCulled())
		{
			delete obj;
			gDebugRenderSystem->m_screenObjectsTextures.erase(gDebugRenderSystem->m_screenObjectsTextures.begin() + objectIndex);
			objectIndex--;
		}
	}
}

bool DebugRenderObject::IsOld() const
{
	return m_remainSeconds <= 0.f;
}