// NOTE: These names must match D3D11_INPUT_ELEMENT_DESC array.
struct VS_INPUT
{
    float2 pos   : POSITION;
    float3 color : COLOR;
    float2 uv    : TEXCOORD;
};

// NOTE: These names do not matter, except SV_... ones.
struct PS_INPUT
{
    float4 pos   : SV_POSITION;
    float4 color : COLOR;
    float2 uv    : TEXCOORD;
};

// NOTE: b0 = constant buffer bound to slot 0.
cbuffer cbuffer0 : register(b0)
{
    float4 u_screen; // NOTE: x, y -> screen size
}

// NOTE: s0 = sampler bound to slot 0.
sampler sampler0 : register(s0);

// NOTE: t0 = shader resource bound to slot 0.
Texture2D<float4> texture0 : register(t0);

// NOTE: t1 = shader resource bound to slot 1.
Texture2D<float4> texture1 : register(t1);

PS_INPUT vs(VS_INPUT input)
{
    PS_INPUT output;
    // float2 a = float2(640, 480);
    // output.pos = float4(2.0f * (input.pos / a) - 1.0f, 0.0f, 1.0f);
    output.pos = float4((2.0f * u_screen.xy * input.pos) - 1.0f, 0.0f, 1.0f);
    output.uv = input.uv;
    output.color = float4(clamp(1.0f / 255.0f * input.color, 0.0f, 1.0f), 1.0f);
    return output;
}

float4 ps(PS_INPUT input) : SV_TARGET
{
    float4 tex = texture0.Sample(sampler0, input.uv);
    return input.color;
}
