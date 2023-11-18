#include "pch.h"
#include "game.h"
#include <iostream>

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
    server_gun_count = 0;
    player_deaths = {};
    std::vector<bool> alive_status = {};

    //TODO: a way to start the server either, 'mp_restart 1' or https://www.reddit.com/r/GlobalOffensive/comments/23gy12/console_commands_to_start_a_map_in_a_deathmatch/

    //TODO: Add our win condition 
    while (true) {
        // Setting weapon values
        int new_gun_count = 0; //TODO: fetch gun count
        if (new_gun_count != server_gun_count) {
            for (size_t i = server_gun_count; i < new_gun_count; i++) {
                //TODO: set clip ammo to 4
                //TODO: set reserver ammo to 0
            }
            server_gun_count = new_gun_count;
        }

        // Remove outdated player deaths
        while (player_deaths.size() != 0 && player_deaths.front().time_from() >= std::chrono::seconds(30)) {
            player_deaths.erase(player_deaths.begin());
        }

        // Setting player values
        server_player_count = 0; // TODO: Fetch new player count
        alive_status.reserve(server_player_count);
        for (size_t i = 0; i < server_player_count; i++) {
            int player_hp = 0;      // TODO: get player health
            Position pos{ 1,2,3 };  // TODO: Get player location
            
            // Player has respawned
            if (player_hp > 1) {
                alive_status[i] = true;
                // Set health to 1
            }

            // Player is dead died
            if (player_hp < 1) {
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

        break; //TODO: added for testing, remove later
    }
}
