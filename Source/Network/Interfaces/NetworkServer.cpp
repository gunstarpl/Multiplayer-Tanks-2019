#include "Precompiled.hpp"
#include "NetworkServer.hpp"
#include "Network/Connection/ConnectionBackend.hpp"
#include "Network/Packets/Protocol.hpp"
#include "Game/GameInstance.hpp"
#include "Game/World/World.hpp"
#include "Game/SnapshotSaveLoad.hpp"

NetworkServer::NetworkServer()
{
}

NetworkServer::~NetworkServer()
{
    LOG_TRACE("NetworkServer::~NetworkServer()");
}

bool NetworkServer::initialize(GameInstance* gameInstance, unsigned short port)
{
    // Initializes UDP socket.
    if(!NetworkBase::initialize(gameInstance))
        return false;

    // Initialize socket connection.
    if(!m_socket.listen(port))
    {
        LOG_ERROR("Could not initialize socket connection!");
        return false;
    }

    return true;
}

void NetworkServer::update(float timeDelta)
{
}

void NetworkServer::tick(float timeDelta)
{
    // Receive packet from default socket.
    // This needs another solution, either simplify or creating ConnectionSwitch for handling connections.
    ConnectionContext& socketContext = m_socket.getConnectionContext();
    ConnectionContext::PacketEntry packetEntry;

    while(socketContext.popIncoming(&packetEntry))
    {
        // Read received packet.
        std::unique_ptr<PacketBase> receivedPacket;
        if(!readPacket(packetEntry.packet, receivedPacket))
            continue;

        // Retrieve connect packet.
        PacketConnect* packetConnect = receivedPacket->as<PacketConnect>();
        if(packetConnect == nullptr)
            continue;

        // Check if we already have socket registered with remote address and port.
        auto socketBackend = m_socket.getConnectionBackend();
        if(socketBackend->hasSocketRegistered(packetEntry.address, packetEntry.port))
            continue;

        // Add new client connection list.
        std::unique_ptr<ConnectionSocket> clientSocket = std::make_unique<ConnectionSocket>(socketBackend);

        if(!clientSocket->connect(packetEntry.address, packetEntry.port))
        {
            LOG_ERROR("Could not connect new client socket!");
            continue;
        }

        // Move socket to client list.
        ClientEntry& clientEntry = m_clients.emplace_back();
        clientEntry.socket = std::move(clientSocket);

        // Reset received memory stream index (bit of a hack).
        // Get lack received packet copy would be a good solution? Would waste some little memory.
        packetEntry.packet.reset();
        
        // Push received packet to new socket.
        clientEntry.socket->getConnectionContext().pushIncoming(packetEntry);
    }

    // Receive packets from connected clients.
    for(auto& clientEntry : m_clients)
    {
        // Respond to received packet.
        std::unique_ptr<PacketBase> receivedPacket;
        while(receivePacket(*clientEntry.socket, receivedPacket, nullptr))
        {
            if(receivedPacket->is<PacketConnect>())
            {
                // Save game snapshot into packet memory stream.
                PacketStateSnapshot packetStateSnapshot;
                SnapshotSaveLoad snapshotSave(m_gameInstance);

                if(!snapshotSave.save(packetStateSnapshot.serializedGameInstance))
                {
                    LOG_ERROR("Coult not save snapshot into packet memoty!");
                    continue;
                }

                // Send reliable snapshot packet.
                if(!sendPacket(*clientEntry.socket, packetStateSnapshot, true))
                {
                    LOG_ERROR("Failed to send state snapshot packet to client!");
                    continue;
                }
            }
            else if(receivedPacket->is<PacketMessage>())
            {
                // Deserialize packet.
                PacketMessage* packetMessage = receivedPacket->as<PacketMessage>();
                ASSERT(packetMessage != nullptr);

                LOG_INFO("Received message packet with \"%s\" text.", packetMessage->text.c_str());

                // Send response packet.
                {
                    PacketMessage packetMessage;
                    packetMessage.text = "Hello server!";

                    if(!sendPacket(*clientEntry.socket, packetMessage, true))
                    {
                        LOG_ERROR("Failed to send message packet to client!");
                        continue;
                    }
                }
            }
        }
    }
}

void NetworkServer::draw()
{
    // Draw ImGui debug window.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(200, 100));
    if(ImGui::Begin("Network State (Server)##NetworkState", &cv_showNetwork.value))
    {
    }
    ImGui::End();
    ImGui::PopStyleVar(1);
}

NetworkMode NetworkServer::getMode() const
{
    return NetworkMode::Server;
}

bool NetworkServer::isConnected() const
{
    return true;
}