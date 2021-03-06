#pragma once

#include "Precompiled.hpp"
#include "ReplicationBase.hpp"
#include "Network/Packets/PacketServerUpdate.hpp"

// Server replication class.
class ReplicationServer : public ReplicationBase
{
public:
    // Type declarations.
    using CommandList = std::vector<ReplicationCommand>;

public:
    ReplicationServer();
    ~ReplicationServer();

    // Collects object replication data.
    void collectReplicationData();

    // Clears replication commands.
    void clearReplicationCommands();

    // Creates reliable update packet.
    const std::vector<ReplicationCommand>& getReliableCommands() const;

    // Creates unreliable update packet.
    const std::vector<ReplicationCommand>& getUnreliableCommands() const;

private:
    // Overridden methods to add command recording.
    void onObjectCreated(Object& object) override final;
    void onObjectDestroyed(Object& object) override final;

private:
    // Replication command buffers.
    std::vector<ReplicationCommand> m_reliableCommands;
    std::vector<ReplicationCommand> m_unreliableCommands;
};
