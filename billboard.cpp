/*==============================================================================

　　　ビルボード描画[billboard.h]
														 Author : Tanaka Kouki
														 Date   : 2025/11/14
--------------------------------------------------------------------------------

==============================================================================*/

#include "billboard.h"

#include "direct3d.h"
#include "texture.h"
#include "shader_billboard.h"

#include <d3d11.h>

using namespace DirectX;

namespace
{
    struct VertexBillboard
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
        XMFLOAT2 texcoord;
    };

    constexpr UINT kVertexCount = 4;
    constexpr UINT kIndexCount = 6;

    ID3D11Buffer* g_pVertexBuffer = nullptr;
    ID3D11Buffer* g_pIndexBuffer = nullptr;

    // Billboard dedicated render states (so other draw calls won't accidentally break it)
    ID3D11BlendState* g_pBlendAlpha = nullptr; // SrcAlpha / InvSrcAlpha
    ID3D11DepthStencilState* g_pDepthReadOnly = nullptr; // Depth test ON, depth write OFF
    ID3D11RasterizerState* g_pCullNone = nullptr; // No culling (billboard can face any way)

    // View matrix with translation cleared (set by Billboard_SetViewMatrix)
    XMFLOAT4X4 g_mtxView{};

    struct RenderStateGuard
    {
        ID3D11DeviceContext* ctx = nullptr;

        ID3D11BlendState* prevBlend = nullptr;
        FLOAT                   prevBlendFactor[4]{};
        UINT                    prevSampleMask = 0xffffffff;

        ID3D11DepthStencilState* prevDS = nullptr;
        UINT                     prevStencilRef = 0;

        ID3D11RasterizerState* prevRS = nullptr;

        explicit RenderStateGuard(ID3D11DeviceContext* c) : ctx(c)
        {
            if (!ctx) return;
            ctx->OMGetBlendState(&prevBlend, prevBlendFactor, &prevSampleMask);
            ctx->OMGetDepthStencilState(&prevDS, &prevStencilRef);
            ctx->RSGetState(&prevRS);
        }

        void ApplyBillboardStates()
        {
            if (!ctx) return;

            const FLOAT blendFactor[4] = { 0, 0, 0, 0 };
            ctx->OMSetBlendState(g_pBlendAlpha, blendFactor, 0xffffffff);
            ctx->OMSetDepthStencilState(g_pDepthReadOnly, 0);
            ctx->RSSetState(g_pCullNone);
        }

        ~RenderStateGuard()
        {
            if (!ctx) return;
            ctx->OMSetBlendState(prevBlend, prevBlendFactor, prevSampleMask);
            ctx->OMSetDepthStencilState(prevDS, prevStencilRef);
            ctx->RSSetState(prevRS);
            SAFE_RELEASE(prevBlend);
            SAFE_RELEASE(prevDS);
            SAFE_RELEASE(prevRS);
        }
    };
}

void Billboard_Initialize()
{
    ShaderBillboard_Initialize();

    // Unit quad centered at origin (Z=0)
    static const VertexBillboard kVerts[kVertexCount] = {
        { { -0.5f,  0.5f, 0.0f }, { 1,1,1,1 }, { 0.0f, 0.0f } }, // 0
        { {  0.5f,  0.5f, 0.0f }, { 1,1,1,1 }, { 1.0f, 0.0f } }, // 1
        { {  0.5f, -0.5f, 0.0f }, { 1,1,1,1 }, { 1.0f, 1.0f } }, // 2
        { { -0.5f, -0.5f, 0.0f }, { 1,1,1,1 }, { 0.0f, 1.0f } }, // 3
    };

    static const uint16_t kIndices[kIndexCount] = {
        0, 1, 2,
        0, 2, 3,
    };

    // Vertex buffer
    {
        D3D11_BUFFER_DESC bd{};
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = sizeof(VertexBillboard) * kVertexCount;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA sd{};
        sd.pSysMem = kVerts;

        Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &g_pVertexBuffer);
    }

    // Index buffer
    {
        D3D11_BUFFER_DESC bd{};
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = sizeof(uint16_t) * kIndexCount;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA sd{};
        sd.pSysMem = kIndices;

        Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &g_pIndexBuffer);
    }

    // --- Render states (billboard-safe defaults) ---
    {
        // Alpha blend
        D3D11_BLEND_DESC bd{};
        bd.AlphaToCoverageEnable = FALSE;
        bd.IndependentBlendEnable = FALSE;
        auto& rt = bd.RenderTarget[0];
        rt.BlendEnable = TRUE;
        rt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
        rt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        rt.BlendOp = D3D11_BLEND_OP_ADD;
        rt.SrcBlendAlpha = D3D11_BLEND_ONE;
        rt.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        Direct3D_GetDevice()->CreateBlendState(&bd, &g_pBlendAlpha);

        // Depth read-only (use depth test but don't write, typical for transparent)
        D3D11_DEPTH_STENCIL_DESC dsd{};
        dsd.DepthEnable = TRUE;
        dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        dsd.StencilEnable = FALSE;
        Direct3D_GetDevice()->CreateDepthStencilState(&dsd, &g_pDepthReadOnly);

        // No culling (quad should be visible from either side)
        D3D11_RASTERIZER_DESC rd{};
        rd.FillMode = D3D11_FILL_SOLID;
        rd.CullMode = D3D11_CULL_NONE;
        rd.DepthClipEnable = TRUE;
        Direct3D_GetDevice()->CreateRasterizerState(&rd, &g_pCullNone);
    }
}

