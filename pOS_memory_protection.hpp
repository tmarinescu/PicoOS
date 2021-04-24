#ifndef _POS_MEMORY_PROTECTION_H_
#define _POS_MEMORY_PROTECTION_H_

#include "pOS_main.hpp"
#include "pOS_enums.hpp"

class pOS_memory_protection
{
private:
	static bool _enabled;
	static bool _broken;
	
	
public:
	static void initialize();
	
	static void disable_mpu();
	static void enable_mpu();
	static void set_mpu_unavailable();
	static void set_mpu_available();
	static bool is_mpu_available();
	
	static void init_region(uint8_t region, void* start, pOS_mpu_size size);
	static void lock_region(uint8_t region);
	static void unlock_region(uint8_t region);
};

#endif