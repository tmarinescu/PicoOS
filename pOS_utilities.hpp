#ifndef _POS_UTILITIES_H_
#define _POS_UTILITIES_H_

#include "pOS_main.hpp"
#include "pOS_enums.hpp"
#include <stdarg.h>

class pOS_utilities
{
public:
	static uint32_t extract_bits(uint32_t value, uint32_t start_bit, uint32_t end_bit);
	static uint32_t set_bits(uint32_t value, uint32_t start_bit, uint32_t end_bit, uint32_t new_value);

	static void debug_print(uint8_t* str, ...);
	
	static pOS_mpu_size convert_stack_to_mpu_size(pOS_stack_size size);
	static pOS_mpu_size convert_stack_to_mpu_size(uint32_t size);
};

#endif