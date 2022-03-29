#pragma once
#include "Engine/Math/IntRange.hpp"
#include <string>
struct ID3D11SamplerState;
class RenderContext;
enum eSamplerType
{
	SAMPLER_POINT,		//pixelated look
	SAMPLER_BILINEAR,	//smoother look - no mips
};
class Sampler
{
public:
	Sampler(RenderContext* ctx, eSamplerType type);
	~Sampler();

	inline ID3D11SamplerState* GetHandle() const { return m_handle; }//function is short enough
public:
	RenderContext* m_owner;
	ID3D11SamplerState* m_handle;
};