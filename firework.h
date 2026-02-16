#ifndef FIREWORK_H
#define FIREWORK_H

#include "particle_emitter.h"

#include <DirectXMath.h>
#include <vector>

class Firework
{
public:
    void Initialize(const wchar_t* particleTexturePath);
    void Finalize();

    void Launch(const DirectX::XMFLOAT3& startPosition,
        const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });
    void Update(float deltaTime);
    void Draw() const;

private:
    struct Shell
    {
        DirectX::XMFLOAT3 position{};
        DirectX::XMFLOAT3 velocity{};
        float fuse = 1.0f;
        float thrustTime = 0.0f;
        float thrustAcceleration = 0.0f;
        float trailTimer = 0.0f;
        int burstCount = 40;
        DirectX::XMFLOAT4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
    };

    EmitterManager m_emitterManager;
    std::vector<Shell> m_shells;
};

#endif // FIREWORK_H