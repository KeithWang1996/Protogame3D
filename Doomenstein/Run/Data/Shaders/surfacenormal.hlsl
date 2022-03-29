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
  // v2f.color = float4(VectorToColor(world_normal.xyz), 1.f);
   //v2f.color = float4(LIGHT.color, LIGHT.intensity);
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
	float3 normal = input.world_normal;
	float3 tangent = input.world_tangent;
	float3 bitangent = normalize(cross(normal, tangent));
	float3x3 tbn = float3x3(tangent, bitangent, normal);

	float4 normal_color = tNormal.Sample(sSampler, input.uv);
	float3 surf_normal = ColorToVector(normal_color.xyz);
	float3 world_normal = normalize(mul(surf_normal, tbn));

	
	return float4(VectorToColor(world_normal), 1.0f);
	//normal_color = mul(normal_color, tbn);

	//float4 surface_normal_color = float4(VectorToColor(normal_color), 1.0f);

	//float4 final_color = input.color * surface_normal_color;
	//return final_color * input.tint;

   // we'll outoupt our UV coordinates as color here
   // to make sure they're being passed correctly.
   // Very common rendering debugging method is to 
   // use color to portray information; 
   //float4 uvAsColor = float4( input.uv, 0.0f, 1.0f ); 
   //float4 finalColor = uvAsColor * input.color; 
   //float r = 0.5f * sin(input.uv.x * 40.0f + SYSTEM_TIME_SECONDS * 10.0f + 1.0f);
   //return finalColor;
}