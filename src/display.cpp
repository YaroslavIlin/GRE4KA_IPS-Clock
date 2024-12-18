#include <display.hpp>

#include <cstring>
#include <algorithm>

bool spi_master_write_byte(spi_device_handle_t handle, const uint8_t* data, size_t size){
	spi_transaction_t trans;
	esp_err_t ret;
	if(size > 0){
		memset(&trans, 0, sizeof(trans));
		trans.length = size * 8;
		trans.tx_buffer = data;
		ret = spi_device_transmit(handle, &trans);
		assert(ret == ESP_OK); 
	}
	return true;
}
bool spi_master_write_command(spi_device_handle_t handle, uint8_t cmd){
	gpio_set_level(DISPLAY_BUS_DC, 0);
	return spi_master_write_byte(handle, &cmd, 1);
}

bool spi_master_write_data_byte(spi_device_handle_t handle, uint8_t data){
	gpio_set_level(DISPLAY_BUS_DC, 1);
	return spi_master_write_byte(handle, &data, 1);
}


bool spi_master_write_data_word(spi_device_handle_t handle, uint16_t data, int flag){
	uint8_t be[2];
	be[0] = (data >> 8) & 0xFF;
	be[1] = data & 0xFF;
	//if(flag)
	//	printf("spi_master_write_data_word Byte=%02x %02x\n",Byte[0],Byte[1]);
	gpio_set_level(DISPLAY_BUS_DC, 1);
	return spi_master_write_byte(handle, be, 2);
}

bool spi_master_write_addr(spi_device_handle_t handle, uint16_t addr1, uint16_t addr2){
	static uint8_t be[4];
	be[0] = (addr1 >> 8) & 0xFF;
	be[1] = addr1 & 0xFF;
	be[2] = (addr2 >> 8) & 0xFF;
	be[3] = addr2 & 0xFF;
	gpio_set_level(DISPLAY_BUS_DC, 1);
	return spi_master_write_byte(handle, be, 4);
}

bool spi_master_write_color(spi_device_handle_t handle, uint16_t color, uint16_t size){
	static uint8_t be[1024];
	int index = 0;
	for(int i = 0; i < size; ++i){
		be[index++] = (color >> 8) & 0xFF;
		be[index++] = color & 0xFF;
	}
	gpio_set_level(DISPLAY_BUS_DC, 1);
	return spi_master_write_byte(handle, be, size * 2);
}

bool spi_master_write_colors(spi_device_handle_t handle, const uint16_t *colors, uint16_t size){
	static uint8_t be[1024];
	int index = 0;
	for(int i = 0; i < size; ++i) {
		be[index++] = (colors[i] >> 8) & 0xFF;
		be[index++] = colors[i] & 0xFF;
	}
	gpio_set_level(DISPLAY_BUS_DC, 1);
	return spi_master_write_byte(handle, be, size * 2);
}

void draw_pixel(spi_device_handle_t handle, uint16_t x, uint16_t y, uint16_t color){
	spi_master_write_command(handle, 0x2A);	// set column(x) address
	spi_master_write_addr(handle, x, x);
	spi_master_write_command(handle, 0x2B);	// set Page(y) address
	spi_master_write_addr(handle, y, y);
	spi_master_write_command(handle, 0x2C);	//	Memory Write
	spi_master_write_data_word(handle, color, 0);
}

void spi_master_write_monofull(spi_device_handle_t handle, const uint8_t *data){
	spi_master_write_command(handle, 0x2A);	// set column(x) address
	spi_master_write_addr(handle, 0, DISPLAY_WIDTH);
	spi_master_write_command(handle, 0x2B);	// set Page(y) address
	spi_master_write_addr(handle, 0, DISPLAY_HEIGHT);
	spi_master_write_command(handle, 0x2C);	//	Memory Write

	static uint8_t buffer[512];
	size_t bytes_for_row = DISPLAY_WIDTH / 8;

	
}

