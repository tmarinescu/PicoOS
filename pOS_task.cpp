#include "pOS_task.hpp"

pOS_task::pOS_task()
{
	id = 0;
	function_handler = 0;
	return_handler = 0;
	attached_thread = 0;
	last_execute = 0;
	next_execute = 0;
	execute_time = 0;
	quanta = 0;
	quanta_max = 0;
	priority = pOS_task_priority::normal;
	initialized = false;
	enabled = false;
	loop = false;
	memory_warning = false;
	black_listed = false;
}

pOS_task::~pOS_task()
{
	
}