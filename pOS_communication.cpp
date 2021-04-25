#include "pOS_communication.hpp"
#include "pOS_gpio.hpp"
#include "pOS_scheduler.hpp"
#include "pOS_memory.hpp"
#include "pOS_critical_section.hpp"
#include "pOS_multicore.hpp"

#include "hardware/uart.h"
#include <string.h>
#include <stdio.h>

uint32_t pOS_communication_terminal::_index = 0;
uint8_t pOS_communication_terminal::_buffer[TERMINAL_BUFFER_SIZE];
volatile uart_inst_t* pOS_communication_terminal::_assigned_uart = 0;

void pOS_communication_terminal::initialize(uart_inst_t* uart, uint32_t tx_pin, uint32_t rx_pin)
{
	_assigned_uart = uart;
	uart_init(uart, 115200);
	uart_set_fifo_enabled(uart, true);

	pOS_gpio::get(tx_pin)->set_function(pOS_gpio_function::uart);
	pOS_gpio::get(rx_pin)->set_function(pOS_gpio_function::uart);
	
	/* Discard garbage on line */
	uart_getc((uart_inst_t*)_assigned_uart);
}

void pOS_communication_terminal::clear_terminal()
{
	if (_assigned_uart == 0)
		return;
	
	core_print_chr_offset((uart_inst_t*)_assigned_uart, 27);
	core_print_str_offset((uart_inst_t*)_assigned_uart, (uint8_t*)"[2J");
	core_print_chr_offset((uart_inst_t*)_assigned_uart, 27);
	core_print_str_offset((uart_inst_t*)_assigned_uart, (uint8_t*)"[H");
	
}
	
void pOS_communication_terminal::print_string(uint8_t* str, ...)
{
	if (_assigned_uart == 0)
		return;
	
	va_list ap;
	va_start(ap, str);
	
	int32_t ival;
	double dval;
	uint8_t* sval;
	uint8_t cval;
	
	uint8_t* p;
	for (p = str; *p; p++)
	{
		if (*p != '%')
		{
			core_print_chr_offset((uart_inst_t*)_assigned_uart, *p);
			continue;
		}
		
		switch (* ++p)
		{
		case 'd':
			{
				ival = va_arg(ap, int);
				print_int(ival);
				break;
			}
		case 'f':
			{
				dval = va_arg(ap, double);
				print_double(dval);
				break;
			
			}
		case 's':
			{
				for (sval = va_arg(ap, uint8_t*); *sval; sval++)
					print_char(*sval);
				break;
			}
		case 'c':
			{
				cval = (uint8_t)va_arg(ap, int);
				print_char(cval);
				break;
			}
		case '0':
			{
				uint8_t count = *(++p) - '0';
				uint8_t type = *(++p);
				ival = va_arg(ap, int);
				if (type == 'x')
				{
					if (count == 2)
						print_hex_2(ival);
					else if (count == 4)
						print_hex_4(ival);
					else
						print_hex_8(ival);
				}
				break;
			}
		default:
			putchar(*p);
			break;
		}
	}
	
	va_end(ap);
}

void pOS_communication_terminal::print_string(uint8_t* str, va_list args)
{
	if (_assigned_uart == 0)
		return;
	
	int32_t ival;
	double dval;
	uint8_t* sval;
	uint8_t cval;
	
	uint8_t* p;
	for (p = str; *p; p++)
	{
		if (*p != '%')
		{
			core_print_chr_offset((uart_inst_t*)_assigned_uart, *p);
			continue;
		}
		
		switch (* ++p)
		{
		case 'd':
			{
				ival = va_arg(args, int);
				print_int(ival);
				break;
			}
		case 'f':
			{
				dval = va_arg(args, double);
				print_double(dval);
				break;
			
			}
		case 's':
			{
				for (sval = va_arg(args, uint8_t*); *sval; sval++)
					print_char(*sval);
				break;
			}
		case 'c':
			{
				cval = (uint8_t)va_arg(args, int);
				print_char(cval);
				break;
			}
		case '0':
			{
				uint8_t count = *(++p) - '0';
				uint8_t type = *(++p);
				ival = va_arg(args, int);
				if (type == 'x')
				{
					if (count == 2)
						print_hex_2(ival);
					else if (count == 4)
						print_hex_4(ival);
					else
						print_hex_8(ival);
				}
				break;
			}
		default:
			putchar(*p);
			break;
		}
	}
}