void st7735_init(spi_device_handle_t &handle){
	//gpio_set_direction(DISPLAY_LED, GPIO_MODE_OUTPUT);
	//gpio_set_level(DISPLAY_LED, 0);

	spi_master_write_command(handle, 0x01);	//Software Reset 
	vTaskDelay(pdMS_TO_TICKS(150));

	spi_master_write_command(handle, 0x11);	//Sleep Out
	vTaskDelay(pdMS_TO_TICKS(255));
	
	spi_master_write_command(handle, 0xB1);	//Frame Rate Control (In normal mode/ Full colors) 
	spi_master_write_data_byte(handle, 0x01);
	spi_master_write_data_byte(handle, 0x2C);
	spi_master_write_data_byte(handle, 0x2D);
	
	spi_master_write_command(handle, 0xB2);	//Frame Rate Control (In Idle mode/ 8-colors) 
	spi_master_write_data_byte(handle, 0x01);
	spi_master_write_data_byte(handle, 0x2C);
	spi_master_write_data_byte(handle, 0x2D);

	spi_master_write_command(handle, 0xB3);	//Frame Rate Control (In Partial mode/ full colors) 
	spi_master_write_data_byte(handle, 0x01);
	spi_master_write_data_byte(handle, 0x2C);
	spi_master_write_data_byte(handle, 0x2D);
	spi_master_write_data_byte(handle, 0x01);
	spi_master_write_data_byte(handle, 0x2C);
	spi_master_write_data_byte(handle, 0x2D);

	spi_master_write_command(handle, 0xB4);	//Display Inversion Control
	spi_master_write_data_byte(handle, 0x07);

	spi_master_write_command(handle, 0xC0);	//Power Control 1
	spi_master_write_data_byte(handle, 0xA2);
	spi_master_write_data_byte(handle, 0x02);
	spi_master_write_data_byte(handle, 0x84);

	spi_master_write_command(handle, 0xC1);	//Power Control 2
	spi_master_write_data_byte(handle, 0xC5);

	spi_master_write_command(handle, 0xC2);	//Power Control 3 (in Normal mode/ Full colors) 
	spi_master_write_data_byte(handle, 0x0A);
	spi_master_write_data_byte(handle, 0x00);

	spi_master_write_command(handle, 0xC3);	//Power Control 4 (in Idle mode/ 8-colors)
	spi_master_write_data_byte(handle, 0x8A);
	spi_master_write_data_byte(handle, 0x2A);

	spi_master_write_command(handle, 0xC4);	//Power Control 5 (in Partial mode/ full-colors)
	spi_master_write_data_byte(handle, 0x8A);
	spi_master_write_data_byte(handle, 0xEE);

	spi_master_write_command(handle, 0xC5);	//VCOM Control 1
	spi_master_write_data_byte(handle, 0x0E);

	spi_master_write_command(handle, 0x20);	//Display Inversion Off 

	spi_master_write_command(handle, 0x36);	//Memory Data Access Control 
	spi_master_write_data_byte(handle, 0xC8);	//BGR color filter panel
	//spi_master_write_data_byte(handle, 0xC0);	//RGB color filter panel

	spi_master_write_command(handle, 0x3A);	//Interface Pixel Format
	spi_master_write_data_byte(handle, 0x05);	//16-bit/pixel 65K-Colors(RGB 5-6-5-bit Input)

	spi_master_write_command(handle, 0x2A);	//Column Address Set
	spi_master_write_data_byte(handle, 0x00);
	spi_master_write_data_byte(handle, 0x02);
	spi_master_write_data_byte(handle, 0x00);
	spi_master_write_data_byte(handle, 0x81);

	spi_master_write_command(handle, 0x2B);	//Row Address Set
	spi_master_write_data_byte(handle, 0x00);
	spi_master_write_data_byte(handle, 0x01);
	spi_master_write_data_byte(handle, 0x00);
	spi_master_write_data_byte(handle, 0xA0);

	spi_master_write_command(handle, 0x21);	//Display Inversion On

	spi_master_write_command(handle, 0xE0);	//Gamma (‘+’polarity) Correction Characteristics Setting
	spi_master_write_data_byte(handle, 0x02);
	spi_master_write_data_byte(handle, 0x1C);
	spi_master_write_data_byte(handle, 0x07);
	spi_master_write_data_byte(handle, 0x12);
	spi_master_write_data_byte(handle, 0x37);
	spi_master_write_data_byte(handle, 0x32);
	spi_master_write_data_byte(handle, 0x29);
	spi_master_write_data_byte(handle, 0x2D);
	spi_master_write_data_byte(handle, 0x29);
	spi_master_write_data_byte(handle, 0x25);
	spi_master_write_data_byte(handle, 0x2B);
	spi_master_write_data_byte(handle, 0x39);
	spi_master_write_data_byte(handle, 0x00);
	spi_master_write_data_byte(handle, 0x01);
	spi_master_write_data_byte(handle, 0x03);
	spi_master_write_data_byte(handle, 0x10);

	spi_master_write_command(handle, 0xE1);	//Gamma ‘-’polarity Correction Characteristics Setting
	spi_master_write_data_byte(handle, 0x03);
	spi_master_write_data_byte(handle, 0x1D);
	spi_master_write_data_byte(handle, 0x07);
	spi_master_write_data_byte(handle, 0x06);
	spi_master_write_data_byte(handle, 0x2E);
	spi_master_write_data_byte(handle, 0x2C);
	spi_master_write_data_byte(handle, 0x29);
	spi_master_write_data_byte(handle, 0x2D);
	spi_master_write_data_byte(handle, 0x2E);
	spi_master_write_data_byte(handle, 0x2E);
	spi_master_write_data_byte(handle, 0x37);
	spi_master_write_data_byte(handle, 0x3F);
	spi_master_write_data_byte(handle, 0x00);
	spi_master_write_data_byte(handle, 0x00);
	spi_master_write_data_byte(handle, 0x02);
	spi_master_write_data_byte(handle, 0x10);

	spi_master_write_command(handle, 0x13);	//Normal Display Mode On
	vTaskDelay(pdMS_TO_TICKS(10));

	spi_master_write_command(handle, 0x29);	//Display On
	vTaskDelay(pdMS_TO_TICKS(100));

	//gpio_set_level(DISPLAY_LED, 1);
}


