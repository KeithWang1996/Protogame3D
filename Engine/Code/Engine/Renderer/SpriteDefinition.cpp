#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Image.hpp"
SpriteDefinition::SpriteDefinition(const SpriteSheet& spriteSheet, int spriteIndex, const Vec2& uvAtMins, const Vec2& uvAtMaxs)
	: m_spriteSheet(spriteSheet)
	, m_spriteIndex(spriteIndex)
	, m_uvAtMins(uvAtMins)
	, m_uvAtMaxs(uvAtMaxs)
{}

void SpriteDefinition::GetUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs) const
{
	out_uvAtMaxs = m_uvAtMaxs;
	out_uvAtMins = m_uvAtMins;
}
const Texture& SpriteDefinition::GetTexture() const
{
	return m_spriteSheet.GetTexture();
}
float SpriteDefinition::GetAspect() const
{
	const Texture& texture = GetTexture();
	float textureAspect = static_cast<float>(texture.m_imageTexelSizeX) / static_cast<float>(texture.m_imageTexelSizeY);
	float selfAspect = (m_uvAtMaxs.x - m_uvAtMins.x) / (m_uvAtMaxs.y - m_uvAtMins.y);
	return textureAspect * selfAspect;
}