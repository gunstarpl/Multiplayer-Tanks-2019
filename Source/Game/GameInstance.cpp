#include "Precompiled.hpp"
#include "Game/GameInstance.hpp"
#include "Game/World/World.hpp"
#include "Game/Level.hpp"
#include "Game/Objects/Tank.hpp"
#include "Game/Player/PlayerManager.hpp"
#include "Game/Player/PlayerControllerHuman.hpp"
#include "Network/Interfaces/NetworkServer.hpp"
#include "Network/Interfaces/NetworkClient.hpp"
#include "Network/Interfaces/NetworkOffline.hpp"
#include "System/Globals.hpp"
#include "System/CommandLine.hpp"
#include "System/Window.hpp"

GameInstance::GameInstance() :
    m_tickCounter(0),
    m_isViewportCentered(false),
    m_isCameraAttachedToPlayer(true)
{
}

GameInstance::~GameInstance()
{
}

bool GameInstance::initialize(const NetworkParams& networkParams)
{
    // Create world instance.
    m_world = std::make_unique<World>();
    if(!m_world->initialize())
        return false;

    // Create game level.
    m_level = std::make_unique<Level>();
    if(!m_level->initialize())
        return false;

    // Create player manager.
    m_playerManager = std::make_unique<PlayerManager>();
    if(!m_playerManager->initialize(this))
        return false;

    // Create network interface.
    // #todo: Move title rename to game sessions. Implement Window::SetTitleDecorator.
    if(networkParams.mode == NetworkMode::Offline)
    {
        // Create offline network interface.
        m_network = std::make_unique<NetworkOffline>();
    }
    if(networkParams.mode == NetworkMode::Server)
    {
        // Change window title.
        g_window->setTitle(g_window->getInitialTitle() + " - Server");

        // Change logger name.
        g_logger->setName("Server");

        // Create server network interface.
        auto networkServer = std::make_unique<NetworkServer>();
        if(!networkServer->initialize(this, networkParams.port))
            return false;

        // Set network interface.
        m_network = std::move(networkServer);
    }
    else if(networkParams.mode == NetworkMode::Client)
    {
        // Change window title.
        g_window->setTitle(g_window->getInitialTitle() + " - Client");

        // Change logger name.
        g_logger->setName("Client");

        // Create client network interface.
        auto networkClient = std::make_unique<NetworkClient>();
        if(!networkClient->initialize(this, sf::IpAddress(networkParams.address), networkParams.port))
            return false;

        // Set network interface.
        m_network = std::move(networkClient);
    }

    // Success!
    return true;
}

void GameInstance::handleEvent(const sf::Event& event)
{
    // Handle keyboard input.
    if(event.type == sf::Event::KeyPressed)
    {
        switch(event.key.code)
        {
        case sf::Keyboard::O:
            m_isCameraAttachedToPlayer = true;
            LOG_TRACE("Camera has re-attached to the player's tank.");
            break;

        case sf::Keyboard::I:
        case sf::Keyboard::K:
        case sf::Keyboard::J:
        case sf::Keyboard::L:
            if(m_isCameraAttachedToPlayer)
            {
                m_isCameraAttachedToPlayer = false;
                LOG_TRACE("Camera has been detached from the player. Press 'O' to restore camera to default.");
            }
            break;
        }
    }

    // Handle player input events.
    m_playerManager->handleEvent(event);
}

void GameInstance::update(float timeDelta)
{
    // Update network interface.
    m_network->update(timeDelta);

    // Update world instance.
    m_world->update(timeDelta);
}

void GameInstance::tick(float timeDelta)
{
    // Pre tick network interface.
    m_network->preTick(timeDelta);

    // Increment total tick count.
    ++m_tickCounter;

    // Tick game level.
    m_level->tick(timeDelta);

    // Tick players.
    m_playerManager->tick(timeDelta);

    // Tick world instance.
    m_world->tick(timeDelta);

    // Post tick network interface.
    m_network->postTick(timeDelta);
}

void GameInstance::draw(float timeAlpha)
{
    // Calculate render target view size and aspect ratio.
    float horizontalAspectRatio = (float)g_window->getWidth() / (float)g_window->getHeight();
    float verticalAspectRatio = (float)g_window->getHeight() / (float)g_window->getWidth();

    if(horizontalAspectRatio >= verticalAspectRatio)
    {
        verticalAspectRatio = 1.0f;
    }
    else
    {
        horizontalAspectRatio = 1.0f;
    }

    // Setup camera viewport.
    sf::Vector2f viewportSize;
    viewportSize.x = 10.0f * horizontalAspectRatio;
    viewportSize.y = 10.0f * verticalAspectRatio;

    if(!m_isViewportCentered)
    {
        m_viewport.setCenter(0.0f, 0.0f);
        m_isViewportCentered = true;
        LOG_TRACE("Application::draw - viewport centered.");
    }

    m_viewport.setSize(viewportSize);
    g_window->render.setView(m_viewport);

    // Center camera on player.
    if(m_isCameraAttachedToPlayer)
    {
        Object* object = m_world->getObjectByName("Player1_Tank");

        if(object)
        {
            Tank* tank = object->as<Tank>();

            if(tank != nullptr)
            {
                sf::Vector2f interpolatedPosition = tank->getPosition(timeAlpha);
                m_viewport.setCenter(interpolatedPosition);
            }
        }
    }

    // Manual camera navigation.
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::I)) m_viewport.move(0.f * timeAlpha, -0.05f * timeAlpha);
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::K)) m_viewport.move(0.f * timeAlpha, 0.05f * timeAlpha);
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::J)) m_viewport.move(-0.05f * timeAlpha, 0.f * timeAlpha);
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::L)) m_viewport.move(0.05f * timeAlpha, 0.f * timeAlpha);

    // Draw game level.
    m_level->draw(timeAlpha);

    // Draw world objects.
    m_world->draw(timeAlpha);

    // Draw network debug.
    m_network->draw();
}

uint64_t GameInstance::getTickFrame() const
{
    return m_tickCounter;
}

World& GameInstance::getWorld()
{
    ASSERT(m_world);
    return *m_world.get();
}

Level& GameInstance::getLevel()
{
    ASSERT(m_level);
    return *m_level.get();
}

PlayerManager& GameInstance::getPlayerManager()
{
    ASSERT(m_playerManager);
    return *m_playerManager.get();
}

NetworkInterface& GameInstance::getNetwork()
{
    ASSERT(m_network);
    return *m_network.get();
}

bool GameInstance::onSerialize(MemoryStream& buffer) const
{
    if(!serialize(buffer, m_tickCounter))
        return false;

    if(!serialize(buffer, *m_world))
        return false;

    if(!serialize(buffer, *m_level))
        return false;

    if(!serialize(buffer, *m_playerManager))
        return false;

    return true;
}

bool GameInstance::onDeserialize(MemoryStream& buffer)
{
    if(!deserialize(buffer, &m_tickCounter))
        return false;

    if(!deserialize(buffer, m_world.get()))
        return false;

    if(!deserialize(buffer, m_level.get()))
        return false;

    if(!deserialize(buffer, m_playerManager.get()))
        return false;

    return true;
}
