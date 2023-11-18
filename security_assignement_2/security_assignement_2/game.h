#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <math.h>

struct Position {
	float x, y, z;
};

class DeathLoc {
private:
	Position pos;
	std::chrono::steady_clock::time_point time;
public:

	DeathLoc(Position pos1) {
		pos = pos1;
		time = std::chrono::steady_clock::now();
	};

	float distance_from(Position pos2) {
		return sqrtf(
			powf(pos2.x - pos.x, 2) +
			powf(pos2.y - pos.y, 2) + 
			powf(pos2.z - pos.z, 2));
	};

	std::chrono::seconds time_from() {
		return std::chrono::duration_cast<std::chrono::seconds>(time - std::chrono::steady_clock::now());
	};
};

class Game {
public:
	HMODULE server_dll_base_addr;
	int server_player_count = 0;
	int server_gun_count = 0;
	std::vector<DeathLoc> player_deaths = {};

	Game(HMODULE addr) : server_dll_base_addr(addr) {};
	void start();
	void every_bullet_counts(std::string cmd);
};

