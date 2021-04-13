#ifndef _POS_MAIN_H_
#define _POS_MAIN_H_

#include "pico/stdlib.h"

/* Registers */
#define AIRCR_Register (*((volatile uint32_t*)(PPB_BASE + 0x0ED0C)))

/* Scheduler stuff */
#define MAX_STACK_PER_THREAD 1024
#define NUM_OF_THREADS 8
#define TOTAL_MAXIMUM_STACK NUM_OF_THREADS * MAX_STACK_PER_THREAD
#define NUM_OF_TASKS 64

/* Memory manager */
#define MEMORY_POOL 256
#define MEMORY_META_POOL 128

/* Used for task demos */
#define MEM_ID_LED_STATE	0xAABBCCDD
#define MEM_ID_LED_FADE		0xAABBCCCC
#define MEM_ID_MCU_STATUS	0xBBAACCDD
#define MEM_ID_UART_INPUT	0xAAAAAAAA
#define MEM_ID_LED_RUNNING	0xCCCCCCCC

/* Used for our custom dev board */
#define BOARD_PICO_ZERO
//#define BOARD_PICO_ONE

#define HANDSHAKE_TIMEOUT 2500

#endif