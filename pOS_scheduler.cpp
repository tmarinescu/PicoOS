#include "pOS_scheduler.hpp"
#include "pOS_kernel_hooks.hpp"
#include "hardware/sync.h"
#include "pOS_critical_section.hpp"

extern "C"
{
	volatile uint32_t** pOS_stack_ptr; /* Used for context switching */
	
	/* Called by context switch. Stack swapping happens in update() */
	void pOS_scheduler_tick()
	{
		pOS_scheduler::pause();
		pOS_scheduler::update();
		pOS_scheduler::resume();
		return;
	}
}

/* Variables used by scheduler */
volatile bool pOS_scheduler::_running; 
pOS_thread pOS_scheduler::_threads[NUM_OF_THREADS];
pOS_task pOS_scheduler::_tasks[NUM_OF_TASKS];

uint32_t pOS_scheduler::_thread_addresses[NUM_OF_THREADS];
	
volatile uint32_t pOS_scheduler::_stack[TOTAL_MAXIMUM_STACK];
uint32_t pOS_scheduler::_stack_offset;
uint32_t pOS_scheduler::_task_count;
uint32_t pOS_scheduler::_thread_init_offset;
	
volatile uint32_t pOS_scheduler::_tick;
volatile uint32_t pOS_scheduler::_current_thread;
volatile uint32_t pOS_scheduler::_task_index;
	
pOS_thread* pOS_scheduler::_active_thread;


bool pOS_scheduler::resume()
{
	if (_running)
		return false;
	_running = true;
	return true;
}

bool pOS_scheduler::pause()
{
	if (!_running)
		return false;
	_running = false;
	return true;
}

/* Run the scheduler */
void pOS_scheduler::update()
{
	if (_running)
		return;
	
	_current_thread++;
	if (_current_thread >= NUM_OF_THREADS)
		_current_thread = 0;
	
	/* Try to find our first thread. By design this skips over the first thread when the scheduler first starts (needs to be fixed) */
	uint32_t _mem_thread = _current_thread;
	while (!_threads[_current_thread].enabled) /* Can cause infinite loop if all threads are disabled, needs a better design */
	{
		_current_thread++;
		if (_current_thread >= NUM_OF_THREADS)
			_current_thread = 0;
		
		if (_mem_thread == _current_thread)
		{
			break;
		}
	}
	
	/* Found a thread */
	_active_thread = &_threads[_current_thread];
	
	/* Find a task for it */
	if (_active_thread->attached_task == 0)
	{
		bool _found_task = false;
		uint32_t _inner_index = _task_index;
		for (uint32_t i = _task_index; i < NUM_OF_TASKS + _task_index; i++)
		{
			if (i < NUM_OF_TASKS)
				_inner_index = i;
			else
				_inner_index = i - NUM_OF_TASKS;
			
			if (_tasks[_inner_index].initialized && _tasks[_inner_index].enabled && _tasks[_inner_index].attached_thread == 0)
			{
				if (_tasks[_inner_index].loop)
				{
					if (get_tick() - _tasks[_inner_index].last_execute >= _tasks[_inner_index].execute_time)
					{
						_tasks[_inner_index].attached_thread = (uint32_t *)_active_thread;
						_active_thread->attached_task = (uint32_t*)&_tasks[_inner_index];
						_found_task = true;
						_task_index = _inner_index;
						break;
					}
					else
					{
						continue;
					}
				}
				else
				{
					_tasks[_inner_index].attached_thread = (uint32_t *)_active_thread;
					_active_thread->attached_task = (uint32_t*)&_tasks[_inner_index];
					_found_task = true;
					_task_index = _inner_index;
					break;
				}
			}
		}
		
		if (!_found_task)
		{
			_current_thread++;
			if (_current_thread >= NUM_OF_THREADS)
				_current_thread = 0;
			
			_mem_thread = _current_thread;
			while (_threads[_current_thread].attached_task == 0)
			{
				_current_thread++;
				if (_current_thread >= NUM_OF_THREADS)
					_current_thread = 0;
				if (_mem_thread == _current_thread)
				{
					break;
				}
			}
			
			_active_thread = &_threads[_current_thread];
		}
	}
	
	_task_index++;
	if (_task_index >= NUM_OF_TASKS)
		_task_index = 0;
	
	/* Swap stacks to the thread */
	pOS_stack_ptr = &_threads[_current_thread].stack;
	_tick++;
	return;
}

