#include "pOS_communication.hpp"
#include "pOS_gpio.hpp"
#include "pOS_scheduler.hpp"
#include "pOS_memory.hpp"
#include "pOS_critical_section.hpp"
#include "pOS_multicore.hpp"
#include "pOS_utilities.hpp"

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
	
bool pOS_communication_terminal::append_buffer(uint8_t chr)
{
	if (_index >= TERMINAL_BUFFER_SIZE)
		return false;
	_buffer[_index] = chr;
	_index++;
	return true;
}

bool pOS_communication_terminal::truncate_buffer()
{
	if (_index <= 0)
		return false;
	_buffer[--_index] = 0;
	return true;
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
	if (chr == '\r')
	{
		return chr;
	}
	else if (chr == '\b' || chr == 127)
	{
		if (truncate_buffer())
		{
			return chr;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if (append_buffer(chr))
		{
			return chr;
		}
		else
		{
			return 0;
		}
	}
}

void pOS_communication_terminal::interpret_command()
{
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
		
		if (pOS_scheduler::corrupt_stack())
		{
			pOS_communication_terminal::print_string((uint8_t*)"\nCorrupted\n");
		}
		else
		{
			pOS_communication_terminal::print_string((uint8_t*)"\nCould not corrupt\n");
		}
	}
	else if (strcmp((char*)_buffer, "bluetooth wake") == 0)
	{
		pOS_bluetooth::clear_response();
		pOS_utilities::debug_print((uint8_t*)"\nWaking up...\n");
		for (uint32_t i = 0; i < 1024; i++)
			pOS_bluetooth::send_data_1b('f');
		while (!pOS_bluetooth::wait_for_any_response(500))
		{
			for (uint32_t i = 0; i < 1024; i++)
				pOS_bluetooth::send_data_1b('f');
		}
		pOS_communication_terminal::print_string((uint8_t*)"Response: ");
		pOS_bluetooth::print_response();
		pOS_communication_terminal::print_string((uint8_t*)"\n");
	}
	else if (strcmp((char*)_buffer, "bluetooth sleep") == 0)
	{
		pOS_bluetooth::clear_response();
		pOS_bluetooth::send_data((uint8_t*)"AT+SLEEP\r\n", 10);
		if (pOS_bluetooth::wait_for_any_response(5000))
		{
			pOS_communication_terminal::print_string((uint8_t*)"Response: ");
			pOS_bluetooth::print_response();
			pOS_communication_terminal::print_string((uint8_t*)"\n");
		}
		else
		{
			pOS_communication_terminal::print_string((uint8_t*)"Response timed out!\n");
		}
	}
	else if (strcmp((char*)_buffer, "bluetooth check") == 0)
	{
		pOS_bluetooth::clear_response();
		pOS_bluetooth::send_data((uint8_t*)"AT\r\n", 4);
		if (pOS_bluetooth::wait_for_any_response(5000))
		{
			pOS_communication_terminal::print_string((uint8_t*)"Response: ");
			pOS_bluetooth::print_response();
			pOS_communication_terminal::print_string((uint8_t*)"\n");
		}
		else
		{
			pOS_communication_terminal::print_string((uint8_t*)"Response timed out!\n");
		}
	}
	else if (strcmp((char*)_buffer, "bluetooth get baud") == 0)
	{
		pOS_bluetooth::clear_response();
		pOS_bluetooth::send_data((uint8_t*)"AT+BAUD?\r\n", 10);
		if (pOS_bluetooth::wait_for_any_response(5000))
		{
			pOS_communication_terminal::print_string((uint8_t*)"Response: ");
			pOS_bluetooth::print_response();
			pOS_communication_terminal::print_string((uint8_t*)"\n");
		}
		else
		{
			pOS_utilities::debug_print((uint8_t*)"Response timed out!\n");
		}
		
	}
	else if (strcmp((char*)_buffer, "bluetooth get addr") == 0)
	{
		pOS_bluetooth::clear_response();
		pOS_bluetooth::send_data((uint8_t*)"AT+ADDR?\r\n", 10);
		if (pOS_bluetooth::wait_for_any_response(5000))
		{
			pOS_communication_terminal::print_string((uint8_t*)"Response: ");
			pOS_bluetooth::print_response();
			pOS_communication_terminal::print_string((uint8_t*)"\n");
		}
		else
		{
			pOS_utilities::debug_print((uint8_t*)"Response timed out!\n");
		}
		
	}
	else if (strcmp((char*)_buffer, "bluetooth help") == 0)
	{
		pOS_bluetooth::clear_response();
		pOS_bluetooth::send_data((uint8_t*)"AT+HELP?\r\n", 10);
		if (pOS_bluetooth::wait_for_any_response(5000))
		{
			pOS_communication_terminal::print_string((uint8_t*)"Response: ");
			pOS_bluetooth::print_response();
			pOS_communication_terminal::print_string((uint8_t*)"\n");
		}
		else
		{
			pOS_utilities::debug_print((uint8_t*)"Response timed out!\n");
		}
		
	}
	else
	{
		pOS_communication_terminal::print_string((uint8_t*)"\nCommand not valid [");
		pOS_communication_terminal::print_string(_buffer);
		pOS_communication_terminal::print_string((uint8_t*)"]\n");
	}
	reset_buffer();
}

