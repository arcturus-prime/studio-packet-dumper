#pragma once

namespace RakNet {
	struct Packet {
		unsigned char unknown1[0x30];
		unsigned int size;
		unsigned int bitSize;
		unsigned char* data;
	};

	struct PacketQueue {
		Packet** array;
		unsigned int head;
		unsigned int tail;
		unsigned int allocSize;
	};

	struct RakPeer {
		char unknown1[0x8f0];
		PacketQueue queue;
	};

} // namespace RakNet
