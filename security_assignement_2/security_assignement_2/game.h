#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <math.h>
#include <csignal>

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

class OurPlayer {
public:
	// CSGO player object
	uintptr_t ptr;

	// Data we want
	Position pos;
	int* hp;
	int* money;
	int is_dead = 0;

	OurPlayer(uintptr_t p, Position p2, int* h, int* m, int d) : ptr(p), pos(p2), hp(h), money(m), is_dead(d) {};
};

// Adds console interupt
inline sig_atomic_t sigflag = 0;
inline void sighandler(int s) { sigflag = 1; }

class Game {
public:
	HMODULE server_dll_base_addr;
	int server_player_count = 0;
	//int server_gun_array_size = 0;
	std::vector<DeathLoc> player_deaths = {};
	std::vector<OurPlayer> player_list = {};
	std::vector<OurPlayer> alive_status = {};

	Game(HMODULE addr) : server_dll_base_addr(addr) {
		std::signal(SIGINT, sighandler);	// Assigns console interupt (Doesn't always work :/ )
	};
	void start();
	void every_bullet_counts(std::string cmd);
	std::vector<OurPlayer> get_new_player_list();
	void easy_mode();

};

