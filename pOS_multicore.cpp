#include "pOS_multicore.hpp"

extern "C"
{
#include "pico/util/queue.h"
}

#include "pico/multicore.h"

typedef struct
{
	uint32_t call_type;
	uart_inst_t* uart_type;
	uint8_t* data_s;
	uint8_t data_c;
} queue_entry_t;

queue_t call_queue;
queue_t results_queue;

void core1_entry()
{
	queue_entry_t entry;
	while (true)
	{
		queue_remove_blocking(&call_queue, &entry);
		
		if (entry.call_type == 1)
		{
			uart_puts(entry.uart_type, (char *)entry.data_s);
		}
		else if (entry.call_type == 2)
		{
			uart_putc(entry.uart_type, entry.data_c);
		}
	}
}

void core1_init()
{
	queue_init(&call_queue, sizeof(queue_entry_t), 2);
	multicore_launch_core1(core1_entry);
}

void core_print_str_offset(uart_inst_t* uart, uint8_t* data)
{
	queue_entry_t entry = { 1, uart, data, 0 };
	queue_add_blocking(&call_queue, &entry);
}

void core_print_chr_offset(uart_inst_t* uart, uint8_t data)
{
	queue_entry_t entry = { 2, uart, 0, data };
	queue_add_blocking(&call_queue, &entry);
}
