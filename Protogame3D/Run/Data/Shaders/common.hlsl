struct vs_input_t
{
	// we are not defining our own input data; 
	float3 position      : POSITION;
	float4 color         : COLOR;
	float2 uv            : TEXCOORD;

	float4 tangent		: TANGENT;
	float3 normal		: NORMAL;
};

struct light_t
{
	float3 world_position;
	float innerAngle;

	float3 color;
	float intensity;

	float3 attenuation;
	float outerAngle;

	float3 specAttenuation;
	float light_pad00;

	float3 direction;
	float directionFactor;
};

static float SHIFT = 0.75f;
cbuffer time_constants : register(b0)//index zero is time data
{
	float SYSTEM_TIME_SECONDS;
	float SYSTEM_TIME_DELTA_SECONDS;//can use, cannot change, basically constant
};

// MVP: Model - View - Projection
cbuffer camera_constants : register(b1)
{
	//float2 orthoMin;
	//float2 orthoMax;
	float4x4 PROJECTION; // CAMERA_TO_CLIP_TRANSFORM
	float4x4 VIEW;
	float3 POSITION;
	float pad00;
};

cbuffer model_constants : register(b2)
{
	float4x4 MODEL;

	float4 TINT;

	float SPECULAR_FACTOR;
	float SPECULAR_POWER;
	float2 obj_pad00;
};

cbuffer light_constants : register(b3)
{
	float4 ambient;

	float4 diffuse;

	float diffuse_factor;
	float emissive_factor;
	float fog_near_distance;
	float fog_far_distance;

	float3 fog_near_color;
	float gamma;

	float3 fog_far_color;
	float light_info_pad00;

	light_t LIGHT[8];
}

cbuffer material_constants : register(b4)
{
	float3 burn_start_color;
	float dissolve_depth;

	float3 burn_end_color;
	float dissolve_range;

	uint STEP_COUNT;
	float DEPTH;
	float2 material_pad00;
}

cbuffer project_constants : register(b5)
{
	float4x4 PROJECTION_MATRIX;
	float3 PROJECTOR_POSITION;
	float PROJECTOR_STRENGTH;
};

//Textures & samplers are also a form of constant
//data - uniform/constant across the entire call
Texture2D<float4> tDiffuse : register(t0); // color of the surface
Texture2D<float4> tNormal : register(t1); // normal of the surface
Texture2D<float4> tDiffuse1 : register(t2); // color of the surface1
Texture2D<float4> tNormal1 : register(t3); // normal of the surface1
Texture2D<float4> tDiffuse2 : register(t4); // color of the surface2
Texture2D<float4> tNormal2 : register(t5); // normal of the surface2
Texture2D<float4> tDiffuse3 : register(t6); // taken by projection for now
Texture2D<float4> tNormal3 : register(t7); // normal of the surface
Texture2D<float4> tDissolve : register(t8); // color of the surface
Texture2D<float4> tHeight : register(t9);
SamplerState sSampler : register(s0); // sampler are rules on how to sample
SamplerState sProjectionSampler : register(s1);
//--------------------------------------------------------------------------------------
// Programmable Shader Stages
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// for passing data from vertex to fragment (v-2-f)
struct v2f_t
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv : UV;
	float4 tint : TINT;
	float3 world_position : WORLD_POSITION;
	float3 world_normal : WORLD_NORMAL;
	float4 world_tangent : WORLD_TANGENT;
};

float RangeMap(float val, float inMin, float inMax, float outMin, float outMax)
{
	float domain = inMax - inMin;
	float range = outMax - outMin;
	return ((val - inMin) / domain) * range + outMin;
}

float3 VectorToColor(float3 vec)
{
	return normalize((vec + float3(1.0f, 1.0f, 1.0f)) * 0.5f);
}

float3 ColorToVector(float3 color)
{
	return normalize(color * float3(2.0f, 2.0f, 1.0f) - float3(1.0f, 1.0f, 0.0f));
}
float3 ApplyFog(float3 world_pos, float3 color)
{
	float dist = length(world_pos - POSITION);
	float t = smoothstep(fog_near_distance, fog_far_distance, dist);
	return lerp(color, fog_far_color, t.xxx);
}

float4 AddProjection(float3 world_position, float3 dir_to_cam, float3 normal)
{
	float4 clip_pos = mul(PROJECTION_MATRIX, float4(world_position, 1.0f));
	float local_z = clip_pos.w;
	float3 ndc = clip_pos.xyz / local_z;
	float final_blend = step(0.01f, local_z);
	float dot3 = dot(normal, dir_to_cam);
	final_blend = final_blend * max(0.0f, dot3) * PROJECTOR_STRENGTH;
	float2 projectuv = (ndc.xy + 1.0f.xx) * 0.5f;
	float4 projection = tDiffuse3.Sample(sProjectionSampler, projectuv);
	return lerp(float4(0.f, 0.f, 0.f, 0.f), projection, final_blend);
}