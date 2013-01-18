#include "lightLib.hlsl"

cbuffer VertexProgramCBuffer
{
	float4 color;
	float4x4 vp;
};

Texture2D gDiffuseMap : register(t0);
Texture2D gNormalMap : register(t1);
Texture2D gSpecular : register(t2);

SamplerState pointSampler : register(s0);

struct VertexIn
{
	float3 position : POSITION;	
	//float3 normal : NORMAL;
	//float2 texCoord : TEXCOORD; 
	//float3 tangent : TANGENT;	
	//float3 binormal : BINORMAL;
	float4x4 instanceTransform : INSTANCETRANSFORM;
};
struct VertexOut
{
	float4 position	: SV_POSITION;
	float2 texCoord	: TEXCOORD0;
	float4 screenPos : TEXCOORD1;
};

VertexOut VS(VertexIn p_input)
{
	VertexOut vout;

	float4x4 wvp = mul(p_input.instanceTransform,vp);
	
	vout.position = mul( float4(p_input.position, 1.0f), wvp );
	vout.screenPos = vout.position;
	//vout.normal = mul( float4(p_input.normal, 0.0f), p_input.instanceTransform).xyz;
	
	//vout.texCoord = p_input.texCoord;
	
	return vout;
}

float4 PS(VertexOut p_input, float4 ndcPos : SV_POSITION) : SV_TARGET
{
	// from screen-space to NDC
	float2 samp = p_input.screenPos + float2( 0.5f, -0.5f);
	//HACK: resolution should be set by cbuffer
	samp = ndcPos.xy / float2( 1280.0f, 720.0f );
	//samp.y*-1.0f; // invert y to correct it. Caused by bad cube coords?

	float4 diffuseColor = float4(gDiffuseMap.Sample(pointSampler, samp));
	float4 normalColor = float4(gNormalMap.Sample(pointSampler, samp));	
	float4 specular = float4(gSpecular.Sample(pointSampler, samp));


	//return specular;
	// Normal in -1 to 1 range
	float4 normal = normalColor*2.0f-1.0f;
	
	// temporary hard coded lighting test
	//Light temp;
	//temp.diffusePower = 0.3f;
	//temp.specularPower = 0.0f;
	//temp.vec = float4(0.0f,-1.0f,0.0f,0.0f); // w==0, directional light
	//temp.diffuseColor = float4(1.0f,1.0f,1.0f,0.0f);
	//temp.specularColor = float4(0.0f,0.0f,0.0f,0.0f);
	//SurfaceLightingData lightManip = Lambert(temp, normal);

	LightInfo light;
	light.pos = float3( 0.0f, 0.0f, 0.0f );
	light.type = 1; // Should be set by instance/mesh
	light.dir = float3( 1.0f, 0.0f, 0.0f ); // Only used by point lights
	light.range = 10.0f;
	light.att = 0.0f;
	light.spotPower = 10.0f; //Not used;
	light.ambient = float4( 1.0f, 1.0f, 1.0f, 1.0f );
	light.diffuse = float4( 1.0f, 1.0f, 1.0f, 1.0f );
	light.spec = float4( 1.0f, 1.0f, 1.0f, 1.0f );
	light.enabled = true;

	SurfaceInfo surface;
	surface.diffuse = diffuseColor;
	surface.spec = specular;

	float3 eyePos = float3( 10.0f, 10.0f, 10.0f );

	float3	lightCol = pointLight( surface, light, eyePos, normalColor, light.pos );

	return float4( lightCol, 1.0f );

	//lets play!
	diffuseColor.x *= p_input.position.x/1000.0f;
	diffuseColor.y *= p_input.position.y/1000.0f;
	diffuseColor.z *= p_input.position.z/1000.0f;

	// smash'em together
	//return diffuseColor + lightManip.diffuseColor.r;
	//return float4(0.0f,1.0f,1.0f,1.0f);
}

