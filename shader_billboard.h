/*==============================================================================

  Billboard shader wrapper [shader_billboard.h]
  Author : Tanaka Kouki
  Date   : 2025/11/14

==============================================================================*/

#ifndef SHADER_BILLBOARD_H
#define SHADER_BILLBOARD_H

#include <DirectXMath.h>

struct UVParameter
{
    DirectX::XMFLOAT2 scale;
    DirectX::XMFLOAT2 translation;
};

bool ShaderBillboard_Initialize();
void ShaderBillboard_Finalize();

void ShaderBillboard_SetWorldMatrix(const DirectX::XMMATRIX& matrix);
void ShaderBillboard_SetViewMatrix(const DirectX::XMMATRIX& matrix);
void ShaderBillboard_SetProjectionMatrix(const DirectX::XMMATRIX& matrix);

void ShaderBillboard_SetColor(const DirectX::XMFLOAT4& color);
void ShaderBillboard_SetUVParameter(const UVParameter& parameter);

void ShaderBillboard_Begin();

#endif // SHADER_BILLBOARD_H