uint32_t pOS_communication_mcu::_index = 0;
uint8_t pOS_communication_mcu::_buffer[UART_BUFFER_SIZE];
volatile uart_inst_t* pOS_communication_mcu::_assigned_uart = 0;
bool pOS_communication_mcu::_handshake_confirmed = false;

bool pOS_communication_mcu::initialize(uart_inst_t* uart, uint32_t tx_pin, uint32_t rx_pin)
{
	for (uint32_t i = 0; i < UART_BUFFER_SIZE; i++)
		_buffer[i] = 0;
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

	uint32_t _tick = pOS_scheduler::get_time_tick();
	uint32_t _period_tick = _tick;
	
#ifdef BOARD_PICO_ZERO
	pOS_utilities::debug_print((uint8_t*)"\n[Phase 1]...");
	while (!pOS_gpio::get(tx_pin)->read())
	{
		/* Lock up until other MCU is responsive */
		if (pOS_scheduler::get_time_tick() - _tick >= HANDSHAKE_TIMEOUT)
		{
			return false; /* Time out */
		}
		
		if (pOS_scheduler::get_time_tick() - _period_tick >= 100)
		{
			pOS_communication_terminal::print_char('.');
			_period_tick = pOS_scheduler::get_time_tick();
		}
	}
	
	pOS_gpio::get(rx_pin)->disable();
	
	_tick = pOS_scheduler::get_time_tick();
	_period_tick = _tick;
	
	pOS_utilities::debug_print((uint8_t*)"\n[Phase 2]...");
	while (pOS_gpio::get(tx_pin)->read())
	{
		/* Lock up until other MCU is responsive */
		if (pOS_scheduler::get_time_tick() - _tick >= HANDSHAKE_TIMEOUT)
		{
			return false; /* Time out */
		}
		
		if (pOS_scheduler::get_time_tick() - _period_tick >= 100)
		{
			pOS_communication_terminal::print_char('.');
			_period_tick = pOS_scheduler::get_time_tick();
		}
	}
#endif
	
#ifdef BOARD_PICO_ONE
	while (!pOS_gpio::get(tx_pin)->read())
	{
		/* Lock up until other MCU is responsive */
		if (pOS_scheduler::get_time_tick() - _tick >= HANDSHAKE_TIMEOUT)
		{
			return false; /* Time out */
		}
		
		if (pOS_scheduler::get_time_tick() - _period_tick >= 100)
		{
			pOS_communication_terminal::print_char('.');
			_period_tick = pOS_scheduler::get_time_tick();
		}
	}
	
	pOS_gpio::get(rx_pin)->enable();
	
	_tick = pOS_scheduler::get_time_tick();
	_period_tick = _tick;
	
	while (pOS_gpio::get(tx_pin)->read())
	{
		/* Lock up until other MCU is responsive */
		if (pOS_scheduler::get_time_tick() - _tick >= HANDSHAKE_TIMEOUT)
		{
			return false; /* Time out */
		}
		
		if (pOS_scheduler::get_time_tick() - _period_tick >= 100)
		{
			pOS_communication_terminal::print_char('.');
			_period_tick = pOS_scheduler::get_time_tick();
		}
	}
	
	pOS_gpio::get(rx_pin)->disable();
#endif

	/* Both MCUs did a GPIO handshake, now initialize uart on the same pins */
	_assigned_uart = uart;
	_handshake_confirmed = true;
	pOS_utilities::debug_print((uint8_t*)"\nPhase 1 and 2 successful, starting UART handshake...");
	uart_init((uart_inst_t *)_assigned_uart, 115200);
	uart_set_fifo_enabled(uart, false);
	
	pOS_gpio::get(tx_pin)->set_function(pOS_gpio_function::uart);
	pOS_gpio::get(rx_pin)->set_function(pOS_gpio_function::uart);
	
	pOS_utilities::debug_print((uint8_t*)"\nUART handshake successful!");
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
		_index = 0;
			
	_buffer[_index] = chr;
	_index++;
	return chr;
}
	
