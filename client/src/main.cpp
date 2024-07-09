#include "Region.h"
#include "Types.h"
#include "VFTable.h"

#include <cstdint>
#include <cstdio>
#include <fileapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>

CRITICAL_SECTION g_receiveLock;
StudioDumper::VFTable g_vftable;
HANDLE g_pipe;

void hook_24(RakNet::RakPeer* rakPeer, char _1)
{
    EnterCriticalSection(&g_receiveLock);

    for (unsigned int i = rakPeer->queue_2.head; i < rakPeer->queue_2.tail; i++)
    {
        auto packet = rakPeer->queue_2.array[i];

        DWORD num;
        WriteFile(g_pipe, (char*) &packet->size, 4, &num, NULL);
        WriteFile(g_pipe, packet->data, packet->size, &num, NULL);
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

    g_pipe = CreateFile(TEXT("\\\\.\\pipe\\StudioDumper"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (g_pipe != INVALID_HANDLE_VALUE)
    {
        printf("Failed to open named pipe, aborting...");
        return;
    }

    printf("Created named pipe.\n");

    g_vftable.hook(24, (uintptr_t) &hook_24);

    printf("Hooked networking!");
}

void Detach()
{
    g_vftable.unhook(24);

    printf("Unhooked networking!\n");

    CloseHandle(g_pipe);

    printf("Closed pipe handle!\n");
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