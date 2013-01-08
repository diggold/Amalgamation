cbuffer cbPerFrame : register(b0)
{
	float4 gEyePosW;
	matrix gView; 
    matrix gProj;
	float4 Color;
	float fadeIn;
	float fadeOut;
	float particleMaxAge;
	float maxOpacity;
};

cbuffer cbFixed
{
	float2 gQuadTexC[4] = 
	{
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
	};
};
  
struct VS_OUT
{
	float3 Position  : POSITION;
	float Size : SIZE;
	float  Age	: AGE;
};

struct GS_OUT
{
	float4 posH  : SV_POSITION;
	float2 texC  : TEXCOORD;
	float4 color : COLOR;
};

[maxvertexcount(4)]
void GShader(point VS_OUT gIn[1], 
            inout TriangleStream<GS_OUT> triStream)
{	
	float3 look  = normalize(gEyePosW.xyz - gIn[0].Position);
	float3 right = normalize(cross(float3(0,1,0), look));
	float3 up    = cross(look, right);
	
	matrix W;
	W[0] = float4(right,       0.0f);
	W[1] = float4(up,          0.0f);
	W[2] = float4(look,        0.0f);
	W[3] = float4(gIn[0].Position, 1.0f);
	
	matrix rot = matrix(cos(gIn[0].Age), -sin(gIn[0].Age), 0, 0,
						sin(gIn[0].Age), cos(gIn[0].Age), 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 1);

	W = mul(rot, W);
	matrix vp = mul(gView, gProj);
	matrix WVP = mul(W, vp);
	
	float halfWidth  = 0.5f*gIn[0].Size;
	float halfHeight = 0.5f*gIn[0].Size;

	float4 v[4];
	v[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f);
	v[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);
	v[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);
	v[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);
	float2 t[4];
	t[0] = float2(0.0f, 1.0f);
	t[1] = float2(1.0f, 1.0f);
	t[2] = float2(0.0f, 0.0f);
	t[3] = float2(1.0f, 0.0f);
	
	float opacity;
	if (gIn[0].Age < fadeIn) //Fade in
		opacity = smoothstep(0.0f, 1.0f, gIn[0].Age / fadeIn); 
	else if (gIn[0].Age > fadeOut)//Fade out
		opacity = (particleMaxAge - gIn[0].Age) / (particleMaxAge - fadeOut);              //1.0f - gIn[0].Age/(5.0f);
	else
		opacity = 1.0f;
	
	opacity *= maxOpacity;
	GS_OUT gOut;
	[unroll]
	for(int i = 0; i < 4; ++i)
	{
		gOut.posH  = mul(v[i], WVP);
		gOut.texC  = t[i];
		gOut.color = float4(Color.x, Color.y, Color.z, opacity);
		triStream.Append(gOut);
	}	
}