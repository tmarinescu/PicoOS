#ifndef _POS_SEMAPHORE_H_
#define _POS_SEMAPHORE_H_

#include "pOS_main.hpp"

class pOS_semaphore
{
public:
	static void initialize_semaphore(int32_t* semaphore, int32_t value);
	static void set_semaphore(int32_t* semaphore);
	static void unset_semaphore(int32_t* semaphore);
	static void wait_semaphore(int32_t* semaphore);
};

#endif