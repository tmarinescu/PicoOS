#include "pOS_utilities.hpp"
#include "pOS_communication.hpp"
#include <stdarg.h>

uint32_t pOS_utilities::extract_bits(uint32_t value, uint32_t start_bit, uint32_t end_bit)
{
	uint32_t mask = 0;
	for (uint32_t i = start_bit; i <= end_bit; i++)
		mask |= 1 << i;
	
	return (value & mask) >> start_bit;
}

uint32_t pOS_utilities::set_bits(uint32_t value, uint32_t start_bit, uint32_t end_bit, uint32_t new_value)
{
	uint32_t mask = 0;
	for (uint32_t i = start_bit; i <= end_bit; i++)
		mask |= 1 << i;
	
	value = value & ~mask;
	value = value | ((new_value << start_bit) & mask);
	return value;
}

void pOS_utilities::debug_print(uint8_t* str, ...)
{
	if (ENABLE_DEBUG_PRINT)
	{
		va_list args;
		va_start(args, str);
		pOS_communication_terminal::print_string(str, args);
		va_end(args);
	}
}
