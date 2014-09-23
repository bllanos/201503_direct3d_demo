// Adapted from the following source: COMP2501A Tutorial 5 //
// Modified to incorporate transparency, as described at http://www.rastertek.com/dx11tut26.html
// (Rastertek DirectX 11 Tutorial 26: Transparency)

////////////////////////////////////////////////////////////////////////////////
// Filename: texture.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;

cbuffer TransparentBuffer
{
    float blendAmount;
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;


    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

	// Adjust the alpha value of this pixel by the blending amount to create the alpha blending effect.
    textureColor.a *= blendAmount;

    return textureColor;
}
