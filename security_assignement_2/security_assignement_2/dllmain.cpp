// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "pointer_map.h"
#include <iostream>
#include <Windows.h>

void fake_main() {

    // Only works if we know base address for the server
    const HMODULE server_dll_base_addr = GetModuleHandle(L"server.dll");
    if (server_dll_base_addr == nullptr) {
        throw "failed to find a loaded server.dll";
    }
    
    // open console
    AllocConsole();
    FILE* pFile;
    freopen_s(&pFile, "CONOUT$", "w", stdout);
    freopen_s(&pFile, "CONIN$", "r", stdin);
    std::cout << "DLL attached\n";

    // Our game-mode here
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        //TODO: look into starting a non-blocking thread a more modern way
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)fake_main, hModule, 0, nullptr));
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

