#ifndef _POS_SCHEDULER_H_
#define _POS_SCHEDULER_H_

#include "pOS_main.hpp"
#include "pOS_thread.hpp"
#include "pOS_task.hpp"

extern "C"
{
	void pOS_kernel_start(); /* Start the kernel with the first thread */
	uint32_t pOS_disable_and_save_interrupts();
	void pOS_restore_interrupts(uint32_t state);
	void pOS_disable_interrupts();
}

class pOS_scheduler
{
private:
	static volatile bool _running;
	
	static pOS_thread_id _thread_assignments[NUM_OF_THREADS];
	static uint32_t _thread_addresses[NUM_OF_THREADS];
	
	static pOS_thread _threads[NUM_OF_THREADS];
	static pOS_task _tasks[NUM_OF_TASKS];
	
	static volatile uint32_t _stack[TOTAL_MAXIMUM_STACK];
	static uint32_t _stack_offset;
	static uint32_t _task_count;
	static uint32_t _thread_init_offset;
	
	static volatile uint32_t _tick;
	static volatile uint32_t _current_thread;
	static volatile uint32_t _task_index;
	
	static pOS_thread* _active_thread;
	
public:
	static bool resume();
	static bool pause();
	static void update();
	static bool initialize();
	static void jump_start();
	
	static uint32_t get_tick();
	static void set_tick(uint32_t tick);
	static uint32_t calculate_checksum(volatile uint32_t* stack_loc, uint32_t size);
	static pOS_thread_id find_thread_critical(uint32_t needed_stack);
	
	static bool link_thread(pOS_thread_id thread, uint32_t thread_num, void(*volatile thrd)(pOS_thread_id));
	static bool enable_thread(pOS_thread_id id);
	static bool disable_thread(pOS_thread_id id);
	static bool reset_thread(pOS_thread_id id);
	static bool is_thread_enabled(pOS_thread_id id);
	static pOS_thread* get_thread(pOS_thread_id id);
	static bool initialize_thread(pOS_thread_id id, pOS_thread_size size);
	static bool is_thread_initialized(pOS_thread_id id);
	static bool set_thread_speed(pOS_thread_id id, pOS_thread_speed speed);
	
	static bool create_task(int32_t(*volatile function)(void), void(*volatile ret_handler)(int32_t), pOS_task_priority prio, uint32_t* ret_id, bool loop = false, uint32_t delayed_start = 0);
	static bool remove_task(uint32_t id);
	static bool enable_task(uint32_t id);
	static bool disable_task(uint32_t id);
	static bool loop_task(uint32_t id, uint32_t start_delay, uint32_t delay);
	static bool unloop_task(uint32_t id);
	static bool does_task_exist(uint32_t id);
	static pOS_task* get_active_task(pOS_thread_id id);
	
	static void set_thread_address(uint32_t index, uint32_t addr);
	static void set_thread_assignment(uint32_t index, pOS_thread_id id);
	static uint32_t get_thread_address(uint32_t index);
	static pOS_thread_id get_thread_assignment(uint32_t index);
};

#endif