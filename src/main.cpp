#include "Region.hpp"
#include "Types.hpp"
#include "VFTable.hpp"
#include "PacketWriter.hpp"

#include <windows.h>
#include <iostream>

CRITICAL_SECTION g_receiveLock;
StudioDumper::VFTable g_vftable;

void hook_25(RakNet::RakPeer* rakPeer, char _1)
{
    EnterCriticalSection(&g_receiveLock);

    for (uint32_t i = rakPeer->queue_2.head; i < rakPeer->queue_2.tail; i++)
    {
        auto packet = rakPeer->queue_2.array[i];
        auto stream = NetworkStream(packet->data, packet->size);

        while (!stream.is_empty()) {
            std::cout << print_packet(stream) << std::endl << std::endl;
        }
    }

    LeaveCriticalSection(&g_receiveLock);

    auto original = (decltype(&hook_25)) g_vftable.get_previous(25);
    original(rakPeer, _1);
}

void Attach()
{
    InitializeCriticalSection(&g_receiveLock);
    AllocConsole();

    freopen_s((FILE**) stdout, "CONOUT$", "w", stdout);

    std::cout << "Searching for VFTable..." << std::endl;

    auto region = StudioDumper::Region::from_module((uintptr_t) GetModuleHandle(NULL));
    auto vftable_optional = StudioDumper::VFTable::find(region, ".?AVRakPeer@RakNet@@");

    if (!vftable_optional.has_value())
    {
        std::cout << "Unable to locate VFTable address!" << std::endl;
        return;
    }

    g_vftable = vftable_optional.value();

    std::cout << "Found RakPeer VFTable at 0x" << std::hex << g_vftable.get_address() << " with length " <<  g_vftable.get_size() << "!" << std::endl;

    g_vftable.hook(25, (uintptr_t) &hook_25);

    std::cout << "Hooked networking!" << std::endl;
}

void Detach()
{
    g_vftable.unhook(25);

    std::cout << "Unhooked networking!" << std::endl;
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
