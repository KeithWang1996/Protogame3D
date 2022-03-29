//--------------------------------------------------------------------------------------
// Stream Input
// ------
// Stream Input is input that is walked by the vertex shader.  
// If you say Draw(3,0), you are telling to the GPU to expect '3' sets, or 
// elements, of input data.  IE, 3 vertices.  Each call of the VertxShader
// we be processing a different element. 
//--------------------------------------------------------------------------------------

// inputs are made up of internal names (ie: uv) and semantic names
// (ie: TEXCOORD).  "uv" would be used in the shader file, where
// "TEXCOORD" is used from the client-side (cpp code) to attach ot. 
// The semantic and internal names can be whatever you want, 
// but know that semantics starting with SV_* usually denote special 
// inputs/outputs, so probably best to avoid that naming.
struct vs_input_t 
{
   // we are not defining our own input data; 
   float3 position      : POSITION; 
   float4 color         : COLOR; 
   float2 uv            : TEXCOORD; 
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

//Textures & samplers are also a form of constant
//data - uniform/constant across the entire call
Texture2D<float4> tDiffuse : register(t0); // color of the surface
SamplerState sSampler : register(s0); // sampler are rules on how to sample

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
}; 

float RangeMap(float val, float inMin, float inMax, float outMin, float outMax)
{
	float domain = inMax - inMin;
	float range = outMax - outMin;
	return ((val - inMin) / domain) * range + outMin;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t VertexFunction( vs_input_t input )
{
   v2f_t v2f = (v2f_t)0;

   // forward vertex input onto the next stage
   v2f.position = float4( input.position, 1.0f ); 
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
	float4 color =  tDiffuse.Sample( sSampler, input.uv );
	float4 final_color = color * input.color;
	clip(final_color.a - .5f);

	if (final_color.r * final_color.g * final_color.b * final_color.a > 0.9)
	{
		float stripe = 0.5 + 0.5 * sin(3.0 * SYSTEM_TIME_SECONDS - 33.0 * input.uv.x + 33.0 * input.uv.y);
		stripe = 1.0 - 0.5 * (stripe * stripe * stripe * stripe * stripe * stripe * stripe * stripe);
		final_color.g *= stripe;
		final_color.b *= stripe;
	}

	return final_color * input.tint;

   // we'll outoupt our UV coordinates as color here
   // to make sure they're being passed correctly.
   // Very common rendering debugging method is to 
   // use color to portray information; 
   //float4 uvAsColor = float4( input.uv, 0.0f, 1.0f ); 
   //float4 finalColor = uvAsColor * input.color; 
   //float r = 0.5f * sin(input.uv.x * 40.0f + SYSTEM_TIME_SECONDS * 10.0f + 1.0f);
   //return finalColor;
}