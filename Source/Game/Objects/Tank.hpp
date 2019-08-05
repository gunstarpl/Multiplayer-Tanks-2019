#pragma once

#include "Precompiled.hpp"
#include "Network/Replication/Replicable.hpp"

class Tank : public Replicable
{
    TYPE_DECLARE(Tank, Replicable);

public:
    Tank();
    ~Tank();

    void shootProjectile();
    void setMovementInput(sf::Vector2f movement);

    void onTick(float timeDelta) override;
    void onDraw(float timeAlpha) override;

    sf::Vector2f getPosition(float alpha = 1.0f);

protected:
    // Serialization methods.
    virtual bool onSerialize(MemoryStream& buffer) const override;
    virtual bool onDeserialize(MemoryStream& buffer) override;

private:
    // Tank movement input.
    sf::Vector2f m_movementInput;

    // Tank sprite texture.
    std::shared_ptr<sf::Texture> m_texture;

    // Tank sprite.
    sf::RectangleShape m_tankSprite;
};
