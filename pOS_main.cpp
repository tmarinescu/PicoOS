#include "pOS_main.hpp"
#include "pOS_scheduler.hpp"
#include "pOS_gpio.hpp"
#include "pOS_memory.hpp"
#include "pOS_critical_section.hpp"
#include "pOS_mutex.hpp"
#include "pOS_communication.hpp"
#include "pOS_multicore.hpp"
#include "pOS_utilities.hpp"
#include "pOS_memory_protection.hpp"
#include "pOS_display.hpp"
#include "pOS_semaphore.hpp"

/* Global mutex for testing */
pOS_mutex g_mutex;
pOS_mutex g_uart_mutex;

#ifdef ENABLE_TFT_DISPLAY
volatile uint32_t g_draw_frame = 0;
#endif

/* Critical section for testing */
pOS_critical::pOS_critical_section g_crit_sec;

/* Simple task example */
int32_t simple_loop_task()
{
	static uint32_t inc1 = 0;
	g_mutex.lock();
	inc1++;
	g_mutex.unlock();
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
	
	uint8_t* led_run = (uint8_t*)pOS_memory::wait_for_memory_id(MEM_ID_LED_RUNNING);
	if (*led_run == 0)
		return 0;
	uint8_t* led_state = (uint8_t*)pOS_memory::wait_for_memory_id(MEM_ID_LED_STATE);
	uint32_t* led_fade = (uint32_t*)pOS_memory::wait_for_memory_id(MEM_ID_LED_FADE);
	
	inc2++;
	
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
	pOS_scheduler::sleep(1000); /* 1 second */
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
	g_crit_sec.enter();
	/* Allocate the IDs */
	void* ptr1 = pOS_memory::allocate(MEM_ID_LED_FADE, 4);
	void* ptr3 = pOS_memory::allocate(MEM_ID_MCU_STATUS, 4);
	void* ptr2 = pOS_memory::allocate(MEM_ID_LED_STATE, 1);
	void* ptr4 = pOS_memory::allocate(MEM_ID_UART_INPUT, 1);
	void* ptr5 = pOS_memory::allocate(MEM_ID_LED_RUNNING, 1);
	void* ptr6 = pOS_memory::allocate(MEM_ID_BLUETOOTH_INIT, 1);
	
	/* Zero out memory as a test */
	pOS_memory::zero(MEM_ID_LED_FADE); /* By direct ID */
	pOS_memory::zero(ptr2); /* By pointer */
	pOS_memory::zero(ptr3);
	pOS_memory::zero(ptr4);
	pOS_memory::zero(ptr5);
	pOS_memory::zero(MEM_ID_BLUETOOTH_INIT);
	
	/* Initialize default values */
	*((uint32_t*)ptr1) = 0; /* By pointer */
	*((uint8_t*)pOS_memory::get_pointer(MEM_ID_LED_STATE)) = 1; /* By direct ID */
	*((uint32_t*)ptr3) = 0;
	*((uint8_t*)ptr4) = 0;
	*((uint8_t*)ptr5) = 1;
	g_crit_sec.exit();
	return 4;
}

void global_memory_init_task_return(int32_t ret)
{
	if (ret == 4)
	{
		
	}
}

int32_t uart_input_task()
{
#ifdef USE_CUSTOM_PROJECT_DEMO
	uint32_t* mcu_ptr = (uint32_t*)pOS_memory::wait_for_memory_id(MEM_ID_MCU_STATUS);
	
	if (*mcu_ptr == 0)
	{
		/* MCU handshake didn't succeed/timeout */
		return 0;
	}
#endif
	
	uint8_t* uart_ptr = (uint8_t*)pOS_memory::wait_for_memory_id(MEM_ID_UART_INPUT);
	
	if (*uart_ptr == 0) /* First time printing */
	{
		pOS_communication_terminal::print_string((uint8_t*)"picoOS>>");
		*uart_ptr = 1;
	}
		
	uint8_t chr = pOS_communication_terminal::wait_for_input();
	if (chr == '\r')
	{
		pOS_communication_terminal::interpret_command();
		pOS_communication_terminal::print_string((uint8_t*)"\npicoOS>>");
	}
	else
	{
		if (chr != 0) /* Check if character is printable */
		{
			pOS_communication_terminal::print_char(chr);
		}
	}
	return 0;
}

