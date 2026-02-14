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
    // Keep vertex color multiply for intended tinting, but avoid full black
    // when input color is unexpectedly zeroed (e.g. VS/CSO mismatch).
    float4 vtxColor = pi.color;
    if (dot(abs(vtxColor), 1.0f.xxxx) < 1e-5f)
    {
        vtxColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    float4 c = tex.Sample(samp, pi.uv) * vtxColor * mulColor;

    // Alpha cutout (for PNG etc.)
    clip(c.a - 0.1f);

    return c;
}