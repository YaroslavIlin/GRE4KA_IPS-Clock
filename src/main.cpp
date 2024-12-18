//#ifndef FORCE_INLINE_ATTR
//#define FORCE_INLINE_ATTR
//#endif
#include <stdio.h>
#include <sdkconfig.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>

#include <cstring>

#include <fixic.hpp>
#include <display.hpp>

uint16_t array[240 * 320];

[[noreturn]]
void task_display(void *){
	//gpio_set_direction(DISPLAY_LED, GPIO_MODE_OUTPUT);
	//gpio_set_level(DISPLAY_LED, 0);
	gpio_set_direction(DISPLAY_RESET, GPIO_MODE_OUTPUT);
	gpio_set_level(DISPLAY_RESET, 0);

	Displays displays;

	auto get_color = [](uint8_t red, uint8_t green, uint8_t blue){
		uint16_t r = 0;
		red &= 0x1f;
		blue &= 0x1f;
		green &= 0x2f;
		//Надеюсь что esp32 little-endian)...
		*reinterpret_cast<uint8_t *>(&r) = blue;
		*reinterpret_cast<uint8_t *>(&r) += green << 5;
		*reinterpret_cast<uint8_t *>(&r + 1) = green >> 3;
		*reinterpret_cast<uint8_t *>(&r + 1) += red << 3;
		return r;
	};
	for(size_t i = 0; i < 240 * 320; ++i)
		array[i] = 0xffff;
	size_t seconds = 0;
	while(1){
		printf("loop entered\n");
		for(size_t i = 0; i < 4; ++i)
			displays.drawFullImage(i, reinterpret_cast<const uint16_t *>(pix0_data));
		printf("drawed image\n");
		vTaskDelay(pdMS_TO_TICKS(1000));
		++seconds;
	}
}



extern "C"
void app_main(void){
	printf("FreeRTOS started!\n");
	//xTaskCreate(&task_printer, "task_printer", 2048, nullptr, 5, nullptr);
	xTaskCreate(&task_display, "task_display", 8192, nullptr, 5, nullptr);
}