void Displays::init_spi(){
	esp_err_t ret;

	auto reset_pin = [](gpio_num_t p){
		gpio_reset_pin(p);
		gpio_set_direction(p, GPIO_MODE_OUTPUT);
		gpio_set_level(p, 1);
	};
	//gpio_reset_pin(DISPLAY_CS);
	//gpio_set_direction(DISPLAY_CS, GPIO_MODE_OUTPUT);
	//gpio_set_level(DISPLAY_CS, 0);
	reset_pin(DISPLAY_CS0);
	reset_pin(DISPLAY_CS1);
	reset_pin(DISPLAY_CS2);
	reset_pin(DISPLAY_CS3);
	
	reset_pin(DISPLAY_BUS_DC);

	reset_pin(DISPLAY_RESET);
	delayMS(100);
	gpio_set_level(DISPLAY_RESET, 1);

	spi_bus_config_t buscfg;
	memset(&buscfg, 0, sizeof(buscfg));
	buscfg.sclk_io_num = DISPLAY_BUS_SCLK;
	buscfg.mosi_io_num = DISPLAY_BUS_MOSI;
	buscfg.miso_io_num = -1;
	buscfg.quadwp_io_num = -1;
	buscfg.quadhd_io_num = -1;
	ret = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
	assert(ret == ESP_OK);

	spi_device_interface_config_t devcfg;
	memset(&devcfg, 0, sizeof(devcfg));
	devcfg.clock_speed_hz = SPI_MASTER_FREQ_8M;
	devcfg.flags = SPI_DEVICE_NO_DUMMY;
	devcfg.queue_size = 7;

	#define hihi(n) \
	devcfg.spics_io_num = DISPLAY_CS##n; \
	ret = spi_bus_add_device(HSPI_HOST, &devcfg, &m_handles[n]); \
	assert(ret == ESP_OK); 

	//hihi(0)
	//hihi(1)
	//hihi(2)
	//hihi(3)
	
	#undef hihi
	
	//devcfg.spics_io_num = DISPLAY_CS;
	ret = spi_bus_add_device(HSPI_HOST, &devcfg, &m_handle);
	assert(ret == ESP_OK); 

	printf("init spi_bus \n");
}