bool pOS_scheduler::initialize()
{
	_running = false;
	_stack_offset = 0;
	_task_count = 0;
	_thread_init_offset = 0;
	_tick = 0;
	_current_thread = 0;
	_task_index = 0;
	_active_thread = 0;
	
	for (uint32_t i = 0; i < TOTAL_MAXIMUM_STACK; i++)
	{
		_stack[i] = 0;
	}
	
	for (uint32_t i = 0; i < NUM_OF_THREADS; i++)
	{
		_threads[i].id = -1;
		_threads[i].attached_task = 0;
		_threads[i].enabled = false;
		_threads[i].error = 0;
		_threads[i].error_code = pOS_thread_error::none;
		_threads[i].initialized = false;
		_threads[i].size = pOS_thread_size::byte_32;
		_threads[i].speed = pOS_thread_speed::normal;
		_threads[i].stack = 0;
		_threads[i].stack_crc32 = 0;
		_threads[i].stack_size = 0;
		_threads[i].used_stack = 0;
		_thread_addresses[i] = 0;
	}
	
	for (uint32_t i = 0; i < NUM_OF_TASKS; i++)
	{
		_tasks[i].id = 0;
		_tasks[i].function_handler = 0;
		_tasks[i].return_handler = 0;
		_tasks[i].attached_thread = 0;
		_tasks[i].last_execute = 0;
		_tasks[i].next_execute = 0;
		_tasks[i].execute_time = 0;
		_tasks[i].quanta = 0;
		_tasks[i].quanta_max = 0;
		_tasks[i].priority = pOS_task_priority::normal;
		_tasks[i].initialized = false;
		_tasks[i].enabled = 0;
		_tasks[i].loop = 0;
		_tasks[i].memory_warning = 0;
		_tasks[i].black_listed = 0;
	}
	
	return true;
}

void pOS_scheduler::jump_start()
{
	pOS_kernel_start();
}

	
uint32_t pOS_scheduler::get_tick()
{
	return _tick;
}

void pOS_scheduler::set_tick(uint32_t tick)
{
	_tick = tick;
}

uint32_t pOS_scheduler::calculate_checksum(volatile uint32_t* stack_loc, uint32_t size)
{
	return 0;
}

int32_t pOS_scheduler::find_thread_critical(uint32_t needed_stack)
{
	return -1;
}

	
bool pOS_scheduler::link_thread(int32_t thread_id, void(*volatile thrd)(int32_t))
{
	if (thread_id >= NUM_OF_THREADS || thread_id < 0)
		return false;
	
	_thread_addresses[thread_id] = (uint32_t)thrd;
	_threads[thread_id].id = thread_id;
	return true;
}

bool pOS_scheduler::enable_thread(int32_t thread_id)
{
	pOS_thread* _thread = get_thread(thread_id);
	if (_thread == 0)
		return false;
	
	if (!_thread->initialized)
		return false;
	
	if (_thread->enabled)
		return false;
	
	_thread->enabled = true;
	
	if (_active_thread == 0)
		_active_thread = _thread;
	
	_active_thread->stack_crc32 = 0;
	
	return true;
}

bool pOS_scheduler::disable_thread(int32_t thread_id)
{
	pOS_thread* _thread = get_thread(thread_id);
	if (_thread == 0)
		return false;
	
	if (!_thread->initialized)
		return false;
	
	if (!_thread->enabled)
		return false;
	
	_thread->enabled = false;
	return true;
}

bool pOS_scheduler::reset_thread(int32_t thread_id)
{
	pOS_thread* _thread = get_thread(thread_id);
	if (_thread == 0)
		return false;
	
	if (!_thread->initialized)
		return false;
	
	_thread->initialized = false;
	return true;
}

bool pOS_scheduler::is_thread_enabled(int32_t thread_id)
{
	pOS_thread* _thread = get_thread(thread_id);
	if (_thread == 0)
		return false;
	
	if (!_thread->initialized)
		return false;
	
	return _thread->enabled;
}

pOS_thread* pOS_scheduler::get_thread(int32_t thread_id)
{
	return &_threads[thread_id];
}

bool pOS_scheduler::initialize_thread(int32_t thread_id, pOS_thread_size size)
{
	if (_thread_init_offset >= NUM_OF_THREADS)
		return false;
	
	pOS_thread* _thread = get_thread(thread_id);
	if (_thread == 0)
		return false;
	
	if (_thread->initialized)
		return false;
	
	for (uint32_t i = _stack_offset; i < _stack_offset + (uint32_t)size; i++)
	{
		_stack[i] = 0;
	}
	
	_thread->stack_size = (uint32_t)size;
	_thread->used_stack = 0;
	_stack[_stack_offset + (uint32_t)size - 1] = 0x01000000; //xPSR
	_stack[_stack_offset + (uint32_t)size - 2] = _thread_addresses[_thread->id]; //PC
	_stack[_stack_offset + (uint32_t)size - 8] = _thread->id; //R0 (argument passed to threads to identify itself by ID)
	_thread->stack = &_stack[_stack_offset + (uint32_t)size - 16]; //start of the stack pointer
	
	if(_thread_init_offset == 0)
		pOS_stack_ptr = &_thread->stack;
	
	_stack_offset += (uint32_t)size;
	_thread->attached_task = 0;
	_thread->enabled = false;
	_thread->initialized = true;
	_thread_init_offset++;
	
	return true;
}

