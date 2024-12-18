#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include <driver/gpio.h>
#include <driver/spi_master.h>

#define DISPLAY_BUS_SCLK	GPIO_NUM_18
#define DISPLAY_BUS_MOSI	GPIO_NUM_19
#define DISPLAY_BUS_DC		GPIO_NUM_12
#define DISPLAY_CS0				GPIO_NUM_13
#define DISPLAY_CS1				GPIO_NUM_15
#define DISPLAY_CS2				GPIO_NUM_2
#define DISPLAY_CS3				GPIO_NUM_17
#define DISPLAY_RESET			GPIO_NUM_27

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320

inline
void delayMS(size_t ms){
	vTaskDelay(pdMS_TO_TICKS(ms));
}


class Displays{
public:
	Displays();
	void drawFullImage(size_t n, const uint16_t *data);
private:
	void init_spi();
	void init_st7789();
private:
	//spi_device_handle_t m_handles[4]; //Эта тема пока не работает
	spi_device_handle_t m_handle;
	gpio_num_t m_cs[4];
};


#endif
