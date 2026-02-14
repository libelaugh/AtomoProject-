/*==============================================================================

   3D描画用ピクセルシェーダー [shader_pixel_3d.hlsl]
														 Author : Tanaka Kouki
														 Date   : 2025/09/11
--------------------------------------------------------------------------------

==============================================================================*/
cbuffer PS_CONSTANT_DIFFUSE : register(b0)
{
    float4 diffuse_color;
    float4 clipParams; // x = clipEnable (0/1), y..w unused
};
struct PS_IN
{
    float4 posH : SV_POSITION;
    float2 uv : TEXCOORD0;
    float posLy : TEXCOORD1;
};

Texture2D tex; // テクスチャ
SamplerState samp; // テクスチャサンプラ

float4 main(PS_IN pi) : SV_TARGET
{
    if (clipParams.x > 0.5f)
    {
        if (pi.posLy > 0.0f)
            discard; // 上半分を描かない（＝下半分だけ）

    }
    
    return tex.Sample(samp, pi.uv) * diffuse_color;
}
