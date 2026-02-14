/*==============================================================================

   ビルボード描画用頂点シェーダー [shader_vertex_billboard.hlsl]
														 Author : Tanaka Kouki
														 Date   : 2025/11/14
--------------------------------------------------------------------------------

==============================================================================*/
cbuffer VS_CONSTANT_BUFFER0 : register(b0)
{
    float4x4 world;
};

cbuffer VS_CONSTANT_BUFFER1 : register(b1)
{
    float4x4 view;
};

cbuffer VS_CONSTANT_BUFFER2 : register(b2)
{
    float4x4 projection;
};

cbuffer VS_CONSTANT_BUFFER3 : register(b6)
{
    float2 uvScale;
    float2 uvTranslation;
};

struct VS_IN
{
    float3 posL : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

VS_OUT main(VS_IN vi)
{
    VS_OUT vo;

    float4 posW = mul(float4(vi.posL, 1.0f), world);
    float4 posV = mul(posW, view);
    vo.posH = mul(posV, projection);

    vo.color = vi.color;
    vo.uv = vi.uv * uvScale + uvTranslation;

    return vo;
}
