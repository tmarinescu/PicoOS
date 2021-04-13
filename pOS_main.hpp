#ifndef _POS_MAIN_H_
#define _POS_MAIN_H_

#include "pico/stdlib.h"

#define MAX_STACK_PER_THREAD 1024
#define NUM_OF_THREADS 8
#define TOTAL_MAXIMUM_STACK NUM_OF_THREADS * MAX_STACK_PER_THREAD
#define NUM_OF_TASKS 64

#define MEMORY_POOL 256
#define MEMORY_META_POOL 128

/* Used for our custom dev board */
#define BOARD_PICO_ZERO
//#define BOARD_PICO_ONE

#define HANDSHAKE_TIMEOUT 2500

#endif