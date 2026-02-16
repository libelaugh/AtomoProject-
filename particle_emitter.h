#ifndef PARTICLE_EMITTER_H
#define PARTICLE_EMITTER_H

#include <DirectXMath.h>
#include <vector>

class Particle
{
public:
    Particle(const DirectX::XMFLOAT3& position,
        const DirectX::XMFLOAT3& velocity,
        float lifeSeconds,
        float startScale,
        float endScale,
        float gravity,
        const DirectX::XMFLOAT4& color);

    bool Update(float deltaTime);
    void Draw(int texId) const;

private:
    DirectX::XMFLOAT3 m_position{};
    DirectX::XMFLOAT3 m_velocity{};
    float m_age = 0.0f;
    float m_lifeSeconds = 1.0f;
    float m_startScale = 1.0f;
    float m_endScale = 0.0f;
    float m_gravity = 0.0f;
    DirectX::XMFLOAT4 m_color{ 1.0f, 1.0f, 1.0f, 1.0f };
};

class Emitter
{
public:
    enum class Pattern
    {
        Dome,
        Ring,
    };

    Emitter(const DirectX::XMFLOAT3& origin,
        int emitCount,
        float speedMin,
        float speedMax,
        float lifeMin,
        float lifeMax,
        float startScale,
        float endScale,
        float gravity,
        const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
        Pattern pattern = Pattern::Dome);

    void Update(float deltaTime);
    void Draw(int texId) const;
    bool IsAlive() const;

private:
    void Emit();

    DirectX::XMFLOAT3 m_origin{};
    std::vector<Particle> m_particles;

    int m_emitCount = 0;
    float m_speedMin = 0.0f;
    float m_speedMax = 0.0f;
    float m_lifeMin = 0.0f;
    float m_lifeMax = 0.0f;
    float m_startScale = 1.0f;
    float m_endScale = 0.0f;
    float m_gravity = 0.0f;
    DirectX::XMFLOAT4 m_color{ 1.0f, 1.0f, 1.0f, 1.0f };
    Pattern m_pattern = Pattern::Dome;
};

class EmitterManager
{
public:
    void Initialize(const wchar_t* particleTexturePath);
    void Finalize();

    void SpawnBurst(const DirectX::XMFLOAT3& position,
        int emitCount = 24,
        const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });
    void SpawnRingBurst(const DirectX::XMFLOAT3& position, int emitCount = 40,
        const DirectX::XMFLOAT4 & color = { 1.0f, 1.0f, 1.0f, 1.0f });
    void Update(float deltaTime);
    void Draw() const;

private:
    int m_texId = -1;
    std::vector<Emitter> m_emitters;
};

#endif // PARTICLE_EMITTER_H