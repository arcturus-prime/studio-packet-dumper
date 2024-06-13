#include "Context.h"
#include "Region.h"
#include "Types.h"
#include "VFTable.h"

#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <winnt.h>

typedef void (*ReceiveFunc)(void*, char);

Context ctx;
size_t funcIndex;

void** vftable = nullptr;

CRITICAL_SECTION ReceiveLock;

void Receive(RakNet_RakPeer* rakPeer, char _)
{

	EnterCriticalSection(&ReceiveLock);

	for (int i = rakPeer->queue.head; i < rakPeer->queue.tail; i++)
	{
		RakNet_Packet* packet = rakPeer->queue.array[i];

		if (packet->data[0] == 0x83 && packet->data[1] == 0x07)
		{

			for (int j = 0; j < packet->size; j++)
			{
				std::cout << std::hex << packet->data[j] << " ";
			}

			std::cout << "\n\n";
		}
	}

	LeaveCriticalSection(&ReceiveLock);

	(*(ReceiveFunc*) (ctx.hooks[funcIndex].previous.data()))(rakPeer, _);
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
			};

			void* func = (void*) &Receive;
			funcIndex = ctx.hook((uintptr_t) vftable + 23, (char*) &func, 8);
		}
		break;

		case DLL_PROCESS_DETACH:
			ctx.unhook(funcIndex);
			break;
	}

	return TRUE;
}