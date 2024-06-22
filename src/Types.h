#pragma once

namespace RakNet
{
struct Packet
{
    unsigned char unknown[0x30];
    unsigned int size;
    unsigned int bit_size;
    unsigned char* data;
};

struct PacketQueue
{
    Packet** array;
    unsigned int head;
    unsigned int tail;
    unsigned int alloc_size;
};

struct RakPeer
{
    char unknown[0x8f0];
    PacketQueue queue;
};

} // namespace RakNet
