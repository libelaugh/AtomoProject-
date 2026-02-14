/*==============================================================================

   ビルボード描画用ピクセルシェーダー [shader_pixel_billboard.hlsl]
														 Author : Tanaka Kouki
														 Date   : 2025/11/14
--------------------------------------------------------------------------------

==============================================================================*/

cbuffer PS_CONSTANT_BUFFER0 : register(b0)
{
    float4 mulColor;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

struct PS_IN
{
    float4 posH : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

float4 main(PS_IN pi) : SV_TARGET
{
    float4 c = tex.Sample(samp, pi.uv) * pi.color * mulColor;

    // Alpha cutout (for PNG etc.)
    clip(c.a - 0.1f);

    return c;
}
