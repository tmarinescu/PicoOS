#include "pOS_communication.hpp"
#include "pOS_gpio.hpp"
#include "hardware/uart.h"
#include <string.h>

uint32_t pOS_communication_terminal::_index = 0;
uint8_t pOS_communication_terminal::_buffer[TERMINAL_BUFFER_SIZE];
bool pOS_communication_terminal::_initialized = false;
volatile uart_inst_t* pOS_communication_terminal::_assigned_uart = 0;

void pOS_communication_terminal::initialize(uart_inst_t* uart, uint32_t tx_pin, uint32_t rx_pin)
{
	_assigned_uart = uart;
	uart_init(uart, 115200);
	uart_set_fifo_enabled(uart, false);

	pOS_gpio::get(tx_pin)->set_function(pOS_gpio_function::uart);
	pOS_gpio::get(rx_pin)->set_function(pOS_gpio_function::uart);
}

void pOS_communication_terminal::clear_terminal()
{
	if (_assigned_uart == 0)
		return;
	
	uart_putc((uart_inst_t*)_assigned_uart, 27) ;
	uart_puts((uart_inst_t*)_assigned_uart, "[2J");
	uart_putc((uart_inst_t*)_assigned_uart, 27);
	uart_puts((uart_inst_t*)_assigned_uart, "[H");
}
	
void pOS_communication_terminal::print_string(uint8_t* str)
{
	if (_assigned_uart == 0)
		return;
	
	uart_puts((uart_inst_t*)_assigned_uart, (char *)str);
}

void pOS_communication_terminal::print_char(uint8_t chr)
{
	if (_assigned_uart == 0)
		return;
	
	uart_putc((uart_inst_t*)_assigned_uart, chr);
}
	
void pOS_communication_terminal::reset_buffer()
{
	for (uint32_t i = 0; i < TERMINAL_BUFFER_SIZE; i++)
		_buffer[i] = 0;
	_index = 0;
}
	
void pOS_communication_terminal::append_buffer(uint8_t chr)
{
	if (_index >= TERMINAL_BUFFER_SIZE)
		return;
	_buffer[_index] = chr;
	_index++;
}
	
uint8_t* pOS_communication_terminal::get_buffer()
{
	return _buffer;
}
	
uint8_t pOS_communication_terminal::wait_for_input()
{
	while (_assigned_uart == 0)
	{
		/* Lock up until uart gets initialized */
	}
	
	while (!uart_is_readable((uart_inst_t*)_assigned_uart))
	{
		/* Lock up until uart is readable */
	}
	
	return uart_getc((uart_inst_t*)_assigned_uart);
}
	