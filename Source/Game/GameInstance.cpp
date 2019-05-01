#include "Precompiled.hpp"
#include "Game/GameInstance.hpp"
#include "Game/World.hpp"
#include "Game/Level.hpp"
#include "Game/PlayerController.hpp"
#include "Network/Server.hpp"
#include "Network/Client.hpp"
#include "System/CommandLine.hpp"
#include "System/Window.hpp"

GameInstance::GameInstance() :
    m_network(nullptr),
    m_world(nullptr),
    m_level(nullptr),
    m_playerController(nullptr)
{
}

GameInstance::~GameInstance()
{
    // Shutdown systems in reverse order.
    if(m_playerController)
    {
        delete m_playerController;
        m_playerController = nullptr;
    }

    if(m_level)
    {
        delete m_level;
        m_level = nullptr;
    }

    if(m_world)
    {
        delete m_world;
        m_world = nullptr;
    }

    if(m_network)
    {
        delete m_network;
        m_network = nullptr;
    }
}

bool GameInstance::initialize()
{
    // Create network interface.
    if(g_commandLine->hasArgument("host"))
    {
        m_network = new Server();
        g_window->setTitle(g_window->getInitialTitle() + " - Server");
    }
    else
    {
        m_network = new Client();
        g_window->setTitle(g_window->getInitialTitle() + " - Client");
    }

    if(m_network)
    {
        if(!m_network->initialize())
            return false;
    }

    // Create world instance.
    m_world = new World;
    if(!m_world->initialize())
        return false;

    // Create game level.
    m_level = new Level;
    if(!m_level->initialize())
        return false;

    // Create player controller. 
    m_playerController = new PlayerController;
    if(!m_playerController->initialize(m_world))
        return false;

    return true;
}

void GameInstance::handleEvent(const sf::Event& event)
{
    // Handle player controller input.
    m_playerController->handleEvent(event);
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
    // Tick network interface.
    m_network->tick(timeDelta);

    // Tick game level.
    m_level->tick(timeDelta);

    // Tick player controller.
    m_playerController->tick(timeDelta);

    // Tick world instance.
    m_world->tick(timeDelta);
}

void GameInstance::draw(float timeAlpha)
{
    // Draw game level.
    m_level->draw(timeAlpha);

    // Draw world objects.
    m_world->draw(timeAlpha);

    // Draw network debug.
    m_network->draw();
}

World* GameInstance::getWorld()
{
    return m_world;
}

Level* GameInstance::getLevel()
{
    return m_level;
}

PlayerController* GameInstance::getPlayerController()
{
    return m_playerController;
}

bool GameInstance::onSerialize(MemoryBuffer& buffer)
{
    if(!serialize(buffer, *m_world))
        return false;

    if(!serialize(buffer, *m_level))
        return false;

    if(!serialize(buffer, *m_playerController))
        return false;

    return true;
}

bool GameInstance::onDeserialize(MemoryBuffer& buffer)
{
    if(!deserialize(buffer, m_world))
        return false;

    if(!deserialize(buffer, m_level))
        return false;

    if(!deserialize(buffer, m_playerController))
        return false;

    return true;
}
