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

enum class pOS_thread_size
{
	byte_32 = 32,
	byte_64 = 64,
	byte_128 = 128,
	byte_256 = 256,
	byte_512 = 512,
	byte_1024 = 1024,
	byte_2048 = 2048
};

#endif