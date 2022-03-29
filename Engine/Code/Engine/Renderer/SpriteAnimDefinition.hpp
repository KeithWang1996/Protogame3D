#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include <map>
#include <vector>
class SpriteSheet;
class SpriteDefinition;
class RenderContext;

enum class SpriteAnimPlaybackType
{
	ONCE,		// for 5-frame animation, plays 0,1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4...
	LOOP,		// for 5-frame animation, plays 0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0...
	PINGPONG,	// for 5-frame animation, plays 0,1,2,3,4,3,2,1,0,1,2,3,4,3,2,1,0,1,2,3,4,3,2,1,0,1...
};

class SpriteAnimDefinition
{
public:
	SpriteAnimDefinition(const SpriteSheet& sheet, std::vector<int>& spriteIndices ,
 		float durationSeconds, SpriteAnimPlaybackType playbackType = SpriteAnimPlaybackType::LOOP);

	const SpriteDefinition& GetSpriteDefAtTime(float seconds) const;

private:
	const			SpriteSheet& m_spriteSheet;
	//new index system
	std::vector<int> m_spriteIndices;

	float			m_durationSeconds = 1.f;
	int				m_fps = 10;
	SpriteAnimPlaybackType	m_playbackType = SpriteAnimPlaybackType::LOOP;
};

class SpriteAnimSet
{
public:
	SpriteAnimSet() = default;
	SpriteAnimSet(RenderContext& renderer, const tinyxml2::XMLElement& SpriteAnimSetXmlElement);
	SpriteAnimSet(const tinyxml2::XMLElement& SpriteAnimSetXmlElement, SpriteSheet* spritesheet);//for XML file in Doomenstein
	SpriteAnimDefinition* GetAnimDefinition(std::string name) { return m_animDefinitions[name]; }
	int GetSize() const { return (int)m_animDefinitions.size(); }
	~SpriteAnimSet();
	SpriteSheet* m_spriteSheet;
private:
	//int m_fps = 10;
	bool m_isSpriteSheetCreatedHere = false;
	std::map<std::string, SpriteAnimDefinition* > m_animDefinitions;
};
