#include "Region.h"
#include "Types.h"
#include "VFTable.h"

#include <cstdint>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

CRITICAL_SECTION g_receiveLock;
StudioDumper::VFTable g_vftable;

void hook_24(RakNet::RakPeer* rakPeer, char _1)
{
    EnterCriticalSection(&g_receiveLock);

    for (unsigned int i = rakPeer->queue_2.head; i < rakPeer->queue_2.tail; i++)
    {
        auto packet = rakPeer->queue_2.array[i];

        printf("Queue 2:\n");

        for (unsigned int j = 0; j < packet->size; j++)
        {
            printf("%02x ", packet->data[j]);
        }

        printf("\n");
    }

    LeaveCriticalSection(&g_receiveLock);

    auto original = (decltype(&hook_24)) g_vftable.get_previous(24);
    original(rakPeer, _1);
}

void Attach()
{
    InitializeCriticalSection(&g_receiveLock);
    AllocConsole();

    freopen_s((FILE**) stdout, "CONOUT$", "w", stdout);

    printf("Searching for VFTable...\n");

    auto region = StudioDumper::Region::from_module((uintptr_t) GetModuleHandle(NULL));
    auto vftable_optional = StudioDumper::VFTable::find(region, ".?AVRakPeer@RakNet@@");

    if (!vftable_optional.has_value())
    {
        printf("Unable to locate VFTable address!\n");
        return;
    }

    g_vftable = vftable_optional.value();
    printf("Found RakPeer VFTable at 0x%llx with length %llu!\n", g_vftable.get_address(), g_vftable.get_size());

    g_vftable.hook(24, (uintptr_t) &hook_24);

    printf("Hooked networking!\n");
}

void Detach()
{
    g_vftable.unhook(24);

    printf("Unhooked networking!\n");
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    (void) hinstDLL;
    (void) lpvReserved;

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(Attach), NULL, NULL, NULL);
        break;
    case DLL_PROCESS_DETACH:
        CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(Detach), NULL, NULL, NULL);
        break;
    }

    return TRUE;
}