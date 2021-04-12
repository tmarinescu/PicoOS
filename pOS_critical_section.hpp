#ifndef _POS_CRITICAL_SECTION_H_
#define _POS_CRITICAL_SECTION_H_

#include "pOS_main.hpp"

class pOS_critical
{
public:
	static uint32_t disable_and_save_interrupts();
	static void enable_and_restore_interrupts(uint32_t state);
	static void enable_all_interrupts();
	static void disable_all_interrupts();
	
	class pOS_critical_section
	{
	public:
		pOS_critical_section();
		~pOS_critical_section();
		void enter();
		void exit();
		
		bool active;
		uint32_t state_memory;
	};
};

#endif