#ifdef USE_CUSTOM_PROJECT_DEMO
int32_t wait_for_other_board()
{
	uint32_t* mcu_status = (uint32_t*)pOS_memory::wait_for_memory_id(MEM_ID_MCU_STATUS);
	
	pOS_communication_terminal::print_string((uint8_t*)"Searching for MCU handshake...");
	bool value = false;//pOS_communication_mcu::initialize(uart0, 16, 17);
	if (!value)
	{
		pOS_communication_terminal::print_string((uint8_t*)"\nMCU handshake timed out!\n\n");
		*mcu_status = 2;
	}
	else
	{
		pOS_communication_terminal::print_string((uint8_t*)"\nMCU handshake successful!\n\n");
		*mcu_status = 1;
	}
	return 0;
}
#endif

#ifdef ENABLE_TFT_DISPLAY
int32_t drawing_task()
{
	static uint32_t x = 0;
	static uint32_t x2 = 0;
	static uint32_t bounce = 1;
	static uint32_t bounce2 = 1;
	static uint32_t width = 8;
	
	if (bounce == 1)
	{
		x+=2;
		if (x >= 240 - (width * 3))
			bounce = 0;
	}
	else
	{
		x-=2;
		if (x <= 0)
			bounce = 1;
	}
	
	if (bounce2 == 1)
	{
		x2 += 4;
		if (x2 >= 240 - (width * 3))
			bounce2 = 0;
	}
	else
	{
		x2 -= 4;
		if (x2 <= 0)
			bounce2 = 1;
	}
	
	while (g_draw_frame != 0) ;
	pOS_display::fill_background(0xFFFF);
	
	for (uint32_t i = 0; i < 20; i++)
	{
		pOS_display::fill_rect(x, 0 + (width * i * 2), width, width, 0x00F8);
		pOS_display::fill_rect(x + width, 0 + (width * i * 2), width, width, 0xE007);
		pOS_display::fill_rect(x + (width * 2), 0 + (width * i * 2), width, width, 0x1F00) ;
		pOS_display::fill_rect(240 - (width * 3) - x, 0 + (width * i * 2), width, width, 0x00F8);
		pOS_display::fill_rect(240 - (width * 3) - x + width, 0 + (width * i * 2), width, width, 0xE007);
		pOS_display::fill_rect(240 - (width * 3) - x + (width * 2), 0 + (width * i * 2), width, width, 0x1F00);
	
		pOS_display::fill_rect(x2, width + (width * i * 2), width, width, 0x00F8);
		pOS_display::fill_rect(x2 + width, width + (width * i * 2), width, width, 0xE007);
		pOS_display::fill_rect(x2 + (width * 2), width + (width * i * 2), width, width, 0x1F00) ;
		pOS_display::fill_rect(240 - (width * 3) - x2, width + (width * i * 2), width, width, 0x00F8);
		pOS_display::fill_rect(240 - (width * 3) - x2 + width, width + (width * i * 2), width, width, 0xE007);
		pOS_display::fill_rect(240 - (width * 3) - x2 + (width * 2), width + (width * i * 2), width, width, 0x1F00);
	}
	
	pOS_display::draw_text();
	g_draw_frame = 1;
	return 0;
}
#endif

int32_t bluetooth_task()
{
	pOS_bluetooth::receive_data();
	return 0;
}

int main() 
{
#ifdef ENABLE_TFT_DISPLAY
	/* Initialize display, leave here since it uses sleep for now */
	pOS_display::initialize(spi0, 16, 18, 19, 13, 11, 12);
#endif
	
	/* Init core 1 */
	core1_init();
	
	/* Disable all interrupts*/
	pOS_critical::disable_all_interrupts();

	/* Initialize MPU */
	pOS_memory_protection::initialize();

	/* Initialize memory manager */
	pOS_memory::initialize();
	
	/* Initialize GPIO system and LED */
	pOS_gpio::initialize_all();
	pOS_gpio::get(25)->set_function(pOS_gpio_function::pwm);
	
	/* Initialize the serial output */
	pOS_communication_terminal::initialize(uart1, 4, 5);
	pOS_communication_terminal::clear_terminal();
	pOS_communication_terminal::reset_buffer();
	
	/* Print debug info if debug printing is enabled */
	pOS_utilities::debug_print((uint8_t*)"Debug printing enabled\n\n");
	
	/* Bluetooth */
	pOS_utilities::debug_print((uint8_t*)"Initializing bluetooth...\n");
	pOS_bluetooth::initialize(uart0, 16, 17);
	pOS_utilities::debug_print((uint8_t*)"Initialized\n");
	
	/* Initialize scheduler */
	pOS_scheduler::initialize();
	pOS_memory_protection::set_mpu_unavailable();
	
	/* Link all the threads to the functions and IDs */
	pOS_scheduler::link_thread(0, &thread_1);
	pOS_scheduler::link_thread(1, &thread_2);
	pOS_scheduler::link_thread(2, &thread_3);
	pOS_scheduler::link_thread(3, &thread_4);
	pOS_scheduler::link_thread(4, &thread_5);
	pOS_scheduler::link_thread(5, &thread_6);
	pOS_scheduler::link_thread(6, &thread_7);
	pOS_scheduler::link_thread(7, &thread_8);

	/* Initialize thread stacks */
	for (uint32_t i = 0; i < NUM_OF_THREADS; i++)
		pOS_scheduler::initialize_thread(i, pOS_stack_size::byte_1024);

	/* Enable all threads */
	for (uint32_t i = 0; i < NUM_OF_THREADS; i++)
		pOS_scheduler::enable_thread(i);
	
	/* Add some random tasks */
	uint32_t id = 0;
	pOS_scheduler::create_task(&global_memory_init_task, /* Main function */
		&global_memory_init_task_return, /* Return function */
		pOS_task_quanta::small, /* Quanta size */
		pOS_task_priority::normal, /* Unused */
		&id); /* Get ID */
	pOS_scheduler::enable_task(id);
	
	pOS_scheduler::create_task(&simple_loop_task, 
		&simple_loop_task_return, 
		pOS_task_quanta::small, 
		pOS_task_priority::normal, 
		&id, 
		true); /* Loop task */
	pOS_scheduler::enable_task(id);
	
	pOS_scheduler::create_task(&led_pwm_fade_task, 
		&led_pwm_fade_task_return, 
		pOS_task_quanta::small, 
		pOS_task_priority::normal, 
		&id, 
		true);
	pOS_scheduler::enable_task(id);
	
	pOS_scheduler::create_task(&delayed_loop_task, 
		&delayed_loop_task_return, 
		pOS_task_quanta::small,
		pOS_task_priority::normal, 
		&id, 
		true);
	pOS_scheduler::enable_task(id);
	
	pOS_scheduler::create_task(&uart_input_task, 
		0, 
		pOS_task_quanta::small, 
		pOS_task_priority::normal, 
		&id, 
		true);
	pOS_scheduler::enable_task(id);
	
#ifdef USE_CUSTOM_PROJECT_DEMO
	pOS_scheduler::create_task(&wait_for_other_board, 
		0, 
		pOS_task_quanta::normal, 
		pOS_task_priority::normal, 
		&id);
	pOS_scheduler::enable_task(id);
#endif
	
#ifdef ENABLE_TFT_DISPLAY

	pOS_scheduler::create_task(&drawing_task,
		0,
		pOS_task_quanta::normal,
		pOS_task_priority::normal,
		&id,
		true);
	pOS_scheduler::enable_task(id);
	
#endif
	
	pOS_scheduler::create_task(&bluetooth_task,
		0,
		pOS_task_quanta::normal,
		pOS_task_priority::normal,
		&id,
		true);
	pOS_scheduler::enable_task(id);

	/* Boot up the MPU and start the kernel */
	pOS_scheduler::jump_start();
	while (1) ;
	return 0;
}