void Displays::init_st7789(){
	auto init = [](spi_device_handle_t &handle){
		spi_master_write_command(handle, 0x01);	//Software Reset
		delayMS(150);

		spi_master_write_command(handle, 0x11);	//Sleep Out
		delayMS(255);
		
		spi_master_write_command(handle, 0x3A);	//Interface Pixel Format
		spi_master_write_data_byte(handle, 0x55);
		delayMS(10);
		
		spi_master_write_command(handle, 0x36);	//Memory Data Access Control
		spi_master_write_data_byte(handle, 0x00);

		spi_master_write_command(handle, 0x2A);	//Column Address Set
		spi_master_write_data_byte(handle, 0x00);
		spi_master_write_data_byte(handle, 0x00);
		spi_master_write_data_byte(handle, 0x00);
		spi_master_write_data_byte(handle, 0xF0);

		spi_master_write_command(handle, 0x2B);	//Row Address Set
		spi_master_write_data_byte(handle, 0x00);
		spi_master_write_data_byte(handle, 0x00);
		spi_master_write_data_byte(handle, 0x00);
		spi_master_write_data_byte(handle, 0xF0);

		spi_master_write_command(handle, 0x21);	//Display Inversion On
		delayMS(10);

		spi_master_write_command(handle, 0x13);	//Normal Display Mode On
		delayMS(10);

		spi_master_write_command(handle, 0x29);	//Display ON
		delayMS(255);
		printf("init handle %p\n", handle);
	};
	for(auto &cs : m_cs){
		gpio_set_level(cs, 1);
		gpio_set_level(cs, 0);
		printf("init cs at %d", int(cs));
		init(m_handle);
		gpio_set_level(cs, 1);
	}
	//init(m_handles[0]);
	//init(m_handles[1]);
	//init(m_handles[2]);
	//init(m_handles[3]);
}

Displays::Displays(){
	m_cs[0] = DISPLAY_CS0;
	m_cs[1] = DISPLAY_CS1;
	m_cs[2] = DISPLAY_CS2;
	m_cs[3] = DISPLAY_CS3;
	init_spi();
	//for(auto cs : m_cs)
	//	gpio_set_level(cs, 0);
	init_st7789();
	//st7735_init(m_handle);
	for(auto cs : m_cs)
		gpio_set_level(cs, 1);
}
void Displays::drawFullImage(size_t ndisplay, const uint16_t *data){
	printf("drawFullImage\n");
	for(auto &cs : m_cs)
		gpio_set_level(cs, 1);
	gpio_set_level(m_cs[ndisplay % 4], 0);
	printf("drawing at cs %d", int(m_cs[ndisplay % 4]));
	spi_device_handle_t handle = m_handle;
	//for(size_t y = 0; y < DISPLAY_HEIGHT; ++y)
	//	for(size_t x = 0; x < DISPLAY_WIDTH; ++x)
	//		draw_pixel(m_handle, x, y, 0xFFFF);
	//return;
	//for(auto &cs : m_cs)
	//	gpio_set_level(cs, 1);
	//gpio_set_level(gpio_num_t(ndisplay % 4), 0);
	//gpio_set_level(gpio_num_t(ndisplay % 4), 1);
	spi_master_write_command(handle, 0x2A);	// set column(x) address
	spi_master_write_addr(handle, 0, DISPLAY_WIDTH - 1);
	spi_master_write_command(handle, 0x2B);	// set Page(y) address
	spi_master_write_addr(handle, 0, DISPLAY_HEIGHT - 1);
	spi_master_write_command(handle, 0x2C);	//	Memory Write
	size_t left = DISPLAY_WIDTH * DISPLAY_HEIGHT;
	size_t packet_size_max = 1024 / 2;
	const uint16_t *ptr = data;//reinterpret_cast<const uint16_t *>(pixnum(seconds));
	while(left){
		size_t packet_size = std::min(left, packet_size_max);
		spi_master_write_colors(handle, ptr, packet_size);
		left -= packet_size;
		ptr += packet_size;
	}
	for(auto &cs : m_cs)
		gpio_set_level(cs, 1);
}
