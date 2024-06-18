#include "Context.h"
#include "Region.h"
#include "Types.h"
#include "VFTable.h"

#include <cstdint>
#include <iostream>
#include <stdlib.h>
#include <windows.h>

typedef void (*ReceiveFunc)(void*, void*, void*, void*);

Context ctx;
size_t funcIndex;

void** vftable = nullptr;

CRITICAL_SECTION ReceiveLock;

void Receive(RakNet_RakPeer* rakPeer, void* _1, void* _2, void* _3)
{
	EnterCriticalSection(&ReceiveLock);

	for (int i = rakPeer->queue.head; i < rakPeer->queue.tail; i++)
	{
		RakNet_Packet* packet = rakPeer->queue.array[i];

		for (int j = 0; j < packet->size; j++)
		{
			printf("%02x ", packet->data[j]);
		}

		printf("\n\n");
	}

	LeaveCriticalSection(&ReceiveLock);

	(*(ReceiveFunc*) (ctx.hooks[funcIndex].previous.data()))(rakPeer, _1, _2, _3);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			InitializeCriticalSection(&ReceiveLock);

			AllocConsole();
			freopen_s((FILE**) stdout, "CONOUT$", "w", stdout);

			auto region = Region::fromModule(GetModuleHandle(NULL));
			vftable = VFTable::find(region, ".?AVRakPeer@RakNet@@");

			if (vftable == nullptr)
			{
				std::cout << "Unable to locate VFTable address!\n";
				return FALSE;
			}

			void* func = (void*) &Receive;
			funcIndex = ctx.hook((uintptr_t) (vftable + 26), (char*) &func, 8);
		}
		break;

		case DLL_PROCESS_DETACH:
			ctx.unhook(funcIndex);
			break;
	}

	return TRUE;
}