#include "Precompiled.hpp"
#include "Projectile.hpp"
#include "System/AssetManager.h"

Projectile::Projectile() :
    m_remainingLifetime(2.0f)
{
    // Load projectile texture.
    m_texture = g_assetManager->loadTexture("Data/Sprites/projectile_base.png");
}

Projectile::~Projectile()
{
}

void Projectile::setPosition(const sf::Vector2f& position)
{
    m_transform.setPosition(position, true);
}

void Projectile::setDirection(const sf::Vector2f& direction)
{
    m_transform.setDirection(direction, true);
}

void Projectile::onTick(float timeDelta)
{
    Object::onTick(timeDelta);

    // Move projectile forward.
    m_transform.move(m_transform.getDirection() * 6.0f * timeDelta);

    // Destroy projectile at the end of its lifetime.
    m_remainingLifetime = std::max(0.0f, m_remainingLifetime - timeDelta);

    if(m_remainingLifetime == 0.0f)
    {
        destroy();
    }
}

void Projectile::onDraw(float timeAlpha)
{
    Object::onDraw(timeAlpha);

    // Draw projectile.
    sf::RectangleShape projectileSprite;
    projectileSprite.setSize(sf::Vector2f(0.3f, 0.3f));
    projectileSprite.setOrigin(projectileSprite.getSize() / 2.0f);
    projectileSprite.setPosition(m_transform.getPosition(timeAlpha));
    projectileSprite.setRotation(m_transform.getRotation());
    projectileSprite.setTexture(m_texture.get());

    projectileSprite.setPosition(projectileSprite.getPosition() + sf::Vector2f(0.0f, -0.05f));

    g_renderTarget->draw(projectileSprite);
}

bool Projectile::onSerialize(MemoryStream& buffer) const
{
    if(!Super::onSerialize(buffer))
        return false;

    if(!serialize(buffer, m_remainingLifetime))
        return false;

    return true;
}

bool Projectile::onDeserialize(MemoryStream& buffer)
{
    if(!Super::onDeserialize(buffer))
        return false;

    if(!deserialize(buffer, &m_remainingLifetime))
        return false;

    return true;
}

bool Projectile::serializeInitialReplication(MemoryStream& stream)
{
    if(!serialize(stream, m_transform))
        return false;

    return true;
}

bool Projectile::deserializeInitialReplication(MemoryStream& stream)
{
    if(!deserialize(stream, &m_transform))
        return false;

    return true;
}
