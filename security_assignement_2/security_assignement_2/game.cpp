#include "pch.h"
#include "game.h"
#include <iostream>
#include "pointer_map.h"

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

void Game::every_bullet_counts(std::string cmd) {
    // Initalization
    server_player_count = 0;
    server_gun_array_size = 0;
    player_deaths = {};
    std::vector<bool> alive_status = {};

    //TODO: a way to start the server either, 'mp_restart 1' or https://www.reddit.com/r/GlobalOffensive/comments/23gy12/console_commands_to_start_a_map_in_a_deathmatch/

    //TODO: Add our win condition 
    while (true) {
        server_gun_array_size = get_gun_array_size(server_dll_base_addr);
        server_player_count = get_server_player_count(server_dll_base_addr);
        alive_status.resize(server_player_count);


        // Setting weapon values
        int count = 0;
        while (count < server_gun_array_size - 2) { //! No clue why -2 is necessary, but do not fucking delete it!
            auto gun_ptr = gun_ammo_clip(server_dll_base_addr, count);
            count++;
            // Non-weapon item/despawned
            if (gun_ptr == nullptr || *gun_ptr < 0)
                continue;


            if (*gun_ptr > 4) {
                *gun_ptr = 4;   // new weapon
            }
            if (*(gun_ptr + 2) != 0) {
                *(gun_ptr + 2) = 0; // new weapon
            }
        }
        
        // Remove outdated player deaths
        while (player_deaths.size() != 0 && player_deaths.front().time_from() >= std::chrono::seconds(30)) {
            player_deaths.erase(player_deaths.begin());
        }

        // Setting player values
        for (size_t i = 0; i < server_player_count; i++) {
            // player[i] stats
            auto player_hp = player_health(server_dll_base_addr, i);
            if (player_hp == nullptr) { // Player left the game 
                continue;
            }
            Position pos = player_pos(server_dll_base_addr, i);
            
            // Player has respawned
            if (*player_hp > 1) {
                alive_status[i] = true;
                *player_hp = 1; // Set their health to 1
            }

            // Player is dead
            if (*player_hp <= 0) {
                if (alive_status[i] == true) {
                    player_deaths.push_back(pos);
                    alive_status[i] = false;
                }
                continue;
            }

            // Working out if a player is over a dead body that hasn't been looted
            for (size_t i = 0; i < player_deaths.size(); i++) {
                if (player_deaths[i].distance_from(pos) <= 150) {  // TODO: Get approate distance for hammar units
                    // TODO: += 4 bullets to this player gun somehow???
                    player_deaths.erase(player_deaths.begin() + (i - 1));
                    i--;
                    continue;
                }
            }

        }

        break; //TODO: added for testing so it's not an infinite loop, remove later
    }
}
