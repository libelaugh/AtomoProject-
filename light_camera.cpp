/*==============================================================================

   ライトのカメラ(深度バッフアシャドウの) [light_camera.cpp]
														 Author : Tanaka Kouki
														 Date   : 2025/12/10
--------------------------------------------------------------------------------

==============================================================================*/

#include "light_camera.h"

using namespace DirectX;

static XMFLOAT3 g_Position{};
static XMFLOAT3 g_Front{ 0.0f,-1.0f,0.0f };

static XMFLOAT4X4 g_View{};
static XMFLOAT4X4 g_Proj{};

void LightCamera_Initialize(const DirectX::XMFLOAT3& worldDirectional, const DirectX::XMFLOAT3& position)
{
	g_Front = worldDirectional;
	g_Position = position;
}

void LightCamera_Finalize()
{
}

void LightCamera_SetPosition(const DirectX::XMFLOAT3& position)
{
	g_Position = position;
}

void LightCamera_SetFront(const DirectX::XMFLOAT3& front)
{
	g_Front = front;
}

const DirectX::XMFLOAT4X4& LightCamera_GetViewMatrix()
{
	XMFLOAT4X4 out{};

	XMVECTOR eye = XMLoadFloat3(&g_Position);
	XMVECTOR dir = XMVector3Normalize(XMLoadFloat3(&g_Front));
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);

	XMMATRIX view = XMMatrixLookToLH(eye, dir, up);
	XMStoreFloat4x4(&out, view);
	return out;
	/*XMFLOAT4X4 mtxView;

	XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&g_Position), XMVECTOR{ 0.0f,-1.0f,0.0f }, XMLoadFloat3(&g_Front));

	XMStoreFloat4x4(&mtxView, view);

	return mtxView;*/
}

const DirectX::XMFLOAT4X4& LightCamera_GetProjectionMatrix()
{
	XMFLOAT4X4 out{};

	float value = 40.0f;
	XMMATRIX proj = XMMatrixOrthographicOffCenterLH(-value, value, -value, value, 0.1f, 1000.0f);
	XMStoreFloat4x4(&out, proj);
	return out;
	/*XMFLOAT4X4 mtxProj;

	float value = 40;
	XMMATRIX proj = XMMatrixOrthographicOffCenterLH(-value, value,-value,value, 0.1f, 1000.0f);

	XMStoreFloat4x4(&mtxProj, proj);

	return mtxProj;*/
}
