#ifndef _POS_DISPLAY_H_
#define _POS_DISPLAY_H_

/* ILI9341*/

#include "pOS_main.hpp"

#ifdef ENABLE_TFT_DISPLAY
#include "hardware/spi.h"

#define WIDTH (uint16_t)240
#define HEIGHT (uint16_t)320

class pOS_display
{
private:
	static uint16_t _buffer[WIDTH * HEIGHT];
	static uint32_t spi_dc_pin;
	static uint32_t spi_cs_pin;
	static uint32_t spi_reset_pin;
	static spi_inst_t* spi_block;
	
public:
	static void initialize(spi_inst_t* spi, uint32_t spi_rx, uint32_t spi_tx, uint32_t spi_clk, uint32_t spi_dc, uint32_t spi_cs, uint32_t spi_reset);
	
	static void fill_rect(uint16_t x, uint16_t y, uint16_t width, uint32_t height, uint16_t color);
	static void fill_background(uint16_t color);
	static void draw_text();
	
	static void push_command(uint8_t cmd);
	static void push_parameter(uint8_t data);
	static void push_data(void* buffer, uint32_t bytes);
	
	static void draw_frame();
};

#endif
#endif
