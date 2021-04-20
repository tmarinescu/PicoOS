#ifndef _POS_MEMORY_PROTECTION_H_
#define _POS_MEMORY_PROTECTION_H_

#include "pOS_main.hpp"

class pOS_memory_protection
{
public:
	static void initialize();
	
	static void disable_mpu();
	static void enable_mpu();
	
	static void lock_area(uint8_t region, void* start, uint32_t size);
	static void unlock_area(uint8_t region, void* start, uint32_t size);
};

#endif