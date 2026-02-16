#include "firework.h"

#include <algorithm>

using namespace DirectX;

void Firework::Initialize(const wchar_t* particleTexturePath)
{
    m_shells.clear();
    m_emitterManager.Initialize(particleTexturePath);
}

void Firework::Finalize()
{
    m_shells.clear();
    m_emitterManager.Finalize();
}

void Firework::Launch(const XMFLOAT3& startPosition)
{
    Shell shell;
    shell.position = startPosition;
    shell.velocity = { 0.0f, 17.0f, 0.0f };
    shell.fuse = 1.35f;
    shell.thrustTime = 0.55f;
    shell.thrustAcceleration = 23.0f;
    shell.trailTimer = 0.0f;
    shell.burstCount = 54;
    m_shells.push_back(shell);
}

void Firework::Update(float deltaTime)
{
    constexpr float kGravity = -9.8f;
    constexpr float kTrailInterval = 0.045f;

    for (auto& shell : m_shells)
    {
        shell.fuse -= deltaTime;

        if (shell.thrustTime > 0.0f)
        {
            shell.velocity.y += shell.thrustAcceleration * deltaTime;
            shell.thrustTime -= deltaTime;
        }

        shell.velocity.y += kGravity * deltaTime;

        shell.position.x += shell.velocity.x * deltaTime;
        shell.position.y += shell.velocity.y * deltaTime;
        shell.position.z += shell.velocity.z * deltaTime;

        shell.trailTimer += deltaTime;
        if (shell.trailTimer >= kTrailInterval)
        {
            shell.trailTimer = 0.0f;
            m_emitterManager.SpawnBurst(shell.position, 2);
        }
    }

    for (const auto& shell : m_shells)
    {
        if (shell.fuse <= 0.0f)
        {
            m_emitterManager.SpawnRingBurst(shell.position, shell.burstCount);
            m_emitterManager.SpawnBurst(shell.position, shell.burstCount / 3);
        }
    }

    m_shells.erase(
        std::remove_if(
            m_shells.begin(),
            m_shells.end(),
            [](const Shell& shell)
            {
                return shell.fuse <= 0.0f;
            }),
        m_shells.end());

    m_emitterManager.Update(deltaTime);
}

void Firework::Draw() const
{
    m_emitterManager.Draw();
}