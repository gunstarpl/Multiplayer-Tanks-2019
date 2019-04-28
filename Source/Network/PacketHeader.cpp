#include "Precompiled.hpp"
#include "Network/PacketHeader.hpp"

static const char MagicValue[] = "TanksXD";
static const int MagicSize = staticArraySize(MagicValue);

PacketHeader::PacketHeader()
{
    type = stringHash("");
}

PacketHeader::~PacketHeader()
{
}

bool PacketHeader::onSerialize(MemoryBuffer& buffer)
{
    for(char magic : MagicValue)
    {
        if(!serialize(buffer, magic))
            return false;
    }

    if(!serialize(buffer, type))
        return false;

    return true;
}

bool PacketHeader::onDeserialize(MemoryBuffer& buffer)
{
    for(char magic : MagicValue)
    {
        char character = '\0';
        if(!deserialize(buffer, &character))
            return false;

        if(character != magic)
            return false;
    }

    if(!deserialize(buffer, &type))
        return false;

    return true;
}