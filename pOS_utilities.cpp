#include "pOS_utilities.hpp"

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
