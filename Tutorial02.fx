
Texture2D ObjTexture;
SamplerState ObjSamplerState
{
	Filter = MIN_MAG_MIP_LINEAR;
};

struct Light
{	
	float3 pos;
	float range;
	float3 att;
	float3 dir;
	float cone;
	float4 ambient;
	float4 diffuse;
	float4 specular;	
	float specularPower;				
};


cbuffer cbPerFrame
{
	Light light;	
};


cbuffer cbPerObject
{
	float4x4 WVP;
	float4x4 World;
	float3 cameraPos;
    bool lightAffected; 

	bool hasSpecular;
};


struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
	bool light_affected : BOOL;
	bool has_specular: BOOL1;
};


VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
    VS_OUTPUT output;

	output.has_specular = hasSpecular;
	output.light_affected = lightAffected;
    output.Pos = mul(inPos, WVP);	
    output.worldPos = mul(inPos, World);
	output.normal = mul(normal, (float3x3)World);
    output.TexCoord = inTexCoord;

    // Calculate the position of the vertex in the world.
    float4 worldPosition = mul(inPos, World);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world
    output.viewDirection = normalize(cameraPos - worldPosition);
	  
    output.viewDirection = normalize(output.viewDirection);
	
	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float3 reflection;
    float4 specular;

	input.normal = normalize(input.normal);
	
    float4 tex = ObjTexture.Sample(ObjSamplerState, input.TexCoord);

	clip(tex.a - 0.25f);

	float3 finalColor = float3(0.0f, 0.0f, 0.0f);
	
    //Create ambient
	float3 finalAmbient = tex * light.ambient;
	
	//Create the vector between light position and pixels position
	float3 lightToPixelVec = light.pos - input.worldPos;
		
	//Find the distance between the light pos and pixel pos
	float distance = length(lightToPixelVec);
	
	//If pixel is too far, return
	if( distance > light.range )
		return float4(finalAmbient, tex.a);
		

	if (input.has_specular == false)
	{
		return float4(finalAmbient, tex.a);
	}

    if (input.light_affected == true)
	{
		//Turn lightToPixelVec into a unit length vector describing the pixels direction from the lights position
		lightToPixelVec /= distance; 
	
		//Calculate how much light the pixel gets by the angle in which the light strikes the pixels surfaecAXz
		float howMuchLight = dot(input.normal, lightToPixelVec);
	
		specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
		reflection = normalize(2 * input.normal - lightToPixelVec); 
		specular = pow(saturate(dot(reflection, input.viewDirection)), light.specularPower);
	
		//If light is striking the front side of the pixel
		if( howMuchLight > 0.0f )
		{	
      		finalColor += howMuchLight * tex * light.diffuse + specular;		

		    // falloff factor
			finalColor /= light.att[0] + (light.att[1] * distance) + (light.att[2] * (distance*distance));
	
		}	
	}
        

	//return float4(input.normal, tex.a);

	finalColor = saturate(finalColor + finalAmbient);
	
	return float4(finalColor, tex.a);
}

