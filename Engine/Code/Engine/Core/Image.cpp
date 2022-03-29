#include "Engine/Core/Image.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
Image::Image(const char* imageFilePath)
	: m_imageFilePath(imageFilePath)
{
	int imageTexelSizeX = 0;
	int imageTexelSizeY = 0;
	int numComponents = 0;
	constexpr int numComponentsRequested = 4;
	unsigned char* imageData = nullptr;
	stbi_set_flip_vertically_on_load(1);
	imageData = stbi_load(imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested);
	m_dimensions = IntVec2(imageTexelSizeX, imageTexelSizeY);
	for (int pixelIndex = 0; pixelIndex < imageTexelSizeX * imageTexelSizeY; ++pixelIndex)
	{
		m_rgbaTexels.push_back(Rgba8(imageData[pixelIndex * 4], imageData[pixelIndex * 4 + 1], imageData[pixelIndex * 4 + 2], imageData[pixelIndex * 4 + 3]));
	}
	GUARANTEE_OR_DIE(imageData, Stringf("Failed to load image \"%s\"", imageFilePath));
	GUARANTEE_OR_DIE(numComponents >= 3 && numComponents <= 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf("ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY));
}

Rgba8 Image::GetTexelColor(int texelX, int texelY) const
{
	return m_rgbaTexels[static_cast<size_t>(texelY) * static_cast<size_t>(m_dimensions.x) + static_cast<size_t>(texelX)];
}

Rgba8 Image::GetTexelColor(const IntVec2& texelCoords) const
{
	return GetTexelColor(texelCoords.x, texelCoords.y);
}

void Image::SetTexelColor(int texelX, int texelY, const Rgba8& color)
{
	m_rgbaTexels[static_cast<size_t>(texelY) * static_cast<size_t>(m_dimensions.x) + static_cast<size_t>(texelX)] = color;
}

void Image::SetTexelColor(const IntVec2& texelCoords, const Rgba8& color)
{
	SetTexelColor(texelCoords.x, texelCoords.y, color);
}