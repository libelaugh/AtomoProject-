#include "particle_emitter.h"

#include "billboard.h"
#include "texture.h"

#include <algorithm>
#include <cmath>
#include <random>

using namespace DirectX;

namespace
{
    std::mt19937& Rng()
    {
        static std::random_device rd;
        static std::mt19937 mt(rd());
        return mt;
    }

    float RandomRange(float minValue, float maxValue)
    {
        std::uniform_real_distribution<float> dist(minValue, maxValue);
        return dist(Rng());
    }
}

Particle::Particle(const XMFLOAT3& position,
    const XMFLOAT3& velocity,
    float lifeSeconds,
    float startScale,
    float endScale,
    float gravity,
    const XMFLOAT4& color)
    : m_position(position)
    , m_velocity(velocity)
    , m_lifeSeconds(lifeSeconds)
    , m_startScale(startScale)
    , m_endScale(endScale)
    , m_gravity(gravity)
    , m_color(color)
{

}

bool Particle::Update(float deltaTime)
{
    m_age += deltaTime;
    if (m_age >= m_lifeSeconds)
        return false;

    m_velocity.y += m_gravity * deltaTime;

    m_position.x += m_velocity.x * deltaTime;
    m_position.y += m_velocity.y * deltaTime;
    m_position.z += m_velocity.z * deltaTime;

    return true;
}

void Particle::Draw(int texId) const
{
    const float t = (m_lifeSeconds <= 0.0f) ? 1.0f : (m_age / m_lifeSeconds);
    const float scale = m_startScale + (m_endScale - m_startScale) * t;
    Billboard_Draw(texId, m_position, scale, scale, m_color);
}

Emitter::Emitter(const XMFLOAT3& origin,
    int emitCount,
    float speedMin,
    float speedMax,
    float lifeMin,
    float lifeMax,
    float startScale,
    float endScale,
    float gravity,
    const XMFLOAT4& color,
    Pattern pattern)
    : m_origin(origin)
    , m_emitCount(emitCount)
    , m_speedMin(speedMin)
    , m_speedMax(speedMax)
    , m_lifeMin(lifeMin)
    , m_lifeMax(lifeMax)
    , m_startScale(startScale)
    , m_endScale(endScale)
    , m_gravity(gravity)
    , m_color(color)
    , m_pattern(pattern)
{
    m_particles.reserve(static_cast<size_t>(std::max(0, emitCount)));

    for (int i = 0; i < m_emitCount; ++i)
    {
        Emit();
    }
}

void Emitter::Update(float deltaTime)
{
    m_particles.erase(
        std::remove_if(
            m_particles.begin(),
            m_particles.end(),
            [deltaTime](Particle& particle)
            {
                return !particle.Update(deltaTime);
            }),
        m_particles.end());
}

void Emitter::Draw(int texId) const
{
    for (const auto& particle : m_particles)
    {
        particle.Draw(texId);
    }
}

bool Emitter::IsAlive() const
{
    return !m_particles.empty();
}

void Emitter::Emit()
{
    constexpr float kPi = 3.1415926535f;

    const float azimuth = RandomRange(0.0f, kPi * 2.0f);
    float elevation = RandomRange(0.2f, 1.2f);

    if (m_pattern == Pattern::Ring)
    {
        // 円状の爆発に見えるよう、水平に近いベクトルへ寄せる
        elevation = RandomRange(-0.08f, 0.08f);
    }

    const float speed = RandomRange(m_speedMin, m_speedMax);

    XMFLOAT3 velocity;
    velocity.x = std::cos(azimuth) * std::cos(elevation) * speed;
    velocity.y = std::sin(elevation) * speed;
    velocity.z = std::sin(azimuth) * std::cos(elevation) * speed;

    const float life = RandomRange(m_lifeMin, m_lifeMax);

    m_particles.emplace_back(m_origin, velocity, life, m_startScale, m_endScale, m_gravity, m_color);
}

void EmitterManager::Initialize(const wchar_t* particleTexturePath)
{
    m_emitters.clear();
    m_texId = Texture_Load(particleTexturePath);
}

void EmitterManager::Finalize()
{
    m_emitters.clear();
    m_texId = -1;
}

void EmitterManager::SpawnBurst(const XMFLOAT3& position, int emitCount, const XMFLOAT4& color)
{
        m_emitters.emplace_back(position, 
            emitCount, 
            1.0f, 
        5.0f,
        0.35f,
        0.9f,
        0.5f,
        0.05f,
        -9.8f,
        color,
            Emitter::Pattern::Dome);
}

void EmitterManager::Update(float deltaTime)
{
    for (auto& emitter : m_emitters)
    {
        emitter.Update(deltaTime);
    }

    m_emitters.erase(
        std::remove_if(
            m_emitters.begin(),
            m_emitters.end(),
            [](const Emitter& emitter)
            {
                return !emitter.IsAlive();
            }),
        m_emitters.end());
}

void EmitterManager::Draw() const
{
    if (m_texId < 0)
        return;

    for (const auto& emitter : m_emitters)
    {
        emitter.Draw(m_texId);
    }
}