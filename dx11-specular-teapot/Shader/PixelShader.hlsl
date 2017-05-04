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
// Per-pixel color data passed through the pixel shader.
//------------------------------------------------------------------------------
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float3 eyeRay: TEXCOORD2;
};

//------------------------------------------------------------------------------
// A pass-through function for the (interpolated) color data. 
//------------------------------------------------------------------------------
float4 main(PixelShaderInput input) : SV_TARGET
{
	float3 lightRay = normalize(-lightDir.xyz);
	float3 viewDir = normalize(input.eyeRay);

	float3 normal = normalize(input.normal);
	float cosLight = dot(normal, lightRay);
	float diffuse = saturate(cosLight);

	// R = 2(n.l)n -l
	float3 reflect = normalize(2 * cosLight * normal - lightRay);

	// Spec = R.V
	float specular = 
		pow(
		saturate(dot(reflect, viewDir))
		, 32);

	return (ambientIC) + (diffuse * diffuseIC) + (specular * specularIC);
}

//------------------------------------------------------------------------------