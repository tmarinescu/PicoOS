#include "pOS_main.hpp"
#include "pOS_scheduler.hpp"
#include "pOS_gpio.hpp"

uint32_t main_tracker = 0; /* track systick */

/* Task 1 Example */
uint32_t inc1 = 0;
int32_t task_1()
{
	inc1++;
	return 0;
}

/* Task 1 Return Example */
void task_1_return(int32_t ret)
{
	if (ret == 0)
	{
		
	}
}

/* Task 2 Example */
uint32_t inc2 = 0;
int32_t task_2()
{
	inc2++;
	busy_wait_us(1000 * 1000);
	pOS_gpio::get(25)->toggle();
	return 1;
}

/* Task 2 Return Example */
void task_2_return(int32_t ret)
{
	if (ret == 1)
	{
		
	}
}

/* Task 3 Example */
uint32_t inc3 = 0;
int32_t task_3()
{
	inc3++;
	busy_wait_us(2000 * 1000);
	return 3;
}

/* Task 3 Return Example */
void task_3_return(int32_t ret)
{
	if (ret == 3)
	{
		
	}
}


int main() 
{
	/* Disable all interrupts*/
	__asm volatile("cpsid if" : : : "memory");
	
	/* Initialize GPIO system and LED */
	pOS_gpio::initialize_all();
	pOS_gpio::get(25)->set_type(pOS_gpio_type::output)->disable();
	
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
	pOS_scheduler::create_task(&task_1, &task_1_return, pOS_task_priority::normal, &id, true, 0);
	pOS_scheduler::enable_task(id);
	pOS_scheduler::create_task(&task_2, &task_2_return, pOS_task_priority::normal, &id, true, 0);
	pOS_scheduler::enable_task(id);
	pOS_scheduler::create_task(&task_3, &task_3_return, pOS_task_priority::normal, &id, true, 0);
	pOS_scheduler::enable_task(id);
	
	/* Start the kernel */
	pOS_scheduler::jump_start();
	while (1) ;
	return 0;
}
