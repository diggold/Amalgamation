#include "perFrameCBuffer.hlsl"
#include "utility.hlsl"

static const float blurFilter3[3][3] = {{0.01f,0.08f,0.01f},
									   {0.08f,0.64f,0.01f},
									   {0.01f,0.08f,0.01f}};

static const float blurFilter5[5][5] = {{0.01f,0.02f,0.04f,0.02f,0.01f},
										{0.02f,0.04f,0.08f,0.04f,0.02f},
										{0.04f,0.08f,0.16f,0.08f,0.04f},
										{0.02f,0.04f,0.08f,0.04f,0.02f},
										{0.01f,0.02f,0.04f,0.02f,0.01f}};

Texture2D gDiffBuffer	: register(t0);
Texture2D gNormalBuffer	: register(t1);
Texture2D gSpecBuffer	: register(t2);
Texture2D gLightDiff 	: register(t3);
Texture2D gLightSpec	: register(t4);
Texture2D gDepthBuffer	: register(t5);

SamplerState pointSampler : register(s0);

cbuffer SSAO : register(b1)
{
	float scale;
	float bias;
	float intensity;
	float sampleRadius;
	float epsilon;
	const static float randSize = 64;
}

struct VertexIn
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD; 
};
struct VertexOut
{
    float4 position	: SV_POSITION;
	float2 texCoord	: TEXCOORD;
};

// Changed by Jarl: removed additional depth sampling, added depth as param instead
float3 getPosition(float2 p_uv,float p_depth) 
{
	return getWorldPosFromTexCoord( p_uv, p_depth, gViewProjInverse);
}

VertexOut VS(VertexIn p_input)
{
	VertexOut vout;
	vout.position = float4(p_input.position,1.0f);
	vout.texCoord = p_input.texCoord;
    
	return vout;
}

float4 PS(VertexOut input) : SV_TARGET
{
	uint3 index;
	index.xy = input.position.xy;
	index.z = 0;

	float4 diffColor = gDiffBuffer.Load( index );
	float4 specColor = gSpecBuffer.Load( index );
	float4 lightDiff = gLightDiff.Load( index ) * 10.0f;
	float4 lightSpec = gLightSpec.Load( index ) * 10.0f;
	float depth = gDepthBuffer.Load( index ).r;
	float4 finalCol = float4(0,0,0,0);
	finalCol += specColor * lightSpec;
	finalCol += diffColor * lightDiff;

	//float4 sampleNormal = float4(gNormalBuffer.Sample(pointSampler, input.texCoord).rgb,1.0f);
	float3 fog = gFogColorAndFogFar.rgb;
	float3 ambient = gAmbientColorAndFogNear.rgb;
	float2 fogNearFarPercentage = float2(gAmbientColorAndFogNear.a,gFogColorAndFogFar.a);
	float3 position = getPosition(input.texCoord,depth);

	// calc linear depths
	float pixelDepthW = length(position-gCameraPos.xyz);
	float linDepth = pixelDepthW / (gFarPlane-gNearPlane);
	float fogDepth = saturate(pixelDepthW / ((gFarPlane*fogNearFarPercentage.x)-(gNearPlane*(2.0f-fogNearFarPercentage.y))));
	
	float finalAO = 0.0f;

	float3 finalEmissiveValue = float3(0,0,0);
		
	float4 sampledGlow;
	[unroll]
	for(int x=-2;x<3;x++)
	{
		[unroll]
		for(int y=-2;y<3;y++)
		{
			finalAO += gLightDiff.Load( index+uint3(x,y,0) ).a * blurFilter5[x+2][y+2];
			
			sampledGlow = gDiffBuffer.Load( index+uint3(x*2,y*2,0) ).rgba;
			sampledGlow.rgb *= sampledGlow.a;
			finalEmissiveValue += sampledGlow.rgb * blurFilter5[x+2][y+2];
		}
	}
	// apply ao
	finalCol *= float4( finalAO, finalAO, finalAO, 1.0f );
	finalCol += float4 (ambient,0.0f );

	finalCol = float4( lerp( finalCol.rgb, fog, fogDepth ), 0.0f ); // can do this when light is separate from diffuse
	return float4( finalCol.rgb + finalEmissiveValue.rgb, 1.0f ); // then all light can be added here like glow is now
}