void Billboard_Finalize()
{
    ShaderBillboard_Finalize();
    SAFE_RELEASE(g_pCullNone);
    SAFE_RELEASE(g_pDepthReadOnly);
    SAFE_RELEASE(g_pBlendAlpha);
    SAFE_RELEASE(g_pIndexBuffer);
    SAFE_RELEASE(g_pVertexBuffer);
}

static void Billboard_DrawInternal(
    int texId,
    const XMFLOAT3& position,
    const XMFLOAT2& scale,
    const UVParameter& uv,
    const XMFLOAT4& color,
    const XMFLOAT2& pivot)
{
    if (texId < 0 || Texture_Width(texId) == 0 || Texture_Height(texId) == 0)
    {
        // Skip invalid / not-loaded textures
        return;
    }

    ShaderBillboard_SetUVParameter(uv);
    ShaderBillboard_SetColor(color);
    ShaderBillboard_Begin();

    auto* ctx = Direct3D_GetContext();

    // Make billboard robust against render-state side effects from other draw calls.
    RenderStateGuard state(ctx);
    state.ApplyBillboardStates();

    // VB/IB
    {
        UINT stride = sizeof(VertexBillboard);
        UINT offset = 0;
        ctx->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
        ctx->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    // Texture
    Texture_SetTexture(texId);

    // Build billboard rotation from view (view with translation cleared)
    // viewNoTrans is R^T, so transpose => R (camera rotation)
    XMMATRIX billboardRot = XMMatrixTranspose(XMLoadFloat4x4(&g_mtxView));

    // Pivot offset in local space (NOTE: pivot is in the same unit as vertex positions: -0.5..0.5)
    XMMATRIX mPivot = XMMatrixTranslation(-pivot.x, -pivot.y, 0.0f);
    XMMATRIX mScale = XMMatrixScaling(scale.x, scale.y, 1.0f);
    XMMATRIX mTrans = XMMatrixTranslation(position.x, position.y, position.z);

    // Row-vector convention: v * (pivot -> scale -> rot -> trans)
    XMMATRIX world = mPivot * mScale * billboardRot * mTrans;
    ShaderBillboard_SetWorldMatrix(world);

    ctx->DrawIndexed(kIndexCount, 0, 0);
}

void Billboard_Draw(int texId, const XMFLOAT3& position, float scaleX, float scaleY, const XMFLOAT2& pivot)
{
    Billboard_DrawInternal(
        texId,
        position,
        { scaleX, scaleY },
        { { 1.0f, 1.0f }, { 0.0f, 0.0f } },
        { 1.0f, 1.0f, 1.0f, 1.0f },
        pivot);
}

void Billboard_Draw(
    int texId,
    const XMFLOAT3& position,
    const XMFLOAT2& scale,
    const XMUINT4& tex_cut,
    const XMFLOAT4& color,
    const XMFLOAT2& pivot)
{
    // tex_cut: (x, y, w, h) in pixels
    const float invW = 1.0f / (float)Texture_Width(texId);
    const float invH = 1.0f / (float)Texture_Height(texId);

    const float uvX = (float)tex_cut.x * invW;
    const float uvY = (float)tex_cut.y * invH;
    const float uvW = (float)tex_cut.z * invW;
    const float uvH = (float)tex_cut.w * invH;

    Billboard_DrawInternal(
        texId,
        position,
        scale,
        { { uvW, uvH }, { uvX, uvY } },
        color,
        pivot);
}

void Billboard_SetViewMatrix(const XMFLOAT4X4& view)
{
    // Clear translation: rotation-only view matrix (R^T)
    g_mtxView = view;
    g_mtxView._41 = 0.0f;
    g_mtxView._42 = 0.0f;
    g_mtxView._43 = 0.0f;
}
