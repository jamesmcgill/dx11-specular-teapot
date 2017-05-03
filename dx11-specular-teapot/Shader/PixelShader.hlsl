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
// A pass-through function for the (interpolated) color data. 
//------------------------------------------------------------------------------
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 color = input.color;
	//float3 lightDir = input.light;
	float3 lightDir = float3(0.0f, 0.0f, 1.0f);
	float3 viewDir = normalize(input.eyeRay);

	float Aintensity = 0.3f;
	float4 Acolor = float4(1.0f, 0.9f, 0.8f, 1.0);

	float3 normal = normalize(input.normal);
	float diffuseComp = dot(normal, lightDir);
	float Dintensity = 0.7f * saturate(diffuseComp);

	// R = 2(n.l)n -l
	float3 reflect = normalize(2 * diffuseComp * normal - lightDir);

	// Spec = R.V
	float specular = 
		pow(
		saturate(dot(reflect, viewDir))
		, 32);

	float4 dColor = float4(1.0f, 1.0f, 1.0f, 0.0f);

	return (Aintensity * Acolor) + (Dintensity * dColor) + (specular * vSpecIC);
}

//------------------------------------------------------------------------------