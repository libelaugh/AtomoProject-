/*==============================================================================

　　  サンプラー設定のユーティリティー[sampler.h]
														 Author : Kouki Tanaka
														 Date   : 2025/09/18
--------------------------------------------------------------------------------

==============================================================================*/

#include "sampler.h"
#include"direct3d.h"

/*DirectXにおいてテクスチャから色を取得するための道具
サンプラーとは、DirectXにおいてテクスチャから色を取得するための道具です。
“どのピクセルを、どんな補間で、はみ出したらどう扱うか”を決めて、同じ画像でも用途ごとに最適表示にできる道具だよ。*/

/*使いどころ（ざっくり指針）

Point：ドット絵、ピクセルアート、UIをカリッと表示したい時

Linear：一般的な2D/3Dのテクスチャ表示

Anisotropic：遠景の床や斜めの面に貼ったテクスチャを高品質で見せたい時*/

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static ID3D11SamplerState* g_pSamplerFilterPoint = nullptr;
static ID3D11SamplerState* g_pSamplerFilterLinear = nullptr;
static ID3D11SamplerState* g_pSamplerFilterAnisotropic = nullptr;

void Sampler_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// 〇サンプラーステート設定
	D3D11_SAMPLER_DESC sampler_desc{};

	//フィルタリングの設定//(ミップマップもできる)//基本LINIEAR
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;// _LINEAR;

	//UV参照外の画像の処理方法の設定   //クランプモードCLAMP//ダブルラップモードWRAP//その他BORDER,MIRRORモード//（UVスクロールもできる）
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.BorderColor[0] = 0.0f;
	sampler_desc.BorderColor[1] = 0.0f;
	sampler_desc.BorderColor[2] = 0.0f;
	sampler_desc.BorderColor[3] = 0.0f;


	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerFilterPoint);

	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerFilterLinear);

	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerFilterAnisotropic);
}

void Sampler_Finalize()
{
	SAFE_RELEASE(g_pSamplerFilterAnisotropic);
	SAFE_RELEASE(g_pSamplerFilterLinear);
	SAFE_RELEASE(g_pSamplerFilterPoint);
}

void Sampler_SetFilterPoint()
{
	g_pContext->PSSetSamplers(0, 1, &g_pSamplerFilterPoint);
}

void Sampler_SetFilterLinear()
{
	g_pContext->PSSetSamplers(0, 1, &g_pSamplerFilterLinear);
}

void Sampler_SetFilterAnisotropic()
{
	g_pContext->PSSetSamplers(0, 1, &g_pSamplerFilterAnisotropic);
}
