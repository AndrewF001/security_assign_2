#include "pch.h"
#include "game.h"
#include <iostream>
#include "pointer_map.h"
#include <thread>
#include <chrono>
#include<cstring>
#include <future>

void Game::start() {
    std::string input;
    std::future<void> play_thread;  // Future for play thread
    std::future<void> easy_mode_thread;  // Future for easy mode thread

    while (true) {
        std::cout << "\nEnter command(help): ";
        std::cin >> input;

        if (input == "exit")
            break;

        if (input == "play") {
            if (play_thread.valid()) {
                std::cout << "Play mode is already running.\n";
            }
            else {
                play_thread = std::async(std::launch::async, &Game::every_bullet_counts, this, input);
            }
            continue;
        }

        if (input == "easymode") {
            if (easy_mode_thread.valid()) {
                std::cout << "Easy mode is already running.\n";
            }
            else {
                easy_mode_thread = std::async(std::launch::async, &Game::easy_mode, this);
            }
            continue;
        }

        if (input == "help") {
            std::cout << "exit - exits the hack\n";
            std::cout << "play - starts the hack\n";
            std::cout << "easy mode - starts the easy mode hack\n";
            std::cout << "help - hello\n";
            continue;
        }

        std::cout << "Not an accepted command, type help\n";
    }

    if (play_thread.valid()) {
        play_thread.wait(); // Wait for play mode thread to finish if it's running
    }

    if (easy_mode_thread.valid()) {
        easy_mode_thread.wait(); // Wait for easy mode thread to finish if it's running
    }
}

//BROKEN! When we're writting values it sometimes crashes
void Game::every_bullet_counts(std::string cmd) {
    std::cout << "Starting Game\n";
    // Initalization
    player_deaths = {};
    player_list = {};
    alive_status = {};
    server_player_count = get_server_player_count(server_dll_base_addr);
    //TODO: a way to start the server either, 'mp_restart 1' or https://www.reddit.com/r/GlobalOffensive/comments/23gy12/console_commands_to_start_a_map_in_a_deathmatch/
    alive_status = get_new_player_list();
    while (server_player_count != 0) {
        // Reduces lag, crash likely-hood, and it's an acceptable timeframe
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Check Command interupt flag
        if (sigflag != 0) {
            break;
        }

        // Setting weapon values
        int count = 0;
        while (count < get_gun_array_size(server_dll_base_addr) - 2) { //! No clue why -2 is necessary, but do not delete it!
            auto gun_ptr = gun_ammo_clip(server_dll_base_addr, count);
            count++;
            // Non-weapon item/despawned
            if (gun_ptr == nullptr)
                continue;
            if (*gun_ptr < 0)
                continue;

            if (*gun_ptr > 4) { // gun clip ammo
                *gun_ptr = 4;   // new weapon
            }
            if (*(gun_ptr + 2) != 0) { // gun reserver ammo
                *(gun_ptr + 2) = 0; // new weapon
            }
        }

        // Remove outdated player deaths
        while (player_deaths.size() != 0 && player_deaths.front().time_from() >= std::chrono::seconds(30)) {
            player_deaths.erase(player_deaths.begin());
        }

        // new alive array
        player_list = get_new_player_list();

        // Setting player values
        for (size_t i = 0; i < player_list.size(); i++) {

            *player_list[i].money = 0;

            if (*player_list[i].hp <= 0) {
                if (alive_status[i].is_dead == 0) {
                    alive_status[i].is_dead = 1;
                    player_deaths.push_back(player_list[i].pos);
                }
                continue;
            }

           

            // Player has respawned
            if (*player_list[i].hp > 1) {
                *player_list[i].hp = 1; // Set their health to 1
                alive_status[i].is_dead = 0;
            }

            // Working out if a player is over a dead body that hasn't been looted
            for (int j = 0; j < player_deaths.size(); j++) {
                if (player_deaths[j].distance_from(player_list[i].pos) <= 150) {

                    //BROKEN! Work out how to assign only player[i] gets a bullet
                    int count = 0;
                    while (count < get_gun_array_size(server_dll_base_addr) - 2) { //! No clue why -2 is necessary, but do not delete it!
                        auto gun_ptr = gun_ammo_clip(server_dll_base_addr, count);
                        count++;

                        if (gun_ptr == nullptr)
                            continue;
                        if (*gun_ptr < 0)
                            continue;
                        *gun_ptr = 4;
                    }

                    player_deaths.erase(player_deaths.begin() + j);
                    j--;
                    continue;
                }
            }
        }
    }
    sigflag = 0;
    std::cout << "Game Mode Finished!\n";
}


