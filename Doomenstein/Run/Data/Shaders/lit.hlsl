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

   float4 local_tangent = float4(normalize(input.tangent), 0.0f);
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

	float3 dir_to_cam = normalize(POSITION - input.world_position);
	float3 normal = input.world_normal;
	float3 tangent = input.world_tangent;
	float3 bitangent = normalize(cross(normal, tangent));
	float3x3 tbn = float3x3(tangent, bitangent, normal);

	float4 diffuse_color = tDiffuse.Sample(sSampler, input.uv);
	float4 normal_color = tNormal.Sample(sSampler, input.uv);
	float4 specular_color = float4(1.0f, 1.0f, 1.0f, 1.0f);//Test
	float4 emissive_color = float4(1.0f, 1.0f, 1.0f, 1.0f);//Test

	// compute surface color
	float3 surf_color = pow(diffuse_color.xyz, gamma.xxx);
	surf_color = surf_color * input.color.xyz;
	float alpha = diffuse_color.w * input.color.w;

	float3 diffuse = ambient.xyz * ambient.w;
	float3 surf_normal = ColorToVector(normal_color.xyz);
	float3 world_normal = normalize(mul(surf_normal, tbn));

	float3 specular = float3(0.0f, 0.0f, 0.0f);
	float3 emissive = emissive_color.xyz * 0.1f; //here is emissive factor

	for (uint i = 0; i < 8; ++i)
	{
		float3 light_color = LIGHT[i].color.xyz;

		//Calculate light factor
		float3 dir_to_light = LIGHT[i].world_position - input.world_position;

		float dist = length(dir_to_light);

		float plane_dist = abs(dot(input.world_position - LIGHT[i].world_position, LIGHT[i].direction));

		dist = lerp(dist, plane_dist, LIGHT[i].directionFactor) + 0.001f;

		dir_to_light = normalize(dir_to_light);
		float3 light_direction = normalize(lerp(-dir_to_light, LIGHT[i].direction, LIGHT[i].directionFactor));
		float3 attenuation_vec = float3(1.0f, dist, dist * dist);

		float dot_angle = dot(-dir_to_light, LIGHT[i].direction);
		float cone_attenuation = smoothstep(LIGHT[i].outerAngle, LIGHT[i].innerAngle, dot_angle);

		float diff_attenuation = LIGHT[i].intensity / dot(attenuation_vec, LIGHT[i].attenuation) * cone_attenuation;
		float spec_attenuation = LIGHT[i].intensity / dot(attenuation_vec, LIGHT[i].attenuation) * cone_attenuation;

		// compute diffuse
		// max prevents lighting from the "back", which would subtract light
		float dot_incident = dot(-light_direction, world_normal);
		float facing = smoothstep(-0.4f, 0.1f, dot_incident);
		float diffuseFactor = max(0.0f, dot_incident) * diff_attenuation;

		// blinn-phong specular
		float3 hv = normalize(-light_direction + dir_to_cam);
		float specularFactor = max(0.0f, dot(world_normal, hv)) * facing;

		// finalize specular
		specularFactor = SPECULAR_FACTOR * pow(specularFactor, SPECULAR_POWER) * spec_attenuation;//1.0f here is specular factor, pow should use specular power

		diffuse += diffuseFactor * light_color;
		specular += specularFactor * light_color;
	}
	
	
	//clamp diffuse
	diffuse = min(1.0f * diffuse, float3(1.0f, 1.0f, 1.0f));//1.0f is diffuse factor
	diffuse = saturate(diffuse);
	specular *= specular_color.xyz;
	// spec does not get clamped - allowed to "blow out" for HDR purposes

	// compute final color; 
	float3 final_color = diffuse * surf_color + specular + emissive;
	final_color = pow(final_color.xyz, float3(gamma.xxx));//TODO: float3 here is inverse gamma.xxx
	final_color = lerp(final_color, burn_color, burn_mix);
	final_color = ApplyFog(input.world_position, final_color);
	return float4(final_color, alpha);
}