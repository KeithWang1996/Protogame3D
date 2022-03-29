#include <common.hlsl>
//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t VertexFunction( vs_input_t input )
{
   v2f_t v2f = (v2f_t)0;

   // forward vertex input onto the next stage
   float4 local_position = float4( input.position, 1.0f ); 
   v2f.world_position = mul(MODEL, local_position).xyz;

   float4 local_normal = float4(normalize(input.normal), 0.0f);
   float4 world_normal = normalize(mul(MODEL, local_normal));
   v2f.world_normal = world_normal;

   float4 local_tangent = float4(normalize(input.tangent.xyz), 0.0f);
   float4 world_tangent = normalize(mul(MODEL, local_tangent));
   v2f.world_tangent = world_tangent;

   v2f.color = input.color; 
   v2f.uv = input.uv;
   v2f.tint = TINT;
   float4 worldPos = mul(MODEL, float4(input.position, 1));
   float4 cameraPos = mul(VIEW, worldPos);
   float4 clipPos = mul(PROJECTION, cameraPos); //might have a w (usually 1 for now)
   
   v2f.position = clipPos;

   return v2f;
}
//raster step
//float3 ndcPos = clipPos / 
//--------------------------------------------------------------------------------------
// Fragment Shader
// 
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.


float4 FragmentFunction(v2f_t input) : SV_Target0
{
	//Test parameters
	//float4 AMBIENT = float4(0.5f, 0.5f, 0.5f, 0.5f);

	float burn_value = tDissolve.Sample(sSampler, input.uv).x;
	float burn_min = lerp(-dissolve_range, 1.0f, 1.0f - dissolve_depth);
	float burn_max = burn_min + dissolve_range;
	clip(burn_max - burn_value);

	float burn_mix = smoothstep(burn_min, burn_max, burn_value);
	float3 burn_color = lerp(burn_start_color, burn_end_color, burn_mix);

	//projection shader

	float3 dir_to_cam = normalize(POSITION - input.world_position);
	float3 normal = input.world_normal;
	float4 clip_pos = mul(PROJECTION_MATRIX, float4(input.world_position, 1.0f));
	float local_z = clip_pos.w;
	float3 ndc = clip_pos.xyz / local_z;
	float final_blend = step(0.01f, local_z);
	float dot3 = dot(normal, dir_to_cam);
	final_blend = final_blend * max(0.0f, dot3) * PROJECTOR_STRENGTH;
	float2 uv = (ndc.xy + 1.0f.xx) * 0.5f;
	float4 projection = tDiffuse3.Sample(sProjectionSampler, uv);
	return lerp(float4(0.f, 0.f, 0.f, 0.f), projection, final_blend);
}