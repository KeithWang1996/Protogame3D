#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/StringUtils.hpp"

SpriteAnimDefinition::SpriteAnimDefinition(const SpriteSheet& sheet, std::vector<int>& spriteIndices,
	float durationSeconds, SpriteAnimPlaybackType playbackType)
	: m_spriteSheet(sheet)
	, m_spriteIndices(spriteIndices)
	, m_durationSeconds(durationSeconds)
	, m_playbackType(playbackType)
{}
const SpriteDefinition& SpriteAnimDefinition::GetSpriteDefAtTime(float seconds) const
{
	int numFrame = (int)m_spriteIndices.size();
	float frameSecond = m_durationSeconds * 0.5f / numFrame;
	int spriteIndex = 0;
	int frameIndex = RoundDownToInt(seconds / frameSecond);
	switch (m_playbackType)
	{
	case SpriteAnimPlaybackType::LOOP:
		frameIndex = frameIndex % numFrame;
		if (frameIndex < 0)
		{
			frameIndex += numFrame;
		}
		spriteIndex = m_spriteIndices[frameIndex];
		break;
	case SpriteAnimPlaybackType::ONCE:
		if (seconds < 0.f)
		{
			spriteIndex = m_spriteIndices[0];
		}
		else if (seconds > m_durationSeconds)
		{
			spriteIndex = m_spriteIndices[numFrame - 1];
		}
		else
		{
			spriteIndex = m_spriteIndices[frameIndex];
		}
		break;
	case SpriteAnimPlaybackType::PINGPONG:
		int numFramePingpong = (numFrame - 1) * 2;
		frameIndex = frameIndex % numFramePingpong;
		if (frameIndex < 0)
		{
			frameIndex += numFramePingpong;
		}

		if (frameIndex < numFrame)
		{
			spriteIndex = m_spriteIndices[frameIndex];
		}
		else
		{
			spriteIndex = m_spriteIndices[numFrame - 1] + numFrame - 1 - frameIndex;
		}
		break;
	}
	return m_spriteSheet.GetSpriteDefinition(spriteIndex);
}

SpriteAnimSet::SpriteAnimSet(RenderContext& renderer, const tinyxml2::XMLElement& SpriteAnimSetXmlElement)
{
	m_isSpriteSheetCreatedHere = true;
	std::string spriteSheetPath = ParseXmlAttribute(SpriteAnimSetXmlElement, "spriteSheet", "KushnariovaCharacters_12x53.png");
	spriteSheetPath = "Data/Images/" + spriteSheetPath;
	IntVec2 spriteSheetLayout = ParseXmlAttribute(SpriteAnimSetXmlElement, "spriteLayout", IntVec2(12, 53));
	Texture* spriteSheetTexture = renderer.CreateOrGetTextureFromFile(spriteSheetPath.c_str());
	//int fps = ParseXmlAttribute(SpriteAnimSetXmlElement, "fps", 10);
	m_spriteSheet = new SpriteSheet(*spriteSheetTexture, spriteSheetLayout);
	//For actor 
	for (const tinyxml2::XMLElement* animation = SpriteAnimSetXmlElement.FirstChildElement(); animation != NULL; animation = animation->NextSiblingElement())
	{
		std::string animName = ParseXmlAttribute(*animation, "name", "name");
		Strings defaultText;
		Strings spriteIndicesText = ParseXmlAttribute(*animation, "spriteIndexes", defaultText);
		int numFrame = (int)spriteIndicesText.size();
		std::vector<int> frameIndices;
		for (int i = 0; i < numFrame; ++i)
		{
			frameIndices.push_back(atoi(spriteIndicesText[i].c_str()));
		}
		m_animDefinitions[animName] = new SpriteAnimDefinition(*m_spriteSheet, frameIndices, 1.f);
	}
}

SpriteAnimSet::SpriteAnimSet(const tinyxml2::XMLElement& SpriteAnimSetXmlElement, SpriteSheet* spritesheet)
	: m_spriteSheet(spritesheet)
{
	m_isSpriteSheetCreatedHere = false;
	Strings defaultText;
	const tinyxml2::XMLAttribute* attribute = SpriteAnimSetXmlElement.FirstAttribute();
	while (attribute)
	{
		std::string animName = attribute->Name();
		Strings spriteIndicesText = ParseXmlAttribute(SpriteAnimSetXmlElement, animName.c_str(), defaultText);
		int numFrame = (int)spriteIndicesText.size();
		if (numFrame == 0)
		{
			continue;
		}
		std::vector<int> frameIndices;
		for (int i = 0; i < numFrame; ++i)
		{
			frameIndices.push_back(atoi(spriteIndicesText[i].c_str()));
		}
		m_animDefinitions[animName] = new SpriteAnimDefinition(*m_spriteSheet, frameIndices, 1.f);
		attribute = attribute->Next();
	}
}

SpriteAnimSet::~SpriteAnimSet()
{
	if (m_isSpriteSheetCreatedHere)
	{
		delete m_spriteSheet;
		m_spriteSheet = nullptr;
	}
	else
	{
		m_spriteSheet = nullptr;
	}
	std::map<std::string, SpriteAnimDefinition* >::iterator itr;
	for (itr = m_animDefinitions.begin(); itr != m_animDefinitions.end(); ++itr)
	{
		delete itr->second;
	}
	m_animDefinitions.clear();
}