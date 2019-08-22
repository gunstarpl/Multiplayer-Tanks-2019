#include "Precompiled.hpp"
#include "Player.hpp"
#include "PlayerControllerBase.hpp"

Player::Player()
{
}
 
Player::~Player()
{
}

void Player::setPlayerHandle(PlayerHandle handle)
{
    ASSERT(!m_playerHandle.isValid(), "Player handle has already been set before!");
    m_playerHandle = handle;
}

void Player::setPlayerController(std::unique_ptr<PlayerControllerBase>&& playerController)
{
    m_playerController = std::move(playerController);
}

bool Player::onSerialize(MemoryStream& stream) const
{
    return true;
}

bool Player::onDeserialize(MemoryStream& stream)
{
    return true;
}

PlayerHandle Player::getPlayerHandle() const
{
    ASSERT(m_playerHandle.isValid(), "Player handle should already be set!");
    return m_playerHandle;
}

PlayerControllerBase* Player::getPlayerController()
{
    return m_playerController.get();
}