#ifndef _POS_ENUMS_H_
#define _POS_ENUMS_H_

enum class pOS_gpio_type
{
	input,
	output
};

enum class pOS_gpio_function
{
	uart,
	i2c,
	spi,
	adc,
	pwm
};

enum class pOS_task_priority
{
	super_low,
	low,
	medium,
	normal,
	high,
	immediate
};

enum class pOS_thread_error
{
	stack_overflow,
	stack_underflow,
	memory_violation,
	infinite_task,
	none
};

enum class pOS_thread_speed
{
	boost,		//Overclock
	fastest,
	normal,
	slow
};

enum class pOS_stack_size
{
	byte_256 = 256,
	byte_512 = 512,
	byte_1024 = 1024,
	byte_2048 = 2048,
	byte_4096 = 4096
};

enum class pOS_task_quanta
{
	heavy = 50,
	normal = 10,
	small = 1,
};

enum class pOS_multicore_commands
{
	print_char,
	print_str,
};

#endif