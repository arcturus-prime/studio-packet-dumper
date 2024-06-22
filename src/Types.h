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

struct SomePacket
{
};

template <typename T>
struct PacketQueue
{
    T** array;
    unsigned int head;
    unsigned int tail;
    unsigned int alloc_size;
};

struct RakPeer
{
    char unknown_1[0x4d0];
    PacketQueue<SomePacket> queue_1;
    char unknown_2[0x408];
    PacketQueue<Packet> queue_2;
};
} // namespace RakNet
