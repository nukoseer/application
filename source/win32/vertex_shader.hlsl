// NOTE: These names must match D3D11_INPUT_ELEMENT_DESC array.
struct VS_INPUT
{
    float2 pos   : POSITION;
    float2 uv    : TEXCOORD;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 pos   : SV_POSITION;
    float2 uv    : TEXCOORD;
    float4 color : COLOR;
};

// NOTE: b0 = constant buffer bound to slot 0.
cbuffer cbuffer0 : register(b0)
{
    float4 u_screen; // NOTE: x, y -> screen size
}

VS_OUTPUT vs(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = float4((2.0f * u_screen.xy * input.pos) - 1.0f, 0.0f, 1.0f);
    output.uv = input.uv;
    output.color = clamp(1.0f / 255.0f * input.color, 0.0f, 1.0f);
    
    return output;
}
