cbuffer CB : register(b0)
{
    row_major float4x4 g_Transform;
};

Texture2D t_Texture : register(t0);
SamplerState s_Sampler : register(s0);

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT main_vs(float3 i_pos : POSITION
				 ,float2 i_uv : UV)
{
    VS_OUTPUT output;
    output.position = mul(float4(i_pos, 1), g_Transform);
    output.uv = i_uv;
    return output;
}

float4 main_ps(VS_OUTPUT input) : SV_Target
{
    return t_Texture.Sample(s_Sampler, input.uv);
    //return float4(1.0f, 0.22f, 0.0f, 1.0f);
}