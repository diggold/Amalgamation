#include "../Game/perFrameCBuffer.hlsl"
#include "../Game/common.hlsl"


cbuffer cbPerObject: register(b1)
{
	float4x4 worldMat;
	float4 color;	
	float4 uvCropRect;
	float fadeIn;
	float fadeOut;
	float particleMaxAge;
	float maxOpacity;
	int alignment;
	int spawnSpace;	   
	int particleSpace;
	float padding2; // Padding
};

Texture2D Texture : register(t0);

struct Particle
{
	float3 Position			: POSITION;
	float Age				: AGE;
	float3 Velocity			: VELOCITY;
	float AngularVelocity	: ANGULARVELOCITY;
	float2 Size				: SIZE;
	float Rotation			: ROTATION;
	float pad				: PAD;
};
struct GS_OUT
{
	float4 posH  : SV_POSITION;
	float2 texC  : TEXCOORD;
	float4 color : COLOR;
};

struct PixelOut
{
	float4 diffuse : SV_TARGET0;
};

Particle VS(Particle vIn)
{
	return vIn;
}

[maxvertexcount(4)]
void GS(point Particle gIn[1], 
			inout TriangleStream<GS_OUT> triStream)
{	
	float opacity;
	if (gIn[0].Age < fadeIn) //Fade in
		opacity = smoothstep(0.0f, 1.0f, gIn[0].Age / fadeIn); 
	else if (gIn[0].Age > fadeOut)//Fade out
		opacity = (particleMaxAge - gIn[0].Age) / (particleMaxAge - fadeOut);
	else
		opacity = 1.0f;
	
	opacity *= maxOpacity;
	opacity *= color.w;

	const float epsilon = 0.01;
	if( opacity < epsilon ) {
		return;
	}

	float4 position = float4( gIn[0].Position.xyz, 1.0 );
	if( particleSpace == 1 ) // 1 = LOCAL
	{ 
		position = mul( position, worldMat );
		gIn[0].Velocity = mul(float4(gIn[0].Velocity, 0.0f), worldMat).xyz;
	}

	float4x4 W;
	if( alignment.x == 0 ) //Observer
	{
		float3 look  = normalize(gCameraPos.xyz - position.xyz);
		float3 right = normalize(cross(float3(0,1,0), look));
		float3 up    = cross(look, right);
		W[0] = float4(right,       0.0f);
		W[1] = float4(up,          0.0f);
		W[2] = float4(look,        0.0f);
		W[3] = float4(position.xyz, 1.0f);
	}
	else if( alignment.x == 1 ) //Screen
	{
		float3 look  = -gCameraForward.xyz;
		float3 up    = gCameraUp.xyz;
		float3 right = normalize(cross(up, look));
		W[0] = float4(right,       0.0f);
		W[1] = float4(up,          0.0f);
		W[2] = float4(look,        0.0f);
		W[3] = float4(position.xyz, 1.0f);
	}
	else if( alignment.x == 2 ) //World Up
	{
		float3 up 	 = float3(0, 1, 0);
		float3 right = normalize(cross(up, gCameraPos.xyz - position.xyz));
		float3 look  = cross(right, up);
		W[0] = float4(right,       0.0f);
		W[1] = float4(up,          0.0f);
		W[2] = float4(look,        0.0f);
		W[3] = float4(position.xyz, 1.0f);
	}
	else //Velocity
	{
		float3 right = normalize(gIn[0].Velocity);
		float3 up 	 = normalize(cross(gCameraPos.xyz - position.xyz, right));
		float3 look  = cross(right, up);
		W[0] = float4(right,       0.0f);
		W[1] = float4(up,          0.0f);
		W[2] = float4(look,        0.0f);
		W[3] = float4(position.xyz, 1.0f);
	}
	
	float4x4 rot = float4x4(cos(gIn[0].Rotation), -sin(gIn[0].Rotation), 0, 0,
						sin(gIn[0].Rotation), cos(gIn[0].Rotation), 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1);

	W = mul( rot, W );
	float4x4 WVP = mul(W, gViewProj);
	
	float halfWidth  = 0.5f*gIn[0].Size.x;
	float halfHeight = 0.5f*gIn[0].Size.y;
	
	if (particleSpace == 2)
		WVP = float4x4(-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, gIn[0].Position.x, gIn[0].Position.y, 0, 1);

	float4 v[4];
	v[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f);
	v[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);
	v[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);
	v[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);
	float2 t[4];
	t[0] = float2(uvCropRect.z, uvCropRect.w);
	t[1] = float2(uvCropRect.x, uvCropRect.w);
	t[2] = float2(uvCropRect.z, uvCropRect.y);
	t[3] = float2(uvCropRect.x, uvCropRect.y);
	
	GS_OUT gOut;
	[unroll]
	for(int i = 0; i < 4; ++i)
	{
		gOut.posH  = mul(v[i], WVP);
		gOut.texC  = t[i];
		gOut.color = float4(color.x, color.y, color.z, opacity);
		triStream.Append(gOut);
	}	
}

float4 PS(GS_OUT pIn) : SV_TARGET
{
	clip(pIn.color.a-0.01f);
	PixelOut pix_out;
	pix_out.diffuse = Texture.Sample(g_samplerAnisotropicWrap, pIn.texC);
	pix_out.diffuse *= pIn.color;
	return pix_out.diffuse;
}