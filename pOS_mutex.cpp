#include "pOS_mutex.hpp"
#include  "pOS_critical_section.hpp"
#include "hardware/sync.h"
#include "pOS_scheduler.hpp"

/* LDREX and STREX are not supported on Cortex-M0, so this is the best we can do in terms of atomic mutex */

pOS_mutex::pOS_mutex()
{
	__dmb();
	_semaphore = 0;
}

void pOS_mutex::lock() volatile
{
	while (true)
	{
		__dmb();
		uint32_t mem = pOS_critical::disable_and_save_interrupts();
		if (_semaphore == 0)
		{
			_semaphore = 1;
			locked = true;
			pOS_critical::enable_and_restore_interrupts(mem);
			return;
		}
		pOS_critical::enable_and_restore_interrupts(mem);
		pOS_scheduler::yield();
	}
}
	
void pOS_mutex::unlock() volatile
{
	__dmb();
	uint32_t mem = pOS_critical::disable_and_save_interrupts();
	_semaphore = 0;
	locked = false;
	pOS_critical::enable_and_restore_interrupts(mem);
}

pOS_auto_mutex::pOS_auto_mutex(pOS_mutex* mutex)
{
	main_mutex = mutex;
	main_mutex->lock();
}
	
pOS_auto_mutex::~pOS_auto_mutex()
{
	main_mutex->unlock();
}
