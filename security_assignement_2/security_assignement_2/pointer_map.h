#pragma once
#include <vector>
#include <iostream>
#include "game.h"

inline void* pointer_map(void* base_addr, std::vector<unsigned int> offset) {
	// Has to have at least a base address
	if (base_addr == nullptr) {
		throw "invalid use of pointer_map";
	}

	uintptr_t itr_ptr = (uintptr_t)base_addr;

	// Iterative treverse addresses by treat Base address + offset as a pointer until last offset
	// Return final pointer without 
	// example: (0x1010, {4, 9, 5})
	// (0x1010+4) = *0x1014  --> 0x555
	// (0x555+9)  = *0x564   --> 0x2000
	// (0x2000+5) = 0x2005	 return value
	for (size_t i = 0; i < offset.size() - 1; i++) {
		if (itr_ptr == (uintptr_t)NULL)
			return nullptr;

		itr_ptr = *(uintptr_t*)(itr_ptr + offset[i]);
	}

	itr_ptr = itr_ptr + offset.back();
	return (void*)itr_ptr;
};

inline int* player_health(HMODULE base_addr, unsigned int index) {
	return (int*)pointer_map(base_addr, { 0x00A49EC8, index * 4, 0x24, 0xc0, 0x230});
};


inline Position player_pos(HMODULE base_addr, unsigned int index) {
	float* x = (float*)pointer_map(base_addr, { 0x00A49EC8, index * 4, 0x24, 0xc0, 0x1dc});
	return Position{ *x, *(x + 1), *(x + 2) };
};

inline int* gun_ammo_clip(HMODULE base_addr, unsigned int index) {
	return (int*)pointer_map(base_addr, { 0x00AAEBFC, index * 16, 0x54, 0x950});
};

inline int* gun_ammo_reserve(HMODULE base_addr, unsigned int index) {
	return (int*)pointer_map(base_addr, { 0x00AAEBFC, index * 16, 0x54, 0x958});
};

inline int get_server_player_count(HMODULE base_addr) {
	return *(int*)pointer_map(base_addr, { 0x00B606FC, 0x7c, 0x20, 0x18, 0x488});
}

// This is always used for current amount of guns but not always accurate for game-modes like deathmatch
inline int get_active_gun_count(HMODULE base_addr) {
	return *(int*)pointer_map(base_addr, { 0x0044F2C4, 0x910});
}

// Total gun count is used for round based game modes as you can carry guns between rounds
inline int get_total_gun_count(HMODULE base_addr) {
	return *(int*)pointer_map(base_addr, { 0x0044D200, 0x33C});
}