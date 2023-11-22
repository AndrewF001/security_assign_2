#pragma once
#include <vector>
#include <iostream>
#include "game.h"
#include <Windows.h>

//! we might need to check for read/write permission
/*
class PointerMap {
private:
	HANDLE hProcess;
public:
	PointerMap() {
		HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, GetCurrentProcessId());
		if (hProcess == NULL) {
			throw "Failed to open process handle. Error code: " + GetLastError();
		}
	}

	inline bool valid_addr(uintptr_t ptr) {
		LPVOID targetAddress = reinterpret_cast<LPVOID>(ptr);  // Change this to your desired address

		// Check the read permission
		MEMORY_BASIC_INFORMATION memInfo;
		if (VirtualQueryEx(hProcess, targetAddress, &memInfo, sizeof(memInfo)) == 0) {
			throw "VirtualQueryEx failed. Error code: " + GetLastError();
			CloseHandle(hProcess);
			return 1;
		}

		return (memInfo.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE));
	}
}
*/




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

inline int* player_health_offset(uintptr_t base_addr) {
	return (int*)pointer_map((void*)base_addr, { 0, 0x230 });
};


inline Position player_pos_offset(uintptr_t base_addr) {
	auto ptr = (float*)pointer_map((void*)base_addr, { 0, 0x1dc });

	if (ptr == nullptr)
		return Position{ 0,0,0 };

	return Position{ *ptr, *(ptr + 1), *(ptr + 2) };
};

inline int* get_player_money_offset(uintptr_t base_addr) {
	return (int*)pointer_map((void*)base_addr, { 0, 0x2DA4 });
};

inline int* gun_ammo_clip(HMODULE base_addr, unsigned int index) {
	return (int*)pointer_map(base_addr, { 0x00AAEBFC, index * 0x8, 0x964 });
};

inline int* gun_ammo_reserve(HMODULE base_addr, unsigned int index) {
	return (int*)pointer_map(base_addr, { 0x00AAEBFC, index * 0x8, 0x96c });
};

inline int get_server_player_count(HMODULE base_addr) {
	auto ptr = (int*)pointer_map(base_addr, { 0x00B606FC, 0x7c, 0x20, 0x18, 0x488 });

	if (ptr == nullptr)
		return 0;

	return *(int*)ptr;
}


inline int get_gun_array_size(HMODULE base_addr) {
	auto ptr = (int*)pointer_map(base_addr, { 0xAAFE40 });

	if (ptr == nullptr)
		return 0;

	return *ptr;
}

inline uintptr_t get_player_ptr(HMODULE base_addr, unsigned int index) {
	return (uintptr_t)pointer_map(base_addr, { 0x00A49EC8, index * 0x4 });
}

/* Pointer map doesn't work after restart :/	- WHY!
inline int get_alive_player_count(HMODULE base_addr) {
	auto ptr = (int*)pointer_map(base_addr, { 0x000C7234, 0x168, 0xA10, 0xE94});

	if (ptr == nullptr)
		return 0;

	return (*ptr) + (*ptr+1);
}
*/
