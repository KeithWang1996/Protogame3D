#pragma once
#include <vector>
#include <string>
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/EngineCommon.hpp"
class Texture;
struct AABB2;
class BitmapFont
{
	friend class RenderContext; // Only the RenderContext can create new BitmapFont objects!

private:
	BitmapFont(const char* fontName, const Texture* fontTexture);

public:
	const Texture* GetTexture() const;

	void AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, const Vec2& textMins,
		float cellHeight, const std::string& text, const Rgba8& tint = Rgba8::WHITE, float cellAspect = 1.f);
	void AddVertsForTextSquare2D(std::vector<Vertex_PCU>& vertexArray, const Vec2& squareMins, float squareHeight, const std::string& text, IntVec2 layout, const Rgba8& tint = Rgba8::WHITE);
	void AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, const AABB2& box, float cellHeight,
		const std::string& text, const Rgba8& tint = Rgba8::WHITE, float cellAspect = 1.f,
		const Vec2 & alignment = ALIGN_CENTERED);
	Vec2 GetDimensionsForText2D(float cellHeight, const std::string& text, float cellAspect = 1.f);
	const SpriteSheet& GetSpriteSheet() { return m_glyphSpriteSheet; }
protected:
	float GetGlyphAspect(int glyphUnicode) const; // For now this will always return 1.0f!!!

protected:
	std::string	m_fontName;
	SpriteSheet	m_glyphSpriteSheet;
};
