#ifndef _POS_GPIO_H_
#define _POS_GPIO_H_

#include "pOS_main.hpp"
#include "pOS_enums.hpp"

class pOS_gpio
{
private:
	uint32_t _pin;
	bool _state;
	static pOS_gpio _gpios[30];
	
public:
	pOS_gpio* set_type(pOS_gpio_type type);
	pOS_gpio* set_function(pOS_gpio_function func);
	pOS_gpio* pull_up();
	pOS_gpio* pull_down();
	pOS_gpio* enable();
	pOS_gpio* disable();
	pOS_gpio* toggle();
	bool read();
	
	static bool initialize_all();
	static pOS_gpio* get(uint32_t pin);
};

#endif