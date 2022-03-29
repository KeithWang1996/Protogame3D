#include "Game/Tile.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"

Tile::Tile(IntVec2 position)
	:m_position(position)
{}

Tile::Tile(int tileX, int tileY)
	: m_position(IntVec2(tileX, tileY))
{}

void Tile::Render() const
{
	AABB2 aabb = GetBound();
	Rgba8 tint = Rgba8(100, 255, 100, 255);
	if (m_type == TERRAIN_TYPE_STONE)
	{
		tint = Rgba8(128, 128, 128, 255);
	}
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawAABB2(aabb, tint);
}

AABB2 Tile::GetBound() const
{
	return AABB2(Vec2(static_cast<float>(m_position.x), static_cast<float>(m_position.y)), Vec2(static_cast<float>(m_position.x + 1.f), static_cast<float>(m_position.y + 1.f)));
}