/*==============================================================================

	ÉSÅ[Éã[goal.h]
														 Author : Tanaka Kouki
														 Date   : 2026/01/13
--------------------------------------------------------------------------------

==============================================================================*/


#ifndef GOAL_H
#define GOAL_H

#include <DirectXMath.h>

enum class GoalState
{
    Inactive,
    Active,
    Clear,
    WaitInput,
};

void Goal_Init();
void Goal_Uninit();
void Goal_Update(double elapsedTime);

// 3D (goal model) draw. Call this during your 3D pass.
void Goal_Draw3D();

// 2D (stage clear UI) draw. Call this at the end of the frame.
void Goal_DrawUI();

// Backward compatible wrapper (currently draws UI only).
void Goal_Draw();

void Goal_SetPosition(const DirectX::XMFLOAT3& pos);
DirectX::XMFLOAT3 Goal_GetPosition();
GoalState Goal_GetState();

#endif // GOAL_H