// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "pointer_map.h"
#include <iostream>
#include <Windows.h>
#include "game.h"
#include <psapi.h>

void fake_main(LPVOID hModule_ptr) {
    // Required for sending agrument to thread
    HMODULE hModule = *static_cast<HMODULE*>(hModule_ptr);
    delete hModule_ptr;

    // Get server.dll base address
    const HMODULE server_dll = GetModuleHandleW(L"server.dll");
    if (server_dll == nullptr) {
        throw "failed to find a loaded server.dll";
    }

    // open a console
    AllocConsole();
    FILE* pFile;
    freopen_s(&pFile, "CONOUT$", "w", stdout);
    freopen_s(&pFile, "CONIN$", "r", stdin);
    std::cout << "DLL attached\n";

    std::cout << "server.dll base address: " << server_dll <<"\n";

    // starts our game logic code
    Game game_obj = Game(server_dll);
    game_obj.start();

    // Clean up console
    std::cout << "Hack exiting\n";
    FreeConsole();

    // allows DLL to be injected multiple times
    FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    HMODULE* hModule_ptr = new HMODULE(hModule);

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        //TODO: look into starting a non-blocking thread a more modern way
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)fake_main, hModule_ptr, 0, nullptr));
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}

