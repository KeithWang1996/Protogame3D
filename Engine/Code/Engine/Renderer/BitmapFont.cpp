#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
BitmapFont::BitmapFont(const char* fontName, const Texture* fontTexture)
	: m_fontName(fontName)
	, m_glyphSpriteSheet(SpriteSheet(*fontTexture, IntVec2(16, 16)))
{}

const Texture* BitmapFont::GetTexture() const
{
	return &m_glyphSpriteSheet.GetTexture();
}

void BitmapFont::AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, const Vec2& textMins,
	float cellHeight, const std::string& text, const Rgba8& tint, float cellAspect)
{
	float currTextX = textMins.x;
	float currTextY = textMins.y;

	for (int charIndex = 0; charIndex < text.length(); ++charIndex)
	{
		Vec2 uvAtMins;
		Vec2 uvAtMaxs;
		m_glyphSpriteSheet.GetSpriteDefinition(static_cast<int>(text[charIndex])).GetUVs(uvAtMins, uvAtMaxs);
		AddVertsForAABB2D(vertexArray, AABB2(Vec2(currTextX, currTextY), Vec2(currTextX + cellHeight * cellAspect, currTextY + cellHeight)), tint, uvAtMins, uvAtMaxs);
		currTextX += cellHeight * cellAspect;
	}
}

void BitmapFont::AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, const AABB2& box, float cellHeight,
	const std::string& text, const Rgba8& tint, float cellAspect,
	const Vec2& alignment)
{
	Vec2 dimension = GetDimensionsForText2D(cellHeight, text, cellAspect);
	if (box.maxs.x - box.mins.x < dimension.x || box.maxs.y - box.mins.y < dimension.y)
	{
		return;
	}
	float startX = RangeMapFloat(0.f, 1.f, box.mins.x, box.maxs.x - dimension.x, alignment.x);
	float startY = RangeMapFloat(0.f, 1.f, box.mins.y, box.maxs.y - dimension.y, alignment.y);

	for (int charIndex = 0; charIndex < text.length(); ++charIndex)
	{
		Vec2 uvAtMins;
		Vec2 uvAtMaxs;
		m_glyphSpriteSheet.GetSpriteDefinition(static_cast<int>(text[charIndex])).GetUVs(uvAtMins, uvAtMaxs);
		AddVertsForAABB2D(vertexArray, AABB2(Vec2(startX, startY), Vec2(startX + cellHeight * cellAspect, startY + cellHeight)), tint, uvAtMins, uvAtMaxs);
		startX += cellHeight * cellAspect;
	}
}

void BitmapFont::AddVertsForTextSquare2D(std::vector<Vertex_PCU>& vertexArray, const Vec2& squareMins, float squareHeight, const std::string& text, IntVec2 layout, const Rgba8& tint)
{
	float cellSideLength = squareHeight / static_cast<float>(layout.y);
	int textIndex = 0;
	for (int charIndexY = 0; charIndexY < layout.y; ++charIndexY)
	{
		for (int charIndexX = 0; charIndexX < layout.x; ++charIndexX)
		{
			Vec2 uvAtMins;
			Vec2 uvAtMaxs;
			m_glyphSpriteSheet.GetSpriteDefinition(static_cast<int>(text[textIndex])).GetUVs(uvAtMins, uvAtMaxs);
			AddVertsForAABB2D(vertexArray, 
				AABB2(Vec2(squareMins.x + static_cast<float>(charIndexX) * cellSideLength, squareMins.y + static_cast<float>(layout.y) * cellSideLength - static_cast<float>(charIndexY + 1) * cellSideLength),
					Vec2(squareMins.x + static_cast<float>(charIndexX + 1) * cellSideLength, squareMins.y+ static_cast<float>(layout.y) * cellSideLength - static_cast<float>(charIndexY) * cellSideLength)),
				tint, uvAtMins, uvAtMaxs);
			if (textIndex < text.length() - 1)
			{
				textIndex++;
			}
			else
			{
				textIndex = 0;
			}
		}
	}
}

Vec2 BitmapFont::GetDimensionsForText2D(float cellHeight, const std::string& text, float cellAspect)
{
	//int numChar = text.size;
	return Vec2(text.size() * (cellHeight * cellAspect), cellHeight);
}

float BitmapFont::GetGlyphAspect(int glyphUnicode) const
{
	UNUSED(glyphUnicode);
	return 1.f;
}