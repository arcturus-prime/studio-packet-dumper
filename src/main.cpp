#include "Region.h"
#include "Types.h"
#include "VFTable.h"

#include <cstdint>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

CRITICAL_SECTION g_receiveLock;
VFTable::Context g_ctx;

void hook(RakNet::RakPeer* rakPeer, void* _1, void* _2, void* _3) {
	EnterCriticalSection(&g_receiveLock);

	for (int i = rakPeer->queue.head; i < rakPeer->queue.tail; i++)
	{
		RakNet::Packet* packet = rakPeer->queue.array[i];

		for (int j = 0; j < packet->size; j++)
		{
			printf("%02x ", packet->data[j]);
		}

		printf("\n\n");
	}

	LeaveCriticalSection(&g_receiveLock);

	((decltype(&hook)) g_ctx.get_previous(119))(rakPeer, _1, _2, _3);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			InitializeCriticalSection(&g_receiveLock);
			AllocConsole();

			freopen_s((FILE**) stdout, "CONOUT$", "w", stdout);

			printf("Searching for VFTable...\n");

			auto region = Region::from_module(GetModuleHandle(NULL));
			auto ctx_optional = VFTable::Context::find(region, ".?AVRakPeer@RakNet@@");

			if (!ctx_optional.has_value())
			{
				printf("Unable to locate VFTable address!\n");
				return TRUE;
			}

			g_ctx = ctx_optional.value();

			printf("Found VFTable at: %llx\n", (uintptr_t) g_ctx.vftable);

			g_ctx.hook(119, (void*) hook);
		}
		break;

		case DLL_PROCESS_DETACH:
			g_ctx.unhook(119);
			break;
	}

	return TRUE;
}