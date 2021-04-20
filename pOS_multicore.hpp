#ifndef _POS_MULTICORE_H_
#define _POS_MULTICORE_H_

#include "pOS_main.hpp"

#define VARIABLE_STACK_SIZE 512

void core1_init();
void core_print_str_offset(uart_inst_t* uart, uint8_t* data);
void core_print_chr_offset(uart_inst_t* uart, uint8_t data);

class pOS_multicore
{
private:
	uint8_t _variable_stack[512];
	
public:
	void begin_command();
	void push_uint32_t();
	void push_uint8_t();
	void end_command();
};

#endif