#include "pOS_critical_section.hpp"
#include "pOS_kernel_hooks.hpp"

uint32_t pOS_critical::disable_and_save_interrupts()
{
	return pOS_disable_and_save_interrupts();
}
	
void pOS_critical::enable_and_restore_interrupts(uint32_t state)
{
	if (state != 0)
		pOS_enable_and_restore_interrupts(state);
	else
		pOS_enable_interrupts();
}
	
void pOS_critical::enable_all_interrupts()
{
	pOS_enable_interrupts();
}

void pOS_critical::disable_all_interrupts()
{
	pOS_disable_interrupts();
}
	

pOS_critical::pOS_critical_section::pOS_critical_section()
{
	active = false;
	state_memory = 0;
}

pOS_critical::pOS_critical_section::~pOS_critical_section()
{
	if (active)
	{
		enable_and_restore_interrupts(state_memory);
	}
}
	
void pOS_critical::pOS_critical_section::enter()
{
	if (!active)
	{
		active = true;
		state_memory = disable_and_save_interrupts();
	}
}
	
void pOS_critical::pOS_critical_section::exit()
{
	if (active)
	{
		enable_and_restore_interrupts(state_memory);
		active = false;
	}
}