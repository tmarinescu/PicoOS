#ifndef _POS_MAIN_H_
#define _POS_MAIN_H_

#include "pico/stdlib.h"

/* Registers */
#define AIRCR_Register (*((volatile uint32_t*)(0xE0000000 + 0xED0C)))
#define MPU_TYPE_Register (*((volatile uint32_t*)(0xE0000000 + 0xED90)))
#define MPU_CTRL_Register (*((volatile uint32_t*)(0xE0000000 + 0xED94)))
#define MPU_RNR_Register (*((volatile uint32_t*)(0xE0000000 + 0xED98)))
#define MPU_RBAR_Register (*((volatile uint32_t*)(0xE0000000 + 0xED9C)))
#define MPU_RASR_Register (*((volatile uint32_t*)(0xE0000000 + 0xEDA0)))

/* Scheduler stuff */
#define MAX_STACK_PER_THREAD 1024
#define NUM_OF_THREADS 8
#define TOTAL_MAXIMUM_STACK (NUM_OF_THREADS + 1) * MAX_STACK_PER_THREAD /* Extra thread stack is being used for other things */
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
#define USE_CUSTOM_PROJECT_DEMO

#define HANDSHAKE_TIMEOUT 2500

#endif