#pragma once

#include "Precompiled.hpp"
#include "Game/Object.hpp"

class Tank : public Object
{
    TYPE_DECLARE(Tank, Object);

public:
    Tank();
    ~Tank();

    void shootProjectile();
    void setMovementInput(sf::Vector2f movement);

    void onUpdate(float timeDelta) override;
    void onDraw(float updateAlpha) override;

protected:
    // Serialization methods.
    virtual bool onSerialize(MemoryBuffer& buffer) override;
    virtual bool onDeserialize(MemoryBuffer& buffer) override;

private:
    // Tank movement input.
    sf::Vector2f m_movementInput;

    // Tank sprite texture.
    std::shared_ptr<sf::Texture> m_texture;
};
