// Adapted from the following source: COMP2501A Tutorial 5 //
// Modified to incorporate transparency, as described at http://www.rastertek.com/dx11tut26.html
// (Rastertek DirectX 11 Tutorial 26: Transparency)

////////////////////////////////////////////////////////////////////////////////
// Filename: color.ps
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
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
    float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
	// Adjust the alpha value of this pixel by the blending amount to create the alpha blending effect.
    input.color.w *= blendAmount;
    return input.color;
}
