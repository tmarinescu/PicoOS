#include "pOS_display.hpp"

#ifdef ENABLE_TFT_DISPLAY

uint16_t pOS_display::_buffer[WIDTH * HEIGHT];

uint32_t pOS_display::spi_dc_pin;
uint32_t pOS_display::spi_cs_pin;
uint32_t pOS_display::spi_reset_pin;
spi_inst_t* pOS_display::spi_block;

void pOS_display::initialize(spi_inst_t* spi, uint32_t spi_rx, uint32_t spi_tx, uint32_t spi_clk, uint32_t spi_dc, uint32_t spi_cs, uint32_t spi_reset)
{
	spi_dc_pin = spi_dc;
	spi_cs_pin = spi_cs;
	spi_reset_pin = spi_reset;
	spi_block = spi;
	
	for (uint32_t i = 0; i < WIDTH * HEIGHT; i++)
		_buffer[i] = 0x00F8;
	
	spi_init(spi, 62500 * 1000);
	gpio_set_function(spi_rx, GPIO_FUNC_SPI);
	gpio_set_function(spi_tx, GPIO_FUNC_SPI);
	gpio_set_function(spi_clk, GPIO_FUNC_SPI);
	
	gpio_init(spi_cs);
	gpio_put(spi_cs, 0);
	gpio_set_dir(spi_cs, GPIO_OUT);
	
	gpio_init(spi_dc);
	gpio_put(spi_dc, 0);
	gpio_set_dir(spi_dc, GPIO_OUT);
	
	gpio_init(spi_reset);
	gpio_put(spi_reset, 1);
	gpio_set_dir(spi_reset, GPIO_OUT);
	
	sleep_ms(10);
	gpio_put(spi_reset, 0);
	sleep_ms(10);
	gpio_put(spi_reset, 1);
	
	push_command(0x01);
	sleep_ms(100);
	
	push_command(0xB6);
	push_parameter(0x0A);
	push_parameter(0x82);
	push_parameter(0x27);
	
	push_command(0xF2);   //Disable 3 gamma
	push_parameter(0x02);   //2 is disable you fucking retards, 3 is enable
	
	push_command(0x26);
	push_parameter(0x01);
	
	push_command(0xE0);
	push_parameter(0x0f);
	push_parameter(0x31);
	push_parameter(0x2b);
	push_parameter(0x0c);
	push_parameter(0x0e);
	push_parameter(0x08);
	push_parameter(0x4e);
	push_parameter(0xf1);
	push_parameter(0x37);
	push_parameter(0x07);
	push_parameter(0x10);
	push_parameter(0x03);
	push_parameter(0x0e);
	push_parameter(0x09);
	push_parameter(0x00);
	
	push_command(0xE1);
	push_parameter(0x00);
	push_parameter(0x0E);
	push_parameter(0x14);
	push_parameter(0x03);
	push_parameter(0x11);
	push_parameter(0x07);
	push_parameter(0x31);
	push_parameter(0xC1);
	push_parameter(0x48);
	push_parameter(0x08);
	push_parameter(0x0F);
	push_parameter(0x0C);
	push_parameter(0x31);
	push_parameter(0x36);
	push_parameter(0x0F);
	
	push_command(0x36);
	push_parameter(0x48);
	
	push_command(0x3A);
	push_parameter(0x55);
	
	push_command(0xB1);
	push_parameter(0x00);
	push_parameter(0x15);   //90Hz
	
	push_command(0x11);
	sleep_ms(120);
	push_command(0x29);
	
	push_command(0x2A);
	push_parameter(0x00);
	push_parameter(0x00);      // start column
	push_parameter(0x00);
	push_parameter(0xEF);      // end column -> 239
	
	push_command(0x2B);
	push_parameter(0x00);
	push_parameter(0x00);       // start column
	push_parameter(0x01);
	push_parameter(0x3F);       // end column -> 239
	
	push_command(0x2C);
}
	
void pOS_display::fill_rect(uint16_t x, uint16_t y, uint16_t width, uint32_t height, uint16_t color)
{
	for (uint32_t i = x; i < width + x; i++)
		for (uint32_t j = y; j < height + y; j++)
			_buffer[WIDTH * i + j] = color;
}
	
void pOS_display::fill_background(uint16_t color)
{
	for (uint32_t i = 0; i < WIDTH * HEIGHT; i++)
		_buffer[i] = color;
}
	
void pOS_display::push_command(uint8_t cmd)
{
	asm volatile("nop \n nop \n nop");
	gpio_put(spi_cs_pin, 0);
	asm volatile("nop \n nop \n nop");
	
	gpio_put(spi_dc_pin, 0);
	spi_write_blocking(spi_block, &cmd, 1);
	gpio_put(spi_dc_pin, 1);
	
	asm volatile("nop \n nop \n nop");
    gpio_put(spi_cs_pin, 1);
	asm volatile("nop \n nop \n nop");
}
	
void pOS_display::push_parameter(uint8_t data)
{
	asm volatile("nop \n nop \n nop");
	gpio_put(spi_cs_pin, 0);
	asm volatile("nop \n nop \n nop");
	
	spi_write_blocking(spi_block, &data, 1);
	
	asm volatile("nop \n nop \n nop");
	gpio_put(spi_cs_pin, 1);
	asm volatile("nop \n nop \n nop");
}
	
void pOS_display::push_data(void* buffer, uint32_t bytes)
{
	asm volatile("nop \n nop \n nop");
	gpio_put(spi_cs_pin, 0);
	asm volatile("nop \n nop \n nop");
	
	spi_write_blocking(spi_block, (uint8_t*)buffer, bytes);
	
	asm volatile("nop \n nop \n nop");
	gpio_put(spi_cs_pin, 1);
	asm volatile("nop \n nop \n nop");
}
	
void pOS_display::draw_frame()
{
	push_command(0x2A);
	push_parameter(0);
	push_parameter(0);
	push_parameter(WIDTH >> 8);
	push_parameter(WIDTH);
	
	push_command(0x2B);
	push_parameter(0);
	push_parameter(0);
	push_parameter(HEIGHT >> 8);
	push_parameter(HEIGHT);
	
	push_command(0x2C);
	
	push_data(_buffer, WIDTH * HEIGHT * 2);
}

#endif