cbuffer StaticBuffer : register(b0)
{
	float4 color;
	float4 vLightDir;
	float4 vDIC;
	float4 vSpecIC;
};

//------------------------------------------------------------------------------
cbuffer DynamicBuffer : register(b1)
{
	matrix model;
	matrix view;
	matrix projection;
	matrix worldInverseTranspose;
	float4 vecEye;
};

//------------------------------------------------------------------------------
// Per-vertex data used as input to the vertex shader.
//------------------------------------------------------------------------------
struct VertexShaderInput
{
	float3 pos : SV_Position;
	float3 normal: NORMAL;
	float2 texCoord: TEXCOORD;
};

//------------------------------------------------------------------------------
// Per-pixel color data passed through the pixel shader.
//------------------------------------------------------------------------------
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float3 light : TEXCOORD1;
	float3 normal : NORMAL;
	float3 eyeRay: TEXCOORD2;
};

//------------------------------------------------------------------------------
// Simple shader to do vertex processing on the GPU.
//------------------------------------------------------------------------------
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);
	float4 normal = float4(input.normal, 0.0f);

	// Transform the vertex position into projected space.
	float4 worldPos = mul(pos, model);
	pos = mul(worldPos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	// transform the normal
	output.normal = mul(normal, model).xyz;

	// for specular light
	output.eyeRay = (vecEye - worldPos).xyz;
	output.light = float3(1.0f, 0.0f, 0.0f);

	return output;
}

//------------------------------------------------------------------------------