void pOS_communication_terminal::print_char(uint8_t chr)
{
	if (_assigned_uart == 0)
		return;
	
	core_print_chr_offset((uart_inst_t*)_assigned_uart, chr);
}

uint8_t num_buff[100];
void pOS_communication_terminal::print_double(double num)
{
	sprintf((char *)num_buff, "%f", num);
	core_print_str_offset((uart_inst_t*)_assigned_uart, (uint8_t*)num_buff);
}
	
void pOS_communication_terminal::print_int(int32_t num)
{
	sprintf((char *)num_buff, "%lu", num);
	core_print_str_offset((uart_inst_t*)_assigned_uart, (uint8_t*)num_buff);
}

void pOS_communication_terminal::print_hex_2(uint32_t num)
{
	sprintf((char *)num_buff, "%02x", num);
	core_print_str_offset((uart_inst_t*)_assigned_uart, (uint8_t*)num_buff);
}

void pOS_communication_terminal::print_hex_4(uint32_t num)
{
	sprintf((char *)num_buff, "%04x", num);
	core_print_str_offset((uart_inst_t*)_assigned_uart, (uint8_t*)num_buff);
}

void pOS_communication_terminal::print_hex_8(uint32_t num)
{
	sprintf((char *)num_buff, "%08x", num);
	core_print_str_offset((uart_inst_t*)_assigned_uart, (uint8_t*)num_buff);
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
	
	uint8_t chr = uart_getc((uart_inst_t*)_assigned_uart);
	if(chr != '\r')
		append_buffer(chr);
	return chr;
}

void pOS_communication_terminal::interpret_command()
{
	pOS_critical::pOS_critical_section sec;
	
	sec.enter();
	if (strcmp((char *)_buffer, "reset") == 0)
	{
		pOS_communication_terminal::print_string((uint8_t*)"\nResetting...\n");
		AIRCR_Register = 0x5FA0004;
	}
	else if (strcmp((char*)_buffer, "led stop") == 0)
	{
		uint8_t* led_run = (uint8_t*)pOS_memory::wait_for_memory_id(MEM_ID_LED_RUNNING);
		pOS_communication_terminal::print_string((uint8_t*)"\nLED stopped\n");
		*led_run = 0;
	}
	else if (strcmp((char*)_buffer, "led start") == 0)
	{
		uint8_t* led_run = (uint8_t*)pOS_memory::wait_for_memory_id(MEM_ID_LED_RUNNING);
		pOS_communication_terminal::print_string((uint8_t*)"\nLED started\n");
		*led_run = 1;
	}
	else if (strcmp((char*)_buffer, "corrupt stack") == 0)
	{
		pOS_communication_terminal::print_string((uint8_t*)"\nCorrupting...\n");
		pOS_scheduler::corrupt_stack();
		pOS_communication_terminal::print_string((uint8_t*)"\nCorrupted\n");
	}
	else
	{
		pOS_communication_terminal::print_string((uint8_t*)"\nCommand not valid [");
		pOS_communication_terminal::print_string(_buffer);
		pOS_communication_terminal::print_string((uint8_t*)"]\n");
	}
	reset_buffer();
	sec.exit();
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
	
	pOS_gpio::get(tx_pin)->set_type(pOS_gpio_type::input);
	
#ifdef BOARD_PICO_ZERO
	pOS_gpio::get(rx_pin)->set_type(pOS_gpio_type::output)->enable();
#endif
	
#ifdef BOARD_PICO_ONE
	pOS_gpio::get(rx_pin)->set_type(pOS_gpio_type::output)->disable();
#endif

	uint32_t _tick = pOS_scheduler::get_tick();
	uint32_t _period_tick = _tick;
	
#ifdef BOARD_PICO_ZERO
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
	
	pOS_gpio::get(rx_pin)->disable();
	
	_tick = pOS_scheduler::get_tick();
	_period_tick = _tick;
	
	while (pOS_gpio::get(tx_pin)->read())
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
#endif
	
#ifdef BOARD_PICO_ONE
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
	
	pOS_gpio::get(rx_pin)->enable();
	
	_tick = pOS_scheduler::get_tick();
	_period_tick = _tick;
	
	while (pOS_gpio::get(tx_pin)->read())
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
	
	pOS_gpio::get(rx_pin)->disable();
#endif

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
