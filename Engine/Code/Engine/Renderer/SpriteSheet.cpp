#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/IntVec2.hpp"
SpriteSheet::SpriteSheet(const Texture& texture, const IntVec2& simpleGridLayout)
	: m_texture(texture)
	, m_simpleGridLayout(simpleGridLayout)
{
	for (int yIndex = 0; yIndex < simpleGridLayout.y; ++yIndex)
	{
		for (int xIndex = 0; xIndex < simpleGridLayout.x; ++xIndex)
		{
			m_spriteDefs.push_back(SpriteDefinition(*this, xIndex + yIndex * simpleGridLayout.x,
				Vec2(static_cast<float>(xIndex) / static_cast<float>(simpleGridLayout.x), static_cast<float>(simpleGridLayout.y - 1 - yIndex) / static_cast<float>(simpleGridLayout.y)),
				Vec2(static_cast<float>(xIndex + 1) / static_cast<float>(simpleGridLayout.x), static_cast<float>(simpleGridLayout.y - yIndex) / static_cast<float>(simpleGridLayout.y))));
		}
	}
}

const SpriteDefinition& SpriteSheet::GetSpriteDefinition(int spriteIndex) const
{
	if (spriteIndex >= m_spriteDefs.size())
	{
		return m_spriteDefs[0];//out of bounds
	}
	return m_spriteDefs[spriteIndex];
}

void SpriteSheet::GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex) const
{
	SpriteDefinition def = GetSpriteDefinition(spriteIndex);
	def.GetUVs(out_uvAtMins, out_uvAtMaxs);
}

void SpriteSheet::GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, IntVec2 spriteCoords) const
{
	GetSpriteUVs(out_uvAtMins, out_uvAtMaxs, spriteCoords.x + spriteCoords.y * m_simpleGridLayout.x);
}