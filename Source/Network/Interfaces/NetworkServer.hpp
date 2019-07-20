#pragma once

#include "Precompiled.hpp"
#include "NetworkBase.hpp"
#include "Network/Replication/ReplicationServer.hpp"
#include "Network/Connection/ConnectionSocket.hpp"

// Forward declarations.
class ConnectionBackend;

// Network server class.
class NetworkServer : public NetworkBase
{
public:
    NetworkServer();
    ~NetworkServer();

    bool initialize(GameInstance* gameInstance, unsigned short port);
    void update(float timeDelta) override;
    void preTick(float timeDelta) override;
    void postTick(float timeDelta) override;
    void draw() override;

    NetworkMode getMode() const override;
    bool isConnected() const override;

    ReplicationBase& getReplication() override final;

private:
    // Replication system.
    ReplicationServer m_replication;

    // List of clients.
    struct ClientEntry
    {
        std::unique_ptr<ConnectionSocket> socket;
    };

    std::vector<ClientEntry> m_clients;
};
