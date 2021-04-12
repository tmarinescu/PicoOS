#include "pOS_main.hpp"
#include "pOS_scheduler.hpp"
#include "pOS_gpio.hpp"
#include "pOS_memory.hpp"


/* Used for LED task */
#define MEM_ID_LED_STATE	0xAABBCCDD
#define MEM_ID_LED_FADE		0xAABBCCCC


/* Simple task example */
int32_t simple_loop_task()
{
	static uint32_t inc1 = 0;
	inc1++;
	return 0;
}

/* Simple task return example */
void simple_loop_task_return(int32_t ret)
{
	if (ret == 0)
	{

	}
}

/* Task that fades the LED on and off */
int32_t led_pwm_fade_task()
{
	static uint32_t inc2 = 0;
	inc2++;
	
	uint8_t* led_state = (uint8_t*)pOS_memory::get_pointer(MEM_ID_LED_STATE);
	uint32_t* led_fade = (uint32_t*)pOS_memory::get_pointer(MEM_ID_LED_FADE);
	
	if (*led_state == 1)
	{
		(*led_fade)++;
	}
	else
	{
		(*led_fade)--;
	}
	
	if (*led_fade > 255)
	{
		*led_fade = 255;
		*led_state = 0;
	}
	else if (*led_fade == 0)
	{
		*led_fade = 0;
		*led_state = 1;
	}
	
	pOS_gpio::get(25)->set_pwm(*led_fade); /* Set the PWM for LED */
	return 1;
}

void led_pwm_fade_task_return(int32_t ret)
{
	if (ret == 1)
	{
		
	}
}

/* Example loop that works with busy_wait */
int32_t delayed_loop_task()
{
	static uint32_t inc3 = 0;
	inc3++;
	busy_wait_us(1000 * 1000); /* 1 second */
	return 3;
}

void delayed_loop_task_return(int32_t ret)
{
	if (ret == 3)
	{
		
	}
}

/* This is an example task used to initialize global pointers */
int32_t global_memory_init_task()
{
	/* Allocate the IDs */
	void* ptr1 = pOS_memory::allocate(MEM_ID_LED_FADE, 4);
	void* ptr2 = pOS_memory::allocate(MEM_ID_LED_STATE, 1);
	
	/* Zero out memory as a test */
	pOS_memory::zero(MEM_ID_LED_FADE);	/* By direct ID */
	pOS_memory::zero(ptr2);				/* By pointer */
	
	/* Initialize default values */
	*((uint32_t*)ptr1) = 0;										/* By pointer */
	*((uint8_t*)pOS_memory::get_pointer(MEM_ID_LED_STATE)) = 1;	/* By direct ID */
	
	return 4;
}

void global_memory_init_task_return(int32_t ret)
{
	if (ret == 4)
	{
		
	}
}


int main() 
{
	/* Disable all interrupts*/
	pOS_disable_interrupts();
	
	pOS_memory::initialize();
	
	/* Initialize GPIO system and LED */
	pOS_gpio::initialize_all();
	pOS_gpio::get(25)->set_function(pOS_gpio_function::pwm);
	//pOS_gpio::get(25)->set_type(pOS_gpio_type::output)->disable();
	
	/* Initialize scheduler */
	pOS_scheduler::initialize();

	/* Link all the threads to the functions and IDs */
	pOS_scheduler::link_thread(pOS_thread_id::communication, 0, &thread_1);
	pOS_scheduler::link_thread(pOS_thread_id::critical, 1, &thread_2);
	pOS_scheduler::link_thread(pOS_thread_id::external, 2, &thread_3);
	pOS_scheduler::link_thread(pOS_thread_id::system, 3, &thread_4);
	pOS_scheduler::link_thread(pOS_thread_id::general_1, 4, &thread_5);
	pOS_scheduler::link_thread(pOS_thread_id::general_2, 5, &thread_6);
	pOS_scheduler::link_thread(pOS_thread_id::general_3, 6, &thread_7);
	pOS_scheduler::link_thread(pOS_thread_id::general_4, 7, &thread_8);

	/* Initialize thread stacks */
	pOS_scheduler::initialize_thread(pOS_thread_id::communication, pOS_thread_size::byte_128);
	pOS_scheduler::initialize_thread(pOS_thread_id::critical, pOS_thread_size::byte_128);
	pOS_scheduler::initialize_thread(pOS_thread_id::external, pOS_thread_size::byte_128);
	pOS_scheduler::initialize_thread(pOS_thread_id::system, pOS_thread_size::byte_128);
	pOS_scheduler::initialize_thread(pOS_thread_id::general_1, pOS_thread_size::byte_128);
	pOS_scheduler::initialize_thread(pOS_thread_id::general_2, pOS_thread_size::byte_128);
	pOS_scheduler::initialize_thread(pOS_thread_id::general_3, pOS_thread_size::byte_128);
	pOS_scheduler::initialize_thread(pOS_thread_id::general_4, pOS_thread_size::byte_128);

	/* Enable all threads */
	pOS_scheduler::enable_thread(pOS_thread_id::communication);
	pOS_scheduler::enable_thread(pOS_thread_id::critical);
	pOS_scheduler::enable_thread(pOS_thread_id::external);
	pOS_scheduler::enable_thread(pOS_thread_id::system);
	pOS_scheduler::enable_thread(pOS_thread_id::general_1);
	pOS_scheduler::enable_thread(pOS_thread_id::general_2);
	pOS_scheduler::enable_thread(pOS_thread_id::general_3);
	pOS_scheduler::enable_thread(pOS_thread_id::general_4);

	/* Add some random tasks */
	uint32_t id = 0;
	pOS_scheduler::create_task(&simple_loop_task, &simple_loop_task_return, pOS_task_priority::normal, &id, true, 0);
	pOS_scheduler::enable_task(id);
	pOS_scheduler::create_task(&led_pwm_fade_task, &led_pwm_fade_task_return, pOS_task_priority::normal, &id, true, 0);
	pOS_scheduler::enable_task(id);
	pOS_scheduler::create_task(&delayed_loop_task, &delayed_loop_task_return, pOS_task_priority::normal, &id, true, 0);
	pOS_scheduler::enable_task(id);
	pOS_scheduler::create_task(&global_memory_init_task, &global_memory_init_task_return, pOS_task_priority::normal, &id);
	pOS_scheduler::enable_task(id);
	
	/* Start the kernel */
	pOS_scheduler::jump_start();
	while (1) ;
	return 0;
}
