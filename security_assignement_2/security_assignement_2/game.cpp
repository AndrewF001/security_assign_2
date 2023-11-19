#include "pch.h"
#include "game.h"
#include <iostream>
#include "pointer_map.h"
#include <thread>
#include <chrono>

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
    // Initalization
    player_deaths = {};
    std::vector<bool> alive_status = {};

    //TODO: a way to start the server either, 'mp_restart 1' or https://www.reddit.com/r/GlobalOffensive/comments/23gy12/console_commands_to_start_a_map_in_a_deathmatch/

    //TODO: Add our win condition 
    while (true) {
        // Reduces lag, crash likely-hood, and it's an acceptable timeframe
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
        // Check Command interupt flag
        if (sigflag != 0) {
            break;
        }

        // Setting weapon values
        int count = 0;
        while (count < get_gun_array_size(server_dll_base_addr) - 2) { //! No clue why -2 is necessary, but do not fucking delete it!
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
        
        //! We're never adding to list as listed below, so pointless
        // Remove outdated player deaths
        /*while (player_deaths.size() != 0 && player_deaths.front().time_from() >= std::chrono::seconds(30)) {
            player_deaths.erase(player_deaths.begin());
        }*/

        auto player_count = get_server_player_count(server_dll_base_addr);  
        alive_status.resize(player_count);

        // Setting player values
        for (size_t i = 0; i < player_count; i++) { //!Broken we want players alive not player connected, as player[] auto srinks when player dies
            // player[i] stats
            auto player_hp = player_health(server_dll_base_addr, i);
            if (player_hp == nullptr) {
                continue;
            }
            
            //*get_player_money(server_dll_base_addr, i) = 0;
            //BROKEN! Causes crashes when the game is writing to memory simultantionally
            auto money_ptr = get_player_money(server_dll_base_addr, i);
            if (money_ptr != nullptr) {
                if (*money_ptr != 0) {
                    *money_ptr = 0;
                }
            }

            //BROKEN! When player dies they are removed from the player list
            /*Position pos = player_pos(server_dll_base_addr, i);
            if (*player_hp <= 0) {
                if (alive_status[i] == true) {
                    player_deaths.push_back(pos);
                    alive_status[i] = false;
                }
                continue;
            }*/

            // Player has respawned
            if (*player_hp > 1) {
                alive_status[i] = true;
                *player_hp = 1; // Set their health to 1
            }

            //BROKEN! We have no idea when a player dies as they're removed from the player list
            // Working out if a player is over a dead body that hasn't been looted
            /*for (size_t i = 0; i < player_deaths.size(); i++) {
                if (player_deaths[i].distance_from(pos) <= 150) {  // TODO: Get approate distance for hammar units
                    // TODO: += 4 bullets to this player gun somehow???
                    player_deaths.erase(player_deaths.begin() + (i - 1));
                    i--;
                    continue;
                }
            }*/
        }

        //break;
    }
    sigflag = 0;
    std::cout << "Play function finished";
}