bool pOS_scheduler::is_thread_initialized(int32_t thread_id)
{
	pOS_thread* _thread = get_thread(thread_id);
	if (_thread == 0)
		return false;
	
	return _thread->initialized;
}

bool pOS_scheduler::set_thread_speed(int32_t thread_id, pOS_thread_speed speed)
{
	return false;
}

	
bool pOS_scheduler::create_task(int32_t(*volatile function)(void), void(*volatile ret_handler)(int32_t), pOS_task_priority prio, uint32_t* ret_id, bool loop, uint32_t delayed_start)
{
	/* Disable all interrupts and remember mask */
	uint32_t status = pOS_critical::disable_and_save_interrupts();
	
	uint32_t _index = 0;
	for (_index = 0; _index < NUM_OF_TASKS; _index++)
	{
		if (_tasks[_index].initialized)
		{
			continue;
		}
		else
		{
			_tasks[_index].initialized = true;
			_tasks[_index].black_listed = false;
			_tasks[_index].enabled = false;
			_tasks[_index].loop = loop;
			_tasks[_index].memory_warning = false;
			_tasks[_index].priority = prio;
			_tasks[_index].quanta = 0;
			_tasks[_index].quanta_max = 0;
			_tasks[_index].last_execute = get_tick();
			_tasks[_index].next_execute = get_tick() + delayed_start;
			_tasks[_index].execute_time = delayed_start;
			_tasks[_index].attached_thread = 0;
			_tasks[_index].return_handler = ret_handler;
			_tasks[_index].function_handler = function;
			_task_count++;
			
			/* Restore all interrupts */
			*ret_id = _index;
			pOS_critical::enable_and_restore_interrupts(status);
			return true;
		}
	}
	
	/* Restore all interrupts */
	ret_id = 0;
	pOS_critical::enable_and_restore_interrupts(status);
	return false;
}

bool pOS_scheduler::remove_task(uint32_t id)
{
	if (id >= NUM_OF_TASKS)
		return false;
	
	_tasks[id].initialized = false;
	return true;
}

bool pOS_scheduler::enable_task(uint32_t id)
{
	if (id >= NUM_OF_TASKS)
		return false;
	
	if (!_tasks[id].initialized)
		return false;
	
	if (_tasks[id].enabled)
		return false;
	
	_tasks[id].enabled = true;
	return true;
}

bool pOS_scheduler::disable_task(uint32_t id)
{
	if (id >= NUM_OF_TASKS)
		return false;
	
	if (!_tasks[id].initialized)
		return false;
	
	if (!_tasks[id].enabled)
		return false;
	
	_tasks[id].enabled = false;
	return true;
}

bool pOS_scheduler::loop_task(uint32_t id, uint32_t start_delay, uint32_t delay)
{
	if (id >= NUM_OF_TASKS)
		return false;
	
	if (!_tasks[id].initialized)
		return false;
	
	if (_tasks[id].loop)
		return false;
	
	_tasks[id].loop = true;
	return true;
}

bool pOS_scheduler::unloop_task(uint32_t id)
{
	if (id >= NUM_OF_TASKS)
		return false;
	
	if (!_tasks[id].initialized)
		return false;
	
	if (!_tasks[id].loop)
		return false;
	
	_tasks[id].loop = true;
	return true;
}

bool pOS_scheduler::does_task_exist(uint32_t id)
{
	if (id >= NUM_OF_TASKS)
		return false;
	
	return _tasks[id].initialized;
}

pOS_task* pOS_scheduler::get_active_task(int32_t thread_id)
{
	pOS_thread* _thread = get_thread(thread_id);
	if (_thread == 0)
		return 0;
	
	if (!_thread->initialized)
		return 0;
	
	return (pOS_task*)_thread->attached_task;
}

void pOS_scheduler::set_thread_address(uint32_t index, uint32_t addr)
{
	_thread_addresses[index] = addr;
}
uint32_t pOS_scheduler::get_thread_address(uint32_t index)
{
	return _thread_addresses[index];
}
