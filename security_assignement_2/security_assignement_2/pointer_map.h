#pragma once
#include <vector>

// use char* as the + operator will only increment 1 byte 
char* pointer_map(char* base_addr, std::vector<unsigned int> offset) {
	// Has to have at least a base address
	if (base_addr == nullptr) {
		throw "invalid use of pointer_map";
	}

	// Iterative treverse addresses by treat Base address + offset as a pointer until last offset
	// Return final pointer without 
	// example: (0x1010, {4, 9, 5})
	// (0x1010+4) = *0x1014  --> 0x555
	// (0x555+9)  = *0x564   --> 0x2000
	// (0x2000+5) = 0x2005	 return value
	for (size_t i = 0; i < offset.size()-1; i++)	{
		base_addr = (char*)*(base_addr + offset[i]);
	}

	base_addr = base_addr + offset.back();
	return base_addr;
}
