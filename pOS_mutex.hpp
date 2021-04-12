#ifndef _POS_MUTEX_H_
#define _POS_MUTEX_H_

#include "pOS_main.hpp"

class pOS_mutex
{
private:
	volatile int32_t _semaphore;
	
public:
	pOS_mutex();
	volatile bool locked;
	void lock() volatile;
	void unlock() volatile;
};

class pOS_auto_mutex
{
public:
	pOS_mutex* main_mutex;
	
	pOS_auto_mutex(pOS_mutex* mutex);
	~pOS_auto_mutex();
};

#endif