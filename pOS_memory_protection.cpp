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
	value = pOS_utilities::set_bits(value, 0, 0, 0); /* Enable */
	value = pOS_utilities::set_bits(value, 1, 1, 0); /* MPU enabled while in hardfault ISR */
	value = pOS_utilities::set_bits(value, 2, 2, 0); /* Use default memory map (defined in Armv6-M reference manual B3.1) */
	MPU_CTRL_Register = value;
	
	/* Not necessary but don't hurt */
	__isb();
	__dsb();
}
	
void pOS_memory_protection::enable_mpu()
{
	
	uint32_t value = MPU_CTRL_Register;
	value = pOS_utilities::set_bits(value, 0, 0, 1);	/* Enable */
	value = pOS_utilities::set_bits(value, 1, 1, 0);	/* MPU enabled while in hardfault ISR */
	value = pOS_utilities::set_bits(value, 2, 2, 1);	/* Use default memory map (defined in Armv6-M reference manual B3.1) (Too lazy to define a memory map so use default one for now) */
	MPU_CTRL_Register = value;
	
	/* Not necessary but don't hurt */
	__isb();
	__dsb();
}

/* Yes, the lock_area and unlock_area functions are slow because of redundant calls, it will be fixed eventually */
void pOS_memory_protection::lock_area(uint8_t region, void* start, uint32_t size)
{
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
	value = pOS_utilities::set_bits(value, 1, 5, size);		/* Size [2^(N+1)] */
	value = pOS_utilities::set_bits(value, 8, 15, 0x00);	/* Subregion enables [0x00 = enable all, 0xFF = disable all] (enable means that the region rules apply to it) */
	value = pOS_utilities::set_bits(value, 28, 28, 0);		/* XN */
	value = pOS_utilities::set_bits(value, 19, 21, 0);		/* TEX */
	value = pOS_utilities::set_bits(value, 24, 26, 0);		/* Access (0x03 = full access, 0x00 = no access, 0x06 = read-only) */
	value = pOS_utilities::set_bits(value, 16, 16, 0);		/* Bufferable */
	value = pOS_utilities::set_bits(value, 17, 17, 1);		/* Cacheable */
	value = pOS_utilities::set_bits(value, 18, 18, 0);		/* Shareable */
	MPU_RASR_Register = value;
}
	
/* Yes, the lock_area and unlock_area functions are slow because of redundant calls, it will be fixed eventually */
void pOS_memory_protection::unlock_area(uint8_t region, void* start, uint32_t size)
{
	uint32_t value = MPU_RNR_Register;
	value = pOS_utilities::set_bits(value, 0, 3, region);
	MPU_RNR_Register = value;
	
	value = MPU_RBAR_Register;
	value = pOS_utilities::set_bits(value, 8, 31, ((uint32_t)start) >> 8);  /* Shift to right 8 bits because the lower 8 bits are ignored in the register [address has to align up to 256 bytes] */
	value = pOS_utilities::set_bits(value, 0, 3, region);
	value = pOS_utilities::set_bits(value, 4, 4, 1);	/* Valid region (used to update region base address, otherwise the MPU uses what it already has */
	MPU_RBAR_Register = value;

	value = MPU_RASR_Register;
	value = pOS_utilities::set_bits(value, 0, 0, 0);		/* Enable region */
	value = pOS_utilities::set_bits(value, 1, 5, size);		/* Size [2^(N+1)] */
	value = pOS_utilities::set_bits(value, 8, 15, 0xFF);	/* Subregion enables [0x00 = enable all, 0xFF = disable all] (enable means that the region rules apply to it) */
	value = pOS_utilities::set_bits(value, 28, 28, 0);		/* XN */
	value = pOS_utilities::set_bits(value, 19, 21, 0);		/* TEX */
	value = pOS_utilities::set_bits(value, 24, 26, 3);		/* Access (0x03 = full access, 0x00 = no access, 0x06 = read-only) */
	value = pOS_utilities::set_bits(value, 16, 16, 0);		/* Bufferable */
	value = pOS_utilities::set_bits(value, 17, 17, 1);		/* Cacheable */
	value = pOS_utilities::set_bits(value, 18, 18, 0);		/* Shareable */
	MPU_RASR_Register = value;
}
