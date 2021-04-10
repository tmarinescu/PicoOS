#ifndef _POS_MAIN_H_
#define _POS_MAIN_H_

#include "pico/stdlib.h"

extern "C" uint32_t main_tracker;

#define MAX_STACK_PER_THREAD 1024
#define NUM_OF_THREADS 8
#define TOTAL_MAXIMUM_STACK NUM_OF_THREADS * MAX_STACK_PER_THREAD
#define NUM_OF_TASKS 32

#endif