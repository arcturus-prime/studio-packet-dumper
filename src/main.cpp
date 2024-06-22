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

void hook(RakNet::RakPeer *rakPeer, void *_1, void *_2, void *_3)
{
    EnterCriticalSection(&g_receiveLock);

    for (unsigned int i = rakPeer->queue.head; i < rakPeer->queue.tail; i++)
    {
        RakNet::Packet *packet = rakPeer->queue.array[i];

        for (unsigned int j = 0; j < packet->size; j++)
        {
            printf("%02x ", packet->data[j]);
        }

        printf("\n\n");
    }

    LeaveCriticalSection(&g_receiveLock);

    auto original = (decltype(&hook))g_vftable.get_previous(119);
    original(rakPeer, _1, _2, _3);
}

void Attach()
{
    InitializeCriticalSection(&g_receiveLock);
    AllocConsole();

    freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);

    printf("Searching for VFTable...\n");

    auto region = StudioDumper::Region::from_module((uintptr_t)GetModuleHandle(NULL));
    auto vftable_optional = StudioDumper::VFTable::find(region, ".?AVRakPeer@RakNet@@");

    if (!vftable_optional.has_value())
    {
        printf("Unable to locate VFTable address!\n");
        return;
    }

    g_vftable = vftable_optional.value();
    printf("Found RakPeer VFTable at: %llx with length: %llu!\n", g_vftable.get_address(), g_vftable.get_size());

    g_vftable.hook(119, (uintptr_t)&hook);

    printf("Hooked networking!\n");
}

void Detach()
{
    g_vftable.unhook(119);

    printf("Unhooked networking!\n");
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    (void)hinstDLL;
    (void)lpvReserved;

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