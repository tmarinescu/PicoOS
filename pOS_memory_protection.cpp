#include "pOS_memory_protection.hpp"
#include "pOS_utilities.hpp"
#include "hardware/sync.h"
#include "pOS_communication.hpp"

bool pOS_memory_protection::_enabled;
bool pOS_memory_protection::_broken;

void pOS_memory_protection::initialize()
{
	_enabled = false;
	_broken = false;
}

void pOS_memory_protection::disable_mpu()
{
	if (_broken)
		return;
	
	uint32_t value = MPU_CTRL_Register;
	value = pOS_utilities::set_bits(value, 0, 0, 0); /* Enable */
	value = pOS_utilities::set_bits(value, 1, 1, 0); /* MPU enabled while in hardfault ISR */
	value = pOS_utilities::set_bits(value, 2, 2, 0); /* Use default memory map (defined in Armv6-M reference manual B3.1) */
	MPU_CTRL_Register = value;
	
	_enabled = false;
	
	/* Not necessary but don't hurt */
	__isb();
	__dsb();
}
	
void pOS_memory_protection::enable_mpu()
{
	if (_broken)
		return;
	
	uint32_t value = MPU_CTRL_Register;
	value = pOS_utilities::set_bits(value, 0, 0, 1);	/* Enable */
	value = pOS_utilities::set_bits(value, 1, 1, 0);	/* MPU enabled while in hardfault ISR */
	value = pOS_utilities::set_bits(value, 2, 2, 1);	/* Use default memory map (defined in Armv6-M reference manual B3.1) (Too lazy to define a memory map so use default one for now) */
	MPU_CTRL_Register = value;
	
	_enabled = true;
	
	/* Not necessary but don't hurt */
	__isb();
	__dsb();
}

void pOS_memory_protection::set_mpu_unavailable()
{
	_enabled = false;
	_broken = true;
}
	
void pOS_memory_protection::set_mpu_available()
{
	_broken = false;
	disable_mpu();
}

bool pOS_memory_protection::is_mpu_available()
{
	return !_broken;
}

bool pOS_memory_protection::is_mpu_enabled()
{
	return _enabled || pOS_utilities::extract_bits(MPU_CTRL_Register, 0, 0);
}

void pOS_memory_protection::init_region(uint8_t region, void* start, pOS_mpu_size size)
{
	if (_broken)
		return;
	
	uint32_t value = MPU_RNR_Register;
	value = pOS_utilities::set_bits(value, 0, 3, region);
	MPU_RNR_Register = value;
	
	value = MPU_RBAR_Register;
	value = pOS_utilities::set_bits(value, 8, 31, ((uint32_t)start) >> 8); /* Shift to right 8 bits because the lower 8 bits are ignored in the register [address has to align up to 256 bytes] */
	value = pOS_utilities::set_bits(value, 0, 3, region);
	value = pOS_utilities::set_bits(value, 4, 4, 1);	/* Valid region (used to update region base address, otherwise the MPU uses what it already has */
	MPU_RBAR_Register = value;
	
	value = MPU_RASR_Register;
	value = pOS_utilities::set_bits(value, 0, 0, 1);		/* Enable region */
	value = pOS_utilities::set_bits(value, 1, 5, (uint32_t)size);		/* Size [2^(N+1)] */
	value = pOS_utilities::set_bits(value, 8, 15, 0x00);	/* Subregion enables [0x00 = enable all, 0xFF = disable all] (enable means that the region rules apply to it) */
	value = pOS_utilities::set_bits(value, 28, 28, 0);		/* XN */
	value = pOS_utilities::set_bits(value, 19, 21, 0);		/* TEX */
	value = pOS_utilities::set_bits(value, 24, 26, 0);		/* Access (0x03 = full access, 0x00 = no access, 0x06 = read-only) */
	value = pOS_utilities::set_bits(value, 16, 16, 0);		/* Bufferable */
	value = pOS_utilities::set_bits(value, 17, 17, 1);		/* Cacheable */
	value = pOS_utilities::set_bits(value, 18, 18, 0);		/* Shareable */
	MPU_RASR_Register = value;
}
	
void pOS_memory_protection::unlock_region(uint8_t region)
{
	if (_broken)
		return;
	
	uint32_t value = MPU_RNR_Register;
	value = pOS_utilities::set_bits(value, 0, 3, region);
	MPU_RNR_Register = value;
	
	value = MPU_RASR_Register;
	value = pOS_utilities::set_bits(value, 0, 0, 0);		/* Enable region */
	MPU_RASR_Register = value;
}

void pOS_memory_protection::lock_region(uint8_t region)
{
	if (_broken)
		return;
	
	uint32_t value = MPU_RNR_Register;
	value = pOS_utilities::set_bits(value, 0, 3, region);
	MPU_RNR_Register = value;
	
	value = MPU_RASR_Register;
	value = pOS_utilities::set_bits(value, 0, 0, 1); /* Enable region */
	MPU_RASR_Register = value;
}
