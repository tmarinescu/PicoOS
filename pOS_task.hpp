#ifndef _POS_TASK_H_
#define _POS_TASK_H_

#include "pOS_main.hpp"
#include "pOS_enums.h"

class pOS_task
{
public:
	uint32_t id;
	
	int32_t(*volatile function_handler)(void);		//Caller function
	void(*volatile return_handler)(int32_t ret);	//Return handler
	uint32_t* attached_thread;
	
	uint32_t last_execute;
	uint32_t next_execute;
	uint32_t execute_time;
	uint32_t quanta;
	uint32_t quanta_max;
	
	pOS_task_priority priority;
	
	bool initialized;
	bool enabled;
	bool loop;
	bool memory_warning;
	bool black_listed;
	
	pOS_task();
	~pOS_task();
};

#endif