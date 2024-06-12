#include "Types.h"
#include "VFTable.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

typedef void (*ReceiveFunc)(void*, char);

ReceiveFunc original_Receive;

CRITICAL_SECTION ReceiveLock;

void** vftable;

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
				printf("%02x ", packet->data[j]);
			}

			printf("\n\n");
		}
	}

	LeaveCriticalSection(&ReceiveLock);

	return original_Receive(rakPeer, _);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{

	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:

			InitializeCriticalSection(&ReceiveLock);

			AllocConsole();
			freopen_s((FILE**) stdout, "CONOUT$", "w", stdout);

			vftable = VFTable_find(GetModuleHandle(NULL), ".?AVRakPeer@RakNet@@", 20);
			if (vftable == NULL)
			{
				printf("Unable to locate VFTable address!\n");
				return FALSE;
			};

			original_Receive = (ReceiveFunc) VFTable_hook(vftable, 23, (void*) &Receive);
			break;

		case DLL_PROCESS_DETACH:
			if (vftable == NULL)
				return TRUE;

			VFTable_hook(vftable, 23, (void*) original_Receive);
			break;
	}

	return TRUE;
}