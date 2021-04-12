#include "pOS_semaphore.hpp"
#include "pOS_critical_section.hpp"

void pOS_semaphore::initialize_semaphore(int32_t* semaphore, int32_t value)
{
	uint32_t state = pOS_critical::disable_and_save_interrupts();
	*semaphore = value;
	pOS_critical::enable_and_restore_interrupts(state);
}
	
void pOS_semaphore::set_semaphore(int32_t* semaphore)
{
	uint32_t state = pOS_critical::disable_and_save_interrupts();
	*semaphore += 1;
	pOS_critical::enable_and_restore_interrupts(state);
}

void pOS_semaphore::unset_semaphore(int32_t* semaphore)
{
	uint32_t state = pOS_critical::disable_and_save_interrupts();
	*semaphore = 0;
	pOS_critical::enable_and_restore_interrupts(state);
}
	
void pOS_semaphore::wait_semaphore(int32_t* semaphore)
{
	uint32_t state = pOS_critical::disable_and_save_interrupts();
	
	while (*semaphore <= 0)
	{
		
	}
	
	*semaphore -= 1;
	pOS_critical::enable_and_restore_interrupts(state);
}
	