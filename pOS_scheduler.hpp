#ifndef _POS_SCHEDULER_H_
#define _POS_SCHEDULER_H_

#define STACK_GUARD 0xA0B0C0D0
#define STACK_GUARD_SIZE 16

#include "pOS_main.hpp"
#include "pOS_thread.hpp"
#include "pOS_task.hpp"

extern "C" void pOS_kernel_start(); /* Start the kernel with the first thread */

class pOS_scheduler
{
private:
	static volatile bool _running;
	static bool _first_run;
	
	static uint32_t _thread_addresses[NUM_OF_THREADS];
	
	static pOS_thread _threads[NUM_OF_THREADS];
	static pOS_task _tasks[NUM_OF_TASKS];
	
	static volatile uint32_t _stack[TOTAL_MAXIMUM_STACK];
	static uint32_t _stack_offset;
	static uint32_t _task_count;
	static uint32_t _thread_init_offset;
	
	static volatile uint32_t _current_thread;
	static volatile uint32_t _task_index;
	
	static pOS_thread* _active_thread;
	
	static bool _mpu_enabled;
	static bool _mpu_broken;
	
public:
	static bool resume();
	static bool pause();
	static void update();
	static bool initialize();
	static void jump_start();
	static void corrupt_stack();
	
	static uint32_t get_tick();
	static void set_tick(uint32_t tick);
	static uint32_t calculate_checksum(volatile uint32_t* stack_loc, uint32_t offset, uint32_t size);
	static int32_t find_thread_critical(uint32_t needed_stack);
	
	static bool link_thread(int32_t thread_id, void(*volatile thrd)(int32_t));
	static bool enable_thread(int32_t thread_id);
	static bool disable_thread(int32_t thread_id);
	static bool reset_thread(int32_t thread_id);
	static bool is_thread_enabled(int32_t thread_id);
	static pOS_thread* get_thread(int32_t thread_id);
	static pOS_thread* get_active_thread();
	static bool initialize_thread(int32_t thread_id, pOS_stack_size size);
	static bool is_thread_initialized(int32_t thread_id);
	static bool set_thread_speed(int32_t thread_id, pOS_thread_speed speed);
	
	static bool create_task(int32_t(*volatile function)(void), void(*volatile ret_handler)(int32_t), pOS_task_quanta quanta, pOS_task_priority prio, uint32_t* ret_id, bool loop = false, uint32_t delayed_start = 0);
	static bool remove_task(uint32_t task_id);
	static bool enable_task(uint32_t task_id);
	static bool disable_task(uint32_t task_id);
	static bool loop_task(uint32_t task_id, uint32_t start_delay, uint32_t delay);
	static bool unloop_task(uint32_t task_id);
	static bool does_task_exist(uint32_t task_id);
	static pOS_task* get_active_task(int32_t thread_id);
	
	static void set_thread_address(uint32_t index, uint32_t addr);
	static uint32_t get_thread_address(uint32_t index);
	
	static void sleep(uint32_t ms);
	static void yield();
};

#endif