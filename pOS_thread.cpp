#include "pOS_thread.hpp"
#include "pOS_scheduler.hpp"

pOS_thread::pOS_thread()
{
	id = pOS_thread_id::invalid;
	stack = 0;
	attached_task = 0;
	stack_size = 0;
	used_stack = 0;
	stack_crc32 = 0;
	error_code = pOS_thread_error::none;
	speed = pOS_thread_speed::normal;
	size = pOS_thread_size::byte_32;
	index = 0;
	
	enabled = false;
	initialized = false;
	error = false;
}

pOS_thread::~pOS_thread()
{
	
}

void thread_1(pOS_thread_id thread)
{
	pOS_thread* _thread = pOS_scheduler::get_thread(thread);
	if (_thread == 0)
		while (1) ; // Uh oh
	int32_t ret_code = 0;
	while (true)
	{
		if (_thread != 0)
		{
			if (_thread->initialized)
			{
				if (_thread->enabled)
				{
					if (_thread->attached_task != 0)
					{
						pOS_task* _task = (pOS_task*)_thread->attached_task;
						if (_task != 0)
						{
							if (_task->function_handler != 0)
							{
								ret_code = (*_task->function_handler)(); /* Call the task */
								if (_task->return_handler != 0)
								{
									(*_task->return_handler)(ret_code); /* Call it's return handler if it has one */
								}
						
								if (!_task->loop) /* If it's not looped just delete it */
								{
									_task->initialized = false;
								}
						
								_task->attached_thread = 0; /* Remove it from thread, if it's looped the scheduler will hand it to another thread */
							}
					
							_task->last_execute = pOS_scheduler::get_tick();
							_thread->attached_task = 0;
						}
					}
				}
			}
		}
	}
}

void thread_2(pOS_thread_id thread)
{
	pOS_thread* _thread = pOS_scheduler::get_thread(thread);
	if (_thread == 0)
		while (1) ;
	int32_t ret_code = 0;
	while (true)
	{
		if (_thread != 0)
		{
			if (_thread->initialized)
			{
				if (_thread->enabled)
				{
					if (_thread->attached_task != 0)
					{
						pOS_task* _task = (pOS_task*)_thread->attached_task;
						if (_task != 0)
						{
							if (_task->function_handler != 0)
							{
								ret_code = (*_task->function_handler)();
								if (_task->return_handler != 0)
								{
									(*_task->return_handler)(ret_code);
								}
						
								if (!_task->loop)
								{
									_task->initialized = false;
								}
						
								_task->attached_thread = 0;
							}
					
							_task->last_execute = pOS_scheduler::get_tick();
							_thread->attached_task = 0;
						}
					}
				}
			}
		}
	}
}

void thread_3(pOS_thread_id thread)
{
	pOS_thread* _thread = pOS_scheduler::get_thread(thread);
	if (_thread == 0)
		while (1) ;
	int32_t ret_code = 0;
	while (true)
	{
		if (_thread != 0)
		{
			if (_thread->initialized)
			{
				if (_thread->enabled)
				{
					if (_thread->attached_task != 0)
					{
						pOS_task* _task = (pOS_task*)_thread->attached_task;
						if (_task != 0)
						{
							if (_task->function_handler != 0)
							{
								ret_code = (*_task->function_handler)();
								if (_task->return_handler != 0)
								{
									(*_task->return_handler)(ret_code);
								}
						
								if (!_task->loop)
								{
									_task->initialized = false;
								}
						
								_task->attached_thread = 0;
							}
					
							_task->last_execute = pOS_scheduler::get_tick();
							_thread->attached_task = 0;
						}
					}
				}
			}
		}
	}
}

void thread_4(pOS_thread_id thread)
{
	pOS_thread* _thread = pOS_scheduler::get_thread(thread);
	if (_thread == 0)
		while (1) ;
	int32_t ret_code = 0;
	while (true)
	{
		if (_thread != 0)
		{
			if (_thread->initialized)
			{
				if (_thread->enabled)
				{
					if (_thread->attached_task != 0)
					{
						pOS_task* _task = (pOS_task*)_thread->attached_task;
						if (_task != 0)
						{
							if (_task->function_handler != 0)
							{
								ret_code = (*_task->function_handler)();
								if (_task->return_handler != 0)
								{
									(*_task->return_handler)(ret_code);
								}
						
								if (!_task->loop)
								{
									_task->initialized = false;
								}
						
								_task->attached_thread = 0;
							}
					
							_task->last_execute = pOS_scheduler::get_tick();
							_thread->attached_task = 0;
						}
					}
				}
			}
		}
	}
}

void thread_5(pOS_thread_id thread)
{
	pOS_thread* _thread = pOS_scheduler::get_thread(thread);
	if (_thread == 0)
		while (1) ;
	int32_t ret_code = 0;
	while (true)
	{
		if (_thread != 0)
		{
			if (_thread->initialized)
			{
				if (_thread->enabled)
				{
					if (_thread->attached_task != 0)
					{
						pOS_task* _task = (pOS_task*)_thread->attached_task;
						if (_task != 0)
						{
							if (_task->function_handler != 0)
							{
								ret_code = (*_task->function_handler)();
								if (_task->return_handler != 0)
								{
									(*_task->return_handler)(ret_code);
								}
						
								if (!_task->loop)
								{
									_task->initialized = false;
								}
						
								_task->attached_thread = 0;
							}
					
							_task->last_execute = pOS_scheduler::get_tick();
							_thread->attached_task = 0;
						}
					}
				}
			}
		}
	}
}

void thread_6(pOS_thread_id thread)
{
	pOS_thread* _thread = pOS_scheduler::get_thread(thread);
	if (_thread == 0)
		while (1) ;
	int32_t ret_code = 0;
	while (true)
	{
		if (_thread != 0)
		{
			if (_thread->initialized)
			{
				if (_thread->enabled)
				{
					if (_thread->attached_task != 0)
					{
						pOS_task* _task = (pOS_task*)_thread->attached_task;
						if (_task != 0)
						{
							if (_task->function_handler != 0)
							{
								ret_code = (*_task->function_handler)();
								if (_task->return_handler != 0)
								{
									(*_task->return_handler)(ret_code);
								}
						
								if (!_task->loop)
								{
									_task->initialized = false;
								}
						
								_task->attached_thread = 0;
							}
					
							_task->last_execute = pOS_scheduler::get_tick();
							_thread->attached_task = 0;
						}
					}
				}
			}
		}
	}
}

void thread_7(pOS_thread_id thread)
{
	pOS_thread* _thread = pOS_scheduler::get_thread(thread);
	if (_thread == 0)
		while (1) ;
	int32_t ret_code = 0;
	while (true)
	{
		if (_thread != 0)
		{
			if (_thread->initialized)
			{
				if (_thread->enabled)
				{
					if (_thread->attached_task != 0)
					{
						pOS_task* _task = (pOS_task*)_thread->attached_task;
						if (_task != 0)
						{
							if (_task->function_handler != 0)
							{
								ret_code = (*_task->function_handler)();
								if (_task->return_handler != 0)
								{
									(*_task->return_handler)(ret_code);
								}
						
								if (!_task->loop)
								{
									_task->initialized = false;
								}
						
								_task->attached_thread = 0;
							}
					
							_task->last_execute = pOS_scheduler::get_tick();
							_thread->attached_task = 0;
						}
					}
				}
			}
		}
	}
}

void thread_8(pOS_thread_id thread)
{
	pOS_thread* _thread = pOS_scheduler::get_thread(thread);
	if (_thread == 0)
		while (1) ;
	int32_t ret_code = 0;
	while (true)
	{
		if (_thread != 0)
		{
			if (_thread->initialized)
			{
				if (_thread->enabled)
				{
					if (_thread->attached_task != 0)
					{
						pOS_task* _task = (pOS_task*)_thread->attached_task;
						if (_task != 0)
						{
							if (_task->function_handler != 0)
							{
								ret_code = (*_task->function_handler)();
								if (_task->return_handler != 0)
								{
									(*_task->return_handler)(ret_code);
								}
						
								if (!_task->loop)
								{
									_task->initialized = false;
								}
						
								_task->attached_thread = 0;
							}
					
							_task->last_execute = pOS_scheduler::get_tick();
							_thread->attached_task = 0;
						}
					}
				}
			}
		}
	}
}