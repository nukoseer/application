// NOTE: These names do not matter, except SV_... ones.
struct PS_INPUT
{
    float4 pos   : SV_POSITION;
    float2 uv    : TEXCOORD;
    float4 color : COLOR;
};

// NOTE: s0 = sampler bound to slot 0.
sampler sampler0 : register(s0);

// NOTE: t0 = shader resource bound to slot 0.
Texture2D<float4> texture0 : register(t0);

// NOTE: t1 = shader resource bound to slot 1.
Texture2D<float4> texture1 : register(t1);

float4 ps(PS_INPUT input) : SV_TARGET
{
    float4 tex = float4(1.0f, 1.0f, 1.0f, 1.0f);

    if (input.uv.x != -1.0f && input.uv.y != -1.0f)
    {
        tex = texture0.Sample(sampler0, input.uv);
    }

    return input.color * tex;
}
