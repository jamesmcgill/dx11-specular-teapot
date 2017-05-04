cbuffer StaticBuffer : register(b0)
{
	float4 modelColor;
	float4 ambientIC;
	float4 diffuseIC;
	float4 specularIC;
	float4 lightDir;
};

//------------------------------------------------------------------------------
cbuffer DynamicBuffer : register(b1)
{
	matrix model;
	matrix view;
	matrix projection;
	matrix worldInverseTranspose;
	float4 eyePos;
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

	// transform the normal
	output.normal = mul(normal, model).xyz;

	// for specular light
	output.eyeRay = (eyePos - worldPos).xyz;

	return output;
}

//------------------------------------------------------------------------------