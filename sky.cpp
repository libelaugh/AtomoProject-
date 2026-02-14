/*==============================================================================

　　  空の表示[sky.cpp]
														 Author : Tanaka Kouki
														 Date   : 2025/11/21
--------------------------------------------------------------------------------

==============================================================================*/

#include "sky.h"
#include "direct3d.h"
#include "model.h"
#include "shader3d_unlit.h"

using namespace DirectX;

static MODEL* g_pModelSky{ nullptr };
static ID3D11RasterizerState* g_pRasterizerStateCullNone{ nullptr };
static XMFLOAT3 g_position{};


void Sky_Initialize()
{

	g_pModelSky = ModelLoad("sky.fbx", 100.0f, true);

	ID3D11Device* device = Direct3D_GetDevice();
	if (device) {
		D3D11_RASTERIZER_DESC rd{};
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_NONE;
		rd.DepthClipEnable = TRUE;
		device->CreateRasterizerState(&rd, &g_pRasterizerStateCullNone);
	}
}

void Sky_Finalize()
{
	ModelRelease(g_pModelSky);
	SAFE_RELEASE(g_pRasterizerStateCullNone);
}

void Sky_Draw()
{
    if (!g_pModelSky) return;

    Shader3DUnlit_Begin();
    Shader3DUnlit_SetClipTopOnly(true); //上半分だけ描く

    ID3D11DeviceContext* ctx = Direct3D_GetContext();
    ID3D11RasterizerState* prevState = nullptr;
    if (ctx && g_pRasterizerStateCullNone) {
        ctx->RSGetState(&prevState);
        ctx->RSSetState(g_pRasterizerStateCullNone);
    }

    const XMMATRIX trs = XMMatrixTranslationFromVector(XMLoadFloat3(&g_position));

    //上（上半分）
    ModelUnlitDraw(g_pModelSky, trs);

    //下（上半分を180°回して下にも配置）
    const XMMATRIX rotDown = XMMatrixRotationX(XM_PI);
    ModelUnlitDraw(g_pModelSky, rotDown * trs);

    if (ctx && g_pRasterizerStateCullNone) ctx->RSSetState(prevState);
    SAFE_RELEASE(prevState);

    Shader3DUnlit_SetClipTopOnly(false); // 他のUnlitに影響させないなら戻す
}

void Sky_SetPosition(const DirectX::XMFLOAT3& position)
{
	g_position = position;
}
