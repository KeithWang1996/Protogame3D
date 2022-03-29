#include <common.hlsl>
//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t VertexFunction( vs_input_t input )
{
   v2f_t v2f = (v2f_t)0;

   // forward vertex input onto the next stage
   v2f.position = float4( input.position, 1.0f ); 
   v2f.color = input.color; 
   v2f.uv = input.uv; 
   
   float4 worldPos = float4(input.position, 1);
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
	float4 color =  tDiffuse.Sample( sSampler, input.uv );
	color = color * input.color;
	color = float4(1.0f, 1.0f, 1.0f, color.a * 2.0f) - color;

	return color;

   // we'll outoupt our UV coordinates as color here
   // to make sure they're being passed correctly.
   // Very common rendering debugging method is to 
   // use color to portray information; 
   //float4 uvAsColor = float4( input.uv, 0.0f, 1.0f ); 
   //float4 finalColor = uvAsColor * input.color; 
   //float r = 0.5f * sin(input.uv.x * 40.0f + SYSTEM_TIME_SECONDS * 10.0f + 1.0f);
   //return finalColor;
}