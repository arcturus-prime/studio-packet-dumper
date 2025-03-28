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

template <typename T>
struct Queue
{
    T** array;
    unsigned int head;
    unsigned int tail;
    unsigned int alloc_size;
};

struct RakPeer
{
    char unknown_1[0x508];
    Queue<unsigned char> queue_1;
    char unknown_2[0x410];
    Queue<Packet> queue_2;
};
} // namespace RakNet
