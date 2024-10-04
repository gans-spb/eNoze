//DS18B20 simple lib
//Igor Bocharov

#include "gd32f10x_gpio.h"
#include "timer.h"

uint8_t DS18B20_Presence (uint32_t ds_port, uint32_t ds_pin)
{
	uint8_t r;
	//uint8_t ret = 0;
	
	//TX reset 480uS
	gpio_init(ds_port, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, ds_pin);
	GPIO_BC(ds_port) = ds_pin;
	GD32_DelayTim2(480);
	
	//RX presence wait 15-60uS 
	gpio_init(ds_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, ds_pin);
	GD32_DelayTim2(80);
	r = gpio_input_bit_get(ds_port, ds_pin);
	
	//RX presence body total 480us
	GD32_DelayTim2 (400);

	return r;
}

void DS18B20_Write (uint32_t ds_port, uint32_t ds_pin, uint8_t data)
{
	int i;
	gpio_init(ds_port, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, ds_pin);

	for (i=0; i<8; i++)
	{

		if ((data & (1<<i))!=0)  // if the bit is high
		{
			// write 1
			gpio_init(ds_port, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, ds_pin);
			GPIO_BC(ds_port) = ds_pin;
			GD32_DelayTim2 (1);

			gpio_init(ds_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, ds_pin);
			GD32_DelayTim2 (50);  // wait for 60 us
		}
		else  // if the bit is low
		{
			// write 0

			gpio_init(ds_port, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, ds_pin);
			GPIO_BC(ds_port) = ds_pin;
			GD32_DelayTim2 (50);

			gpio_init(ds_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, ds_pin);
		}
	}
}

uint8_t DS18B20_Read (uint32_t ds_port, uint32_t ds_pin)
{
	uint8_t value=0;
	uint8_t r;
	int i;
	gpio_init(ds_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, ds_pin);

	for (i=0;i<8;i++)
	{
		gpio_init(ds_port, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, ds_pin);

		GPIO_BC(ds_port) = ds_pin;
		GD32_DelayTim2 (2);

		gpio_init(ds_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, ds_pin);
		
		r = gpio_input_bit_get(ds_port, ds_pin);
		
		if (r){
			value |= 1<<i;
		}
		GD32_DelayTim2 (60);  // wait for 60 us
	}
	return value;
}

uint16_t DS18B20_Got(uint32_t ds_port, uint32_t ds_pin){
	uint16_t ret;
	uint8_t  b1, b2;
	
	ret = DS18B20_Presence (ds_port, ds_pin);
	if (ret) return 0xffff;
	
	GD32_DelayTim2 (1);
	DS18B20_Write (ds_port, ds_pin, 0xCC);  // skip ROM
	DS18B20_Write (ds_port, ds_pin, 0x44);  // convert t

	//12bit 750uS conversion time
	GD32_DelayTim2 (800);

	ret = DS18B20_Presence (ds_port, ds_pin);
	if (ret) return 0xffff;
	
	GD32_DelayTim2 (1);
	DS18B20_Write (ds_port, ds_pin, 0xCC);  // skip ROM
	DS18B20_Write (ds_port, ds_pin, 0xBE);  // Read Scratch-pad

	b1 = DS18B20_Read(ds_port, ds_pin);
	b2 = DS18B20_Read(ds_port, ds_pin);
	//2DO CRC
	
	GD32_DelayTim2 (100);
	
	ret = (b1 + (b2<<8)) * 0.625f;
	
	return ret;
}