void pOS_communication_mcu::interpret_data()
{
	if (_assigned_uart == 0)
		return;
}


uint32_t pOS_bluetooth::_index;
uint8_t pOS_bluetooth::_buffer[BLUETOOTH_BUFFER_SIZE];
volatile uart_inst_t* pOS_bluetooth::_assigned_uart;
volatile uint32_t pOS_bluetooth::_response_check;

bool pOS_bluetooth::initialize(uart_inst_t* uart, uint32_t tx_pin, uint32_t rx_pin)
{
	_response_check = 0;
	for (uint32_t i = 0; i < BLUETOOTH_BUFFER_SIZE; i++)
		_buffer[i] = 0;
	_index = 0;
	_assigned_uart = uart;
	uart_init(uart, 9600);
	uart_set_fifo_enabled(uart, true);

	pOS_gpio::get(tx_pin)->set_function(pOS_gpio_function::uart);
	pOS_gpio::get(rx_pin)->set_function(pOS_gpio_function::uart);
	
	/* Discard garbage on line */
	//uart_getc((uart_inst_t*)_assigned_uart);
	return true;
}
	
void pOS_bluetooth::send_data(uint8_t* data, uint32_t size)
{
	if (_assigned_uart == 0)
		return;
	
	pOS_communication_terminal::print_string((uint8_t*)"\nCommand: ");
	for (uint32_t i = 0; i < size; i++)
	{
		if (data[i] != '\r' && data[i] != '\n')
		{
			pOS_communication_terminal::print_char(data[i]);
		}
	}
	pOS_communication_terminal::print_string((uint8_t*)"\n");
		
	for (uint32_t i = 0; i < size; i++)
	{
		uart_putc((uart_inst_t*)_assigned_uart, data[i]);
	}
}

void pOS_bluetooth::send_data_1b(uint8_t data)
{
	if (_assigned_uart == 0)
		return;
	uart_putc((uart_inst_t*)_assigned_uart, data);
}
	
void pOS_bluetooth::receive_data()
{
	if (_assigned_uart == 0)
		return;
	
	if (_response_check == 1)
		return;
	
	while (uart_is_readable((uart_inst_t*)_assigned_uart))
	{
		uint8_t chr = uart_getc((uart_inst_t*)_assigned_uart);
		if (chr == '\n' || chr == '\r')
		{
			_buffer[_index] = 0;
			_response_check = 1;
			return;
		}
		else
		{
			if (_index >= BLUETOOTH_BUFFER_SIZE)
				_index = 0;
			_buffer[_index] = chr;
			_index++;
		}
	}
}

bool pOS_bluetooth::wait_for_response(uint8_t* data, uint32_t size, uint32_t timeout)
{
	uint32_t tick = pOS_scheduler::get_time_tick();
	
	while (pOS_scheduler::get_time_tick() - tick < timeout)
	{
		if (_response_check == 1)
		{
			for (uint32_t i = 0; i < size; i++)
			{
				if (data[i] != _buffer[i])
				{
					return false;
				}
			}
				
			return true;
		}
	}
	
	return false;
}

bool pOS_bluetooth::wait_for_any_response(uint32_t timeout)
{
	uint32_t tick = pOS_scheduler::get_time_tick();
	
	while (pOS_scheduler::get_time_tick() - tick < timeout)
	{
		if (_response_check == 1)
		{
			return true;
		}
	}
	
	return false;
}

void pOS_bluetooth::clear_response()
{
	if (_assigned_uart == 0)
		return;
	
	while (uart_is_readable((uart_inst_t*)_assigned_uart))
		uart_getc((uart_inst_t*)_assigned_uart);
	
	_index = 0;
	for (uint32_t i = 0; i < BLUETOOTH_BUFFER_SIZE; i++)
		_buffer[i] = 0;
	_response_check = 0;
}

void pOS_bluetooth::print_response()
{
	if (_response_check == 0)
		return;
	
	for (uint32_t i = 0; i < _index; i++)
	{
		pOS_communication_terminal::print_char(_buffer[i]);
	}
}
