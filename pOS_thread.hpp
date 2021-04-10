#ifndef _POS_THREADS_H_
#define _POS_THREADS_H_

#include "pOS_main.hpp"
#include "pOS_enums.hpp"

class pOS_thread
{
public:
	pOS_thread_id id;
	
	volatile uint32_t* stack;
	volatile uint32_t* attached_task;
	
	uint32_t stack_size;
	uint32_t used_stack;
	uint32_t stack_crc32;
	uint32_t index;
	
	pOS_thread_error error_code;
	pOS_thread_speed speed;
	pOS_thread_size size;
	
	volatile bool enabled;
	volatile bool initialized;
	volatile bool error;
	
	pOS_thread();
	~pOS_thread();
};

void thread_1(pOS_thread_id id);
void thread_2(pOS_thread_id id);
void thread_3(pOS_thread_id id);
void thread_4(pOS_thread_id id);
void thread_5(pOS_thread_id id);
void thread_6(pOS_thread_id id);
void thread_7(pOS_thread_id id);
void thread_8(pOS_thread_id id);

#endif