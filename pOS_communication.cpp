#include "pOS_communication.hpp"
#include "pOS_gpio.hpp"
#include "pOS_scheduler.hpp"

#include "hardware/uart.h"
#include <string.h>

uint32_t pOS_communication_terminal::_index = 0;
uint8_t pOS_communication_terminal::_buffer[TERMINAL_BUFFER_SIZE];
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
	
	uint8_t chr = uart_getc((uart_inst_t*)_assigned_uart);
	append_buffer(chr);
	return chr;
}

uint32_t pOS_communication_mcu::_index = 0;
uint8_t pOS_communication_mcu::_buffer[UART_BUFFER_SIZE];
volatile uart_inst_t* pOS_communication_mcu::_assigned_uart = 0;
bool pOS_communication_mcu::_handshake_confirmed = false;

bool pOS_communication_mcu::initialize(uart_inst_t* uart, uint32_t tx_pin, uint32_t rx_pin)
{
	for (uint32_t i = 0; i < UART_BUFFER_SIZE; i++)
	{
		_buffer[i] = 0;
		
	}
	_index = 0;
	_handshake_confirmed = false;
	_assigned_uart = 0;
	
	/* Swap the pins for each board */
#ifdef BOARD_PICO_ONE
	uint32_t temp = tx_pin;
	tx_pin = rx_pin;
	rx_pin = temp;
#endif

	pOS_gpio::get(tx_pin)->set_type(pOS_gpio_type::input);
	pOS_gpio::get(rx_pin)->set_type(pOS_gpio_type::output)->enable();
	
	uint32_t _timeout = 0;
	uint32_t _tick = pOS_scheduler::get_tick();
	uint32_t _period_tick = _tick;
	while (!pOS_gpio::get(tx_pin)->read())
	{
		/* Lock up until other MCU is responsive */
		if (pOS_scheduler::get_tick() - _tick >= HANDSHAKE_TIMEOUT)
		{
			return false; /* Time out */
		}
		
		if (pOS_scheduler::get_tick() - _period_tick >= 100)
		{
			pOS_communication_terminal::print_char('.');
			_period_tick = pOS_scheduler::get_tick();
		}
	}
	
	/* Both MCUs did a GPIO handshake, now initialize uart on the same pins */
	_assigned_uart = uart;
	_handshake_confirmed = true;
	uart_init((uart_inst_t *)_assigned_uart, 115200);
	uart_set_fifo_enabled(uart, false);
	
	pOS_gpio::get(tx_pin)->set_function(pOS_gpio_function::uart);
	pOS_gpio::get(rx_pin)->set_function(pOS_gpio_function::uart);
	return true;
}
	
void pOS_communication_mcu::send_data(uint8_t* data, uint32_t size)
{
	if (_assigned_uart == 0)
		return;
		
	for (uint32_t i = 0; i < size; i++)
	{
		uart_putc((uart_inst_t*)_assigned_uart, data[i]);
	}
}
	
uint8_t pOS_communication_mcu::receive_data()
{
	if (_assigned_uart == 0)
		return 0;
	
	uint8_t chr = uart_getc((uart_inst_t*)_assigned_uart);
	
	if (_index >= UART_BUFFER_SIZE)
	{
		_index = 0;
		_buffer[_index] = chr;
	}
	
	return chr;
}
	
void pOS_communication_mcu::interpret_data()
{
	if (_assigned_uart == 0)
		return;
}