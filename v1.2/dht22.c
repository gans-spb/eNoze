//DHT22 AM2302 simple lib
//Igor Bocharov

#include "gd32f10x_gpio.h"
#include "timer.h"

#define	DHT22_ERR_PRES	0xffffffff
#define	DHT22_ERR_TIME	0xfffffffe
#define	DHT22_ERR_CRC		0xfffffffd

uint32_t DHT22_Presence (uint32_t ds_port, uint32_t ds_pin){
	
	uint8_t  r;
	uint16_t d;
	
	//TX reset >18ms low, 20-40us hi
	gpio_init(ds_port, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, ds_pin);
	GPIO_BC(ds_port) = ds_pin;
	GD32_DelayTim2(1200);
	GPIO_BOP(ds_port) = ds_pin;
	GD32_DelayTim2(30);
	
	//RX presence wait 80uS low + 80us hi + wait low as data stream
	gpio_init(ds_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, ds_pin);
	GD32_DelayTim2(40);
	r = gpio_input_bit_get(ds_port, ds_pin);
	if (r) return DHT22_ERR_PRES;
	
	GD32_DelayTim2(80);
	r = gpio_input_bit_get(ds_port, ds_pin);
	if (!r) return DHT22_ERR_PRES;
	
	d=0xffff;
	while (gpio_input_bit_get(ds_port, ds_pin)){
		if (!--d) return DHT22_ERR_TIME;
	}
	r = gpio_input_bit_get(ds_port, ds_pin);
	
	return r;
}

uint32_t DHT22_Read_Byte (uint32_t ds_port, uint32_t ds_pin){
	uint8_t v;
	uint8_t j;
	uint16_t d;
	
	for (j=0;j<8;j++)
	{
		d=0xffff;
		while (!gpio_input_bit_get(ds_port, ds_pin)){ //wait hi
			if (!--d) return DHT22_ERR_TIME;
			}
		GD32_DelayTim2 (40); // middle of 0/1 pwm

		if (!gpio_input_bit_get(ds_port, ds_pin))
				 v &= ~(1<<(7-j)); //0 = 50us low + 26-28us hi
		else v |=  (1<<(7-j)); //1 = 50us low + 70us hi
		
		d=0xffff;
		while (gpio_input_bit_get(ds_port, ds_pin)){ //wait lo
			if (!--d) return DHT22_ERR_TIME;
		}
	}
	return v;
}
	
uint32_t DHT22_Read (uint32_t ds_port, uint32_t ds_pin){
	uint32_t hh,hl,th,tl,cr,su;
	
	hh = DHT22_Read_Byte (ds_port, ds_pin);
	if (hh>0xff) return DHT22_ERR_TIME;
	hl = DHT22_Read_Byte (ds_port, ds_pin);
	if (hh>0xff) return DHT22_ERR_TIME;
	th = DHT22_Read_Byte (ds_port, ds_pin);
	if (hh>0xff) return DHT22_ERR_TIME;
	tl = DHT22_Read_Byte (ds_port, ds_pin);
	if (hh>0xff) return DHT22_ERR_TIME;
	cr = DHT22_Read_Byte (ds_port, ds_pin);
	if (hh>0xff) return DHT22_ERR_TIME;
	
	su = (uint8_t)(hh+hl+th+tl);
	if (su!=cr) 
		return DHT22_ERR_CRC;
	else	
		return (hh<<24) + (hl<<16) + (th<<8) + (tl);
}
	
uint32_t DHT22_Got(uint32_t ds_port, uint32_t ds_pin){
	uint32_t ret;
	
	ret = DHT22_Presence (ds_port, ds_pin);
	if (ret) return ret;
	
	ret = DHT22_Read (ds_port, ds_pin);
	
	GD32_DelayTim2(1000);
	
	return ret;
}