void Game::easy_mode() {
    // Implement the easy mode functionality here
    std::cout << "Easy mode started\n";
    // ... easy mode logic ...

     DWORD moduleBase = (DWORD)GetModuleHandle(L"client.dll");


    while (!GetAsyncKeyState(VK_END))
    {
        DWORD LocalPlayer = *(DWORD*)(moduleBase + 0xDEF97C); //dwLocalPlayer
        DWORD GlowObjectManager = *(DWORD*)(moduleBase + 0x535FCB8); // dwGlowObjectManager
        DWORD EntityList = *(DWORD*)(moduleBase + 0x4E051DC); //dwEntityList
        int localTeam = *(int*)(LocalPlayer + 0xF4); //iTeamNunber
        for (int i = 1; i < 64; i++)
        {
            DWORD entity = *(DWORD*)((moduleBase + 0x4E051DC) + i * 0x10); //0x10 size of each entity
            if (entity == NULL) continue;

            int glowIndex = *(int*)(entity + 0x10488); //glowIndex
            int entityTeam = *(int*)(entity + 0xF4); //iTeam_num
            if (entityTeam == localTeam)
            {
                //Local Team

                //each rgb is 4 bytes shifted from eachother
                *(float*)((GlowObjectManager + glowIndex * 0x38 + 0x8)) = 0.f; //R
                *(float*)((GlowObjectManager + glowIndex * 0x38 + 0xc)) = 1.f; //G
                *(float*)((GlowObjectManager + glowIndex * 0x38 + 0x10)) = 0.f; //B
                *(float*)((GlowObjectManager + glowIndex * 0x38 + 0x14)) = 1.7f; //A

            }
            else {
                *(float*)((GlowObjectManager + glowIndex * 0x38 + 0x8)) = 1.f; //R
                *(float*)((GlowObjectManager + glowIndex * 0x38 + 0xc)) = 0.f; //G
                *(float*)((GlowObjectManager + glowIndex * 0x38 + 0x10)) = 0.f; //B
                *(float*)((GlowObjectManager + glowIndex * 0x38 + 0x14)) = 1.7f; //A

            }
            *(bool*)((GlowObjectManager + glowIndex * 0x38 + 0x24)) = true;
            *(bool*)((GlowObjectManager + glowIndex * 0x38 + 0x25)) = false;

        }
    }
    std::cout << "Easy mode finished\n";

}

std::vector<OurPlayer> Game::get_new_player_list() {

    auto player_count = get_server_player_count(server_dll_base_addr);
    if (player_count == 0) {
        server_player_count = 0;
        return {};
    }

    if (player_count > server_player_count) {
        server_player_count = player_count;
    }

    int* buffer = new int[server_player_count + 1];
    std::vector<OurPlayer> out;
    out.reserve(server_player_count);

    memcpy(buffer, (void*)((uintptr_t)server_dll_base_addr + 0x00A49EC8), server_player_count * 4);
    
    for (size_t i = 0; i < server_player_count; i++) {
        auto ptr = ((uintptr_t)*buffer) + (i * 4);
        if (ptr == (uintptr_t)NULL)
            continue;

        out.push_back({ ptr, player_pos_offset(ptr),player_health_offset(ptr), get_player_money_offset(ptr), 0 });
    }

    return out;
}
