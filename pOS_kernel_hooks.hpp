#ifndef _POS_KERNEL_HOOKS_H_
#define _POS_KERNEL_HOOKS_H_

#include "pOS_main.hpp"

extern "C"
{
	uint32_t pOS_disable_and_save_interrupts();
	void pOS_enable_and_restore_interrupts(uint32_t state);
	void pOS_disable_interrupts();
	void pOS_enable_interrupts();
	
	extern uint32_t pOS_stack_upper_limit;
	extern uint32_t pOS_stack_lower_limit;
	extern uint32_t pOS_kernel_debug_values[9]; /* Used to send data to hard fault */
}

#endif