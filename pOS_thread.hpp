#ifndef _POS_THREADS_H_
#define _POS_THREADS_H_

#include "pOS_main.hpp"
#include "pOS_enums.hpp"

class pOS_thread
{
public:
	int32_t id;
	
	volatile uint32_t* stack;
	volatile uint32_t* attached_task;
	
	uint32_t stack_size;
	uint32_t used_stack;
	
	uint32_t* stack_start;
	uint32_t stack_total_checksum;
	uint32_t stack_used_checksum;
	uint32_t stack_free_checksum;
	
	pOS_thread_error error_code;
	pOS_thread_speed speed;
	pOS_stack_size size;
	
	volatile bool enabled;
	volatile bool initialized;
	volatile bool error;
	
	pOS_thread();
	~pOS_thread();
};

void thread_1(int32_t id);
void thread_2(int32_t id);
void thread_3(int32_t id);
void thread_4(int32_t id);
void thread_5(int32_t id);
void thread_6(int32_t id);
void thread_7(int32_t id);
void thread_8(int32_t id);

#endif