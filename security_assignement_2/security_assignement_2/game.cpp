#include "pch.h"
#include "game.h"
#include <iostream>
#include "pointer_map.h"
#include <thread>
#include <chrono>
#include<cstring>

void Game::start() {
    std::string input;
    while (true) {
        std::cout << "\nEnter command(help): ";
        std::cin >> input;

        if (input == "exit")
            break;

        // Only care if it starts with play 
        if (input.rfind("play", 0) == 0) {
            every_bullet_counts(input);
            continue;
        }

        if (input == "help") {
            std::cout << "exit - exits the hack\n";
            std::cout << "play - starts the hack\n";
            std::cout << "help - hello\n";
            continue;
        }

        std::cout << "Not a accepted command, type help\n";
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
