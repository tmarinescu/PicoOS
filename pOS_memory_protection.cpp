#include "pOS_memory_protection.hpp"
#include "pOS_utilities.hpp"
#include "hardware/sync.h"

void pOS_memory_protection::initialize()
{
	disable_mpu();
}

void pOS_memory_protection::disable_mpu()
{
	uint32_t value = MPU_CTRL_Register;
	value = pOS_utilities::set_bits(value, 0, 0, 0);
	value = pOS_utilities::set_bits(value, 1, 1, 0);
	value = pOS_utilities::set_bits(value, 2, 2, 0);
	MPU_CTRL_Register = value;
}
	
void pOS_memory_protection::enable_mpu()
{
	uint32_t value = MPU_CTRL_Register;
	value = pOS_utilities::set_bits(value, 0, 0, 1);
	value = pOS_utilities::set_bits(value, 1, 1, 1);
	value = pOS_utilities::set_bits(value, 2, 2, 1);
	MPU_CTRL_Register = value;
	__isb();
	__dsb();
}
	
void pOS_memory_protection::lock_area(uint8_t region, void* start, uint32_t size)
{
	//size is defined as 2^(size+1)
	uint32_t value = MPU_RNR_Register;
	value = pOS_utilities::set_bits(value, 0, 3, region);
	MPU_RNR_Register = value;
	
	value = MPU_RBAR_Register;
	value = pOS_utilities::set_bits(value, 8, 31, ((uint32_t)start) >> 8); //shift to right 8 bits because the lower 8 bits are ignored in the register (address has to line up to 256 bytes)
	value = pOS_utilities::set_bits(value, 0, 3, region);
	value = pOS_utilities::set_bits(value, 4, 4, 1);
	MPU_RBAR_Register = value;
	
	value = MPU_RASR_Register;
	value = pOS_utilities::set_bits(value, 0, 0, 1);
	value = pOS_utilities::set_bits(value, 1, 5, size);
	value = pOS_utilities::set_bits(value, 8, 15, 0x00);
	value = pOS_utilities::set_bits(value, 28, 28, 1);
	value = pOS_utilities::set_bits(value, 24, 26, 6); //AP
	value = pOS_utilities::set_bits(value, 16, 16, 0);
	value = pOS_utilities::set_bits(value, 17, 17, 0);
	value = pOS_utilities::set_bits(value, 18, 18, 0);
	MPU_RASR_Register = value;
}
	
void pOS_memory_protection::unlock_area(uint8_t region, void* start, uint32_t size)
{
	uint32_t value = MPU_RNR_Register;
	value = pOS_utilities::set_bits(value, 0, 3, region);
	MPU_RNR_Register = value;
	
	value = MPU_RBAR_Register;
	value = pOS_utilities::set_bits(value, 8, 31, ((uint32_t)start) >> 8);  //shift to right 8 bits because the lower 8 bits are ignored in the register (address has to line up to 256 bytes)
	value = pOS_utilities::set_bits(value, 0, 3, region);
	value = pOS_utilities::set_bits(value, 4, 4, 1);
	MPU_RBAR_Register = value;

	value = MPU_RASR_Register;
	value = pOS_utilities::set_bits(value, 0, 0, 0);
	value = pOS_utilities::set_bits(value, 1, 5, size);
	value = pOS_utilities::set_bits(value, 8, 15, 0xFF);
	value = pOS_utilities::set_bits(value, 28, 28, 1);
	value = pOS_utilities::set_bits(value, 24, 26, 3);  //AP
	value = pOS_utilities::set_bits(value, 16, 16, 0);
	value = pOS_utilities::set_bits(value, 17, 17, 0);
	value = pOS_utilities::set_bits(value, 18, 18, 0);
	MPU_RASR_Register = value;
}
