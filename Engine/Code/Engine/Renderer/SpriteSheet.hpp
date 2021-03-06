#pragma once
#include <vector>
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Math/IntVec2.hpp"
struct Vec2;

class Texture;
class SpriteSheet
{
public:
	explicit SpriteSheet(const Texture& texture, const IntVec2& simpleGridLayout);

	const Texture&				GetTexture() const		{return m_texture; }
	int							GetNumSprites() const	{ return (int)m_spriteDefs.size(); }
	const SpriteDefinition&		GetSpriteDefinition(int spriteIndex) const;
	void						GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, IntVec2 spriteCoords) const;
	void						GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex) const;
	IntVec2						GetLayout() const { return m_simpleGridLayout; }
private:
	const Texture&					m_texture;
	std::vector<SpriteDefinition>	m_spriteDefs;
	IntVec2 m_simpleGridLayout;
};