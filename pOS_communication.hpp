#ifndef _POS_COMMUNICATION_H_
#define _POS_COMMUNICATION_H_

#include "pOS_main.hpp"

#define TERMINAL_BUFFER_SIZE 256
#define UART_BUFFER_SIZE 256

class pOS_communication_terminal
{
private:
	static uint32_t _index;
	static uint8_t _buffer[TERMINAL_BUFFER_SIZE];
	static volatile uart_inst_t* _assigned_uart;
	
public:
	static void initialize(uart_inst_t * uart, uint32_t tx_pin, uint32_t rx_pin);
	static void clear_terminal();
	static void print_string(uint8_t* str);
	static void print_char(uint8_t chr);
	static void reset_buffer();
	static void append_buffer(uint8_t chr);
	static uint8_t* get_buffer();
	static uint8_t wait_for_input();
	static void interpret_command();
};

/* Used for our development board */
class pOS_communication_mcu
{
private:
	static uint32_t _index;
	static uint8_t _buffer[TERMINAL_BUFFER_SIZE];
	static volatile uart_inst_t* _assigned_uart;
	static bool _handshake_confirmed;
	
public:
	static bool initialize(uart_inst_t* uart, uint32_t tx_pin, uint32_t rx_pin);
	static void send_data(uint8_t* data, uint32_t size);
	static uint8_t receive_data();
	static void interpret_data();
};

#endif