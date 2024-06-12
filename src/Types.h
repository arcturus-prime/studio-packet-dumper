#pragma once


typedef struct RakNet_Packet
{
	char unknown1[0x30];
	unsigned int size;
	unsigned int bitSize;
	unsigned char* data;

} RakNet_Packet;

typedef struct RakNet_PacketQueue
{
	RakNet_Packet** array;
	unsigned int head;
	unsigned int tail;
	unsigned int allocSize;

} RakNet_PacketQueue;
 
typedef struct RakNet_RakPeer
{
	char unknown1[0x8d0];
	RakNet_PacketQueue queue;

} RakNet_RakPeer;