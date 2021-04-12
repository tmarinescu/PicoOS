#include "pOS_gpio.hpp"
#include "hardware/adc.h"
#include "hardware/pwm.h"

pOS_gpio pOS_gpio::_gpios[30];
	
bool pOS_gpio::initialize_all()
{
	for (uint32_t i = 0; i < 30; i++)
	{
		_gpios[i]._pin = i;
		_gpios[i]._state = false;
	}
	
	return true;
}
	
pOS_gpio* pOS_gpio::get(uint32_t pin)
{
	if (pin >= 30)
		return 0;
	return &_gpios[pin];
}

pOS_gpio* pOS_gpio::set_type(pOS_gpio_type type)
{
	gpio_init(_pin);
	if (type == pOS_gpio_type::input)
	{
		gpio_set_dir(_pin, false);
	}
	else
	{
		gpio_set_dir(_pin, true);
	}
	
	return this;
}
	
pOS_gpio* pOS_gpio::set_function(pOS_gpio_function func)
{
	if (func == pOS_gpio_function::uart)
	{
		gpio_set_function(_pin, GPIO_FUNC_UART);
	}
	else if (func == pOS_gpio_function::i2c)
	{
		gpio_set_function(_pin, GPIO_FUNC_I2C);
	}
	else if (func == pOS_gpio_function::spi)
	{
		gpio_set_function(_pin, GPIO_FUNC_SPI);
	}
	else if (func == pOS_gpio_function::adc)
	{
		adc_gpio_init(_pin);
	}
	else if (func == pOS_gpio_function::pwm)
	{
		gpio_set_function(_pin, GPIO_FUNC_PWM);
		uint32_t slice = pwm_gpio_to_slice_num(_pin);
		pwm_config config = pwm_get_default_config();
		pwm_config_set_clkdiv(&config, 4.0f);
		pwm_init(slice, &config, true);
	}
	
	return this;
}

pOS_gpio* pOS_gpio::set_pwm(uint32_t value)
{
	pwm_set_gpio_level(_pin, value * value);
	return this;
}
	
pOS_gpio* pOS_gpio::pull_up()
{
	gpio_pull_up(_pin);
	return this;
}
	
pOS_gpio* pOS_gpio::pull_down()
{
	gpio_pull_down(_pin);
	return this;
}
	
pOS_gpio* pOS_gpio::enable()
{
	gpio_put(_pin, true);
	return this;
}
	
pOS_gpio* pOS_gpio::disable()
{
	gpio_put(_pin, false);
	return this;
}

pOS_gpio* pOS_gpio::toggle()
{
	if (read())
		gpio_put(_pin, false);
	else
		gpio_put(_pin, true);
	return this;
}

bool pOS_gpio::read()
{
	return gpio_get(_pin);
}
	