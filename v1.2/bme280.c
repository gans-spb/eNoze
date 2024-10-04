//BME280 small lib
//Igor Bocharov

#include "gd32f10x_gpio.h"
#include "gd32f10x_i2c.h"
#include "config.h"

#define wto 5000	//~5ms

#define I2C_NUM					I2C1
#define I2C_MASTER_ADDR 0x72
#define I2C_SLAVE_ADDR1 (0x76<<1)
#define I2C_SLAVE_ADDR2 (0x77<<1)

//ready to multichannel, 2 for one i2c lane
#define BME280_NUM	2
uint8_t ch2slave(uint8_t ch){
	switch (ch){
		case 0:  return I2C_SLAVE_ADDR1;
		case 1:  return I2C_SLAVE_ADDR2;
		default: return 0xFF;
	}
}

#define BME280_REG_Id					0xD0
#define BME280_REG_Reset			0xE0
#define BME280_REG_Ctrl_Hum		0XF2
#define BME280_REG_Status			0XF3
#define BME280_REG_Ctrl_Meas	0xF4
#define BME280_REG_Cfg				0xF5
#define BME280_REG_Weather		0xF7

#define BME280_REG_Calib1			0x88
#define BME280_REG_Calib2			0xE1

#define BME280_REG_Id_Val			0x60
#define BME280_REG_Res_Val		0xB6

#define  TO_STOPPER if (--e==0) {i2c_stop_on_bus(I2C_NUM); return TRUE;}

//-----------------------------------------------------------------------------
//start i2c and send register number
bool BME_280_send_reg(uint8_t slave, uint8_t reg){

		uint16_t e;
	
		//wait idle, start, wait start-send-ok
		e = wto;
		while(i2c_flag_get(I2C_NUM, I2C_FLAG_I2CBSY))  if (--e==0) return TRUE;
		i2c_start_on_bus(I2C_NUM);
		e = wto;
		while(!i2c_flag_get(I2C_NUM, I2C_FLAG_SBSEND)) TO_STOPPER
		
		// send slave address, wait addr-send-ok, clear addr-send-ok, wait send-buf-empty
    i2c_master_addressing(I2C_NUM, slave, I2C_TRANSMITTER);
		e = wto;
		while(!i2c_flag_get(I2C_NUM, I2C_FLAG_ADDSEND)) TO_STOPPER
		i2c_flag_clear(I2C_NUM, I2C_FLAG_ADDSEND);
		e = wto;
		while(!i2c_flag_get(I2C_NUM, I2C_FLAG_TBE)) TO_STOPPER
	 
	  //send data = reg num, wait send-buf-empty
		i2c_data_transmit(I2C_NUM,reg);
		e = wto;
		while(!i2c_flag_get(I2C_NUM, I2C_FLAG_TBE)) TO_STOPPER
		
		return FALSE;
}

//continue i2c and send register value to set
bool BME_280_send_val(uint8_t val){

		uint16_t e;

		//send data = reg val, wait send-buf-empty
		i2c_data_transmit(I2C_NUM,val);
		e = wto;
		while(!i2c_flag_get(I2C_NUM, I2C_FLAG_TBE)) TO_STOPPER
		
		e = wto;
		while(!i2c_flag_get(I2C_NUM, I2C_FLAG_BTC)) TO_STOPPER
		i2c_ack_config(I2C_NUM, I2C_ACK_DISABLE);
	
				
		i2c_stop_on_bus(I2C_NUM);
		e = wto;
		while(I2C_CTL0(I2C_NUM)&0x0200) if (--e==0) return TRUE;

		return FALSE;
}

//continue i2c and read only one register value
bool BME_280_read_val(uint8_t slave, uint16_t* data){
		uint16_t e;
		
		i2c_stop_on_bus(I2C_NUM);
		e = wto;
		while(I2C_CTL0(I2C_NUM)&0x0200) if (--e==0) return TRUE;

		//RCV
		//start bus
		e = wto;
		while(i2c_flag_get(I2C_NUM, I2C_FLAG_I2CBSY)) if (--e==0) return TRUE;
		i2c_start_on_bus(I2C_NUM);
		e = wto;
		while(!i2c_flag_get(I2C_NUM, I2C_FLAG_SBSEND)) TO_STOPPER

    //i2c_rcvr
		i2c_master_addressing(I2C_NUM, slave, I2C_RECEIVER);
		e = wto;
		while(!i2c_flag_get(I2C_NUM, I2C_FLAG_ADDSEND)) TO_STOPPER
		
		//! GD32 specifique N=1 mode
		i2c_ack_config(I2C_NUM, I2C_ACK_DISABLE);
		
		i2c_flag_clear(I2C_NUM, I2C_FLAG_ADDSEND);
		
		i2c_stop_on_bus(I2C_NUM);
		
		//rcv data
		e = wto;
		while(!i2c_flag_get(I2C_NUM, I2C_FLAG_RBNE)) TO_STOPPER
    *data = i2c_data_receive(I2C_NUM); 
			
		e = wto;
		while(I2C_CTL0(I2C_NUM)&0x0200) if (--e==0) return TRUE;
		i2c_ack_config(I2C_NUM, I2C_ACK_ENABLE);

		return FALSE;
}

//continue i2c and read array register values
bool BME_280_read_val_array(uint8_t slave, uint8_t* data, uint8_t size){
		uint16_t e,i;
		if (size<3) return TRUE;
		
		i2c_stop_on_bus(I2C_NUM);
		e = wto;
		while(I2C_CTL0(I2C_NUM)&0x0200) if (--e==0) return TRUE;

		//RCV
		//start bus
		e = wto;
		while(i2c_flag_get(I2C_NUM, I2C_FLAG_I2CBSY)) if (--e==0) return TRUE;
		i2c_start_on_bus(I2C_NUM);
		e = wto;
		while(!i2c_flag_get(I2C_NUM, I2C_FLAG_SBSEND)) TO_STOPPER

    //i2c_rcvr
		i2c_master_addressing(I2C_NUM, slave, I2C_RECEIVER);
		e = wto;
		while(!i2c_flag_get(I2C_NUM, I2C_FLAG_ADDSEND)) TO_STOPPER
		i2c_flag_clear(I2C_NUM, I2C_FLAG_ADDSEND);
		
		//GD32 array normal mode
		for(i=0;i<size;i++){
			if(i==size-3){
				e = wto;
				while(!i2c_flag_get(I2C_NUM, I2C_FLAG_BTC)) TO_STOPPER;
				i2c_ack_config(I2C_NUM, I2C_ACK_DISABLE);
			}
			
			e = wto;
			while(!i2c_flag_get(I2C_NUM, I2C_FLAG_RBNE)) TO_STOPPER;
			data[i] = i2c_data_receive(I2C_NUM); 
		}
		
		//stop
		i2c_stop_on_bus(I2C_NUM);
		e = wto;
		while(I2C_CTL0(I2C_NUM)&0x0200) if (--e==0) return TRUE;
		i2c_ack_config(I2C_NUM, I2C_ACK_ENABLE);

		return FALSE;
}

//Set an register with value
bool BME_280_set_reg(uint8_t slave, uint8_t reg, uint8_t val){
	uint16_t e;
	e = BME_280_send_reg(slave,reg);
		if (e) return TRUE;
	e = BME_280_send_val(val);
		if (e) return TRUE;
	return FALSE;
}

//Get an register with value
uint16_t BME_280_get_reg(uint8_t slave, uint8_t reg){
	uint16_t e,v;	
	e = BME_280_send_reg(slave,reg);
		if (e) return TRUE;
	e = BME_280_read_val(slave,&v);
		if (e) return TRUE;
	return v;
}

//-----------------------------------------------------------------------------
//calibration deals
struct bme280_calib_data{
    uint16_t t1;
    int16_t t2;
    int16_t t3;
    uint16_t p1;
    int16_t p2;
    int16_t p3;
    int16_t p4;
    int16_t p5;
    int16_t p6;
    int16_t p7;
    int16_t p8;
    int16_t p9;
    uint8_t h1;
    int16_t h2;
    uint8_t h3;
    int16_t h4;
    int16_t h5;
    int8_t h6;
    int32_t t_fine;
		bool en;
} calib_data[BME280_NUM];

#define CONCATU(msb, lsb)	(int16_t)(((uint16_t)msb << 8) | (uint16_t)lsb)
#define CONCATS(msb, lsb)					 (((uint16_t)msb << 8) | (uint16_t)lsb)

//get calibration data 88-A1, E1-F0
bool BME_280_read_calib(uint8_t ch){
	uint16_t e;
	uint8_t data[26];
	  
	int16_t h4_lsb;
  int16_t h4_msb;
  int16_t h5_lsb;
  int16_t h5_msb;
	
	uint8_t slave;
	
	slave = ch2slave(ch);

		e = BME_280_send_reg(slave,BME280_REG_Calib1);
			if (e) return FALSE;
		e = BME_280_read_val_array(slave,data,25);
			if (e) return FALSE;
		
		calib_data[ch].t1 = CONCATS(data[1],data[0]);
		calib_data[ch].t2 = CONCATU(data[3],data[2]);
		calib_data[ch].t3 = CONCATU(data[5],data[4]);
		
		calib_data[ch].p1 = CONCATS(data[7],data[6]);
		calib_data[ch].p2 = CONCATU(data[9],data[8]);
		calib_data[ch].p3 = CONCATU(data[11],data[10]);
		calib_data[ch].p4 = CONCATU(data[13],data[12]);
		calib_data[ch].p5 = CONCATU(data[15],data[14]);
		calib_data[ch].p6 = CONCATU(data[17],data[16]);
		calib_data[ch].p7 = CONCATU(data[19],data[18]);
		calib_data[ch].p8 = CONCATU(data[21],data[20]);
		calib_data[ch].p9 = CONCATU(data[23],data[22]);
		
		calib_data[ch].h1 = data[25];
			
		e = BME_280_send_reg(slave,BME280_REG_Calib2);
			if (e) return FALSE;
		e = BME_280_read_val_array(slave,data,7);
			if (e) return FALSE;
			
		calib_data[ch].h2 = CONCATS(data[1],data[0]);
		calib_data[ch].h3 = data[2];
		h4_msb = (int16_t)(int8_t)data[3] * 16;
		h4_lsb = (int16_t)(data[4] & 0x0F);
		calib_data[ch].h4 = h4_msb | h4_lsb;
		h5_msb = (int16_t)(int8_t)data[5] * 16;
		h5_lsb = (int16_t)(data[4] >> 4);
		calib_data[ch].h5 = h5_msb | h5_lsb;
		calib_data[ch].h6 = (int8_t)data[6];
		
		calib_data[ch].en = TRUE;

	return TRUE;
}

//init sensor
bool BME_280_init(uint8_t ch){
	uint16_t e;
	uint8_t slave;
	slave = ch2slave(ch);
		
	//check BME280 id = 0x60
	e = BME_280_get_reg(slave,BME280_REG_Id);
	if (e!=BME280_REG_Id_Val) return FALSE;
	
	//check BME280 status = 0x00
	e = BME_280_get_reg(slave,BME280_REG_Status);
	if (e!=0) return FALSE;
	
	//set config
	//F2, Humidity oversampling = x16 = 101 = 0x5
	e = BME_280_set_reg(slave,BME280_REG_Ctrl_Hum,0x5);
	e = BME_280_get_reg(slave,BME280_REG_Ctrl_Hum);
	if (e!=0x5) return FALSE;

	//F4, Pos/Tos=x16=101=0x5, Mode=Norm=11
	// 101 101 11 = 0xB7
	e = BME_280_set_reg(slave,BME280_REG_Ctrl_Meas,0xB7);
	e = BME_280_get_reg(slave,BME280_REG_Ctrl_Meas);
	if (e!=0xB7) return FALSE;
	
	
	//F5, Tstdb=1s=101, filter=off=000, 0, spi=off=0
	// 101 000 0 0 = 0xA0
	e = BME_280_set_reg(slave,BME280_REG_Cfg,0xA0);
	e = BME_280_get_reg(slave,BME280_REG_Cfg);
	if (e!=0xA0) return FALSE;
	
	return TRUE;
}


void BME_280_initAll(void){
		bool ret;
	
		ret = BME_280_init(0);
			if (ret) BME_280_read_calib(0); 
			else  calib_data[0].en = FALSE;
		ret = BME_280_init(1);
			if (ret) BME_280_read_calib(1);
			else  calib_data[1].en = FALSE;	
}

//PB10(69) PB11(70) AltMode USART2_TX/RX -> I2C1_SCL/SDA, 10KHz
void I2C_Conf(){
		uint16_t d=0xFFFF;
		//rcu conf
		//rcu_periph_clock_enable(RCU_GPIOB);
		rcu_periph_clock_enable(RCU_I2C1);
		//gpio conf
		gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_10MHZ, GPIO_PIN_10);       // i2c scl	
		gpio_init(GPIOB, GPIO_MODE_AF_OD,	GPIO_OSPEED_10MHZ, GPIO_PIN_11);       // i2c sda
		    
    //i2c config
		i2c_clock_config(I2C_NUM, 10000, I2C_DTCY_2);
    i2c_mode_addr_config(I2C_NUM, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C_MASTER_ADDR);
    i2c_enable(I2C_NUM);
    i2c_ack_config(I2C_NUM, I2C_ACK_ENABLE);
	
		Screen_Pwr_On();
		while (d--);
	
		BME_280_initAll();
}

//-----------------------------------------------------------------------------
//compensators raw->real
//based on Bosch Sensortec GmbH BME280_SensorAPI 2020-12-17 v3.5.1

//110000/100=1100.00kPa
uint32_t compens_press(uint8_t ch,uint32_t raw_press)
{
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    uint32_t var5;
    uint32_t pressure;
    uint32_t pressure_min = 30000;
    uint32_t pressure_max = 110000;

    var1 = (((int32_t)calib_data[ch].t_fine) / 2) - (int32_t)64000;
    var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((int32_t)calib_data[ch].p6);
    var2 = var2 + ((var1 * ((int32_t)calib_data[ch].p5)) * 2);
    var2 = (var2 / 4) + (((int32_t)calib_data[ch].p4) * 65536);
    var3 = (calib_data[ch].p3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8;
    var4 = (((int32_t)calib_data[ch].p2) * var1) / 2;
    var1 = (var3 + var4) / 262144;
    var1 = (((32768 + var1)) * ((int32_t)calib_data[ch].p1)) / 32768;

    // Avoid exception caused by division by zero
    if (var1){
        var5 = (uint32_t)((uint32_t)1048576) - raw_press;
        pressure = ((uint32_t)(var5 - (uint32_t)(var2 / 4096))) * 3125;

        if (pressure < 0x80000000) pressure = (pressure << 1) / ((uint32_t)var1);
															else pressure = (pressure / (uint32_t)var1) * 2;

        var1 = (((int32_t)calib_data[ch].p9) * ((int32_t)(((pressure / 8) * (pressure / 8)) / 8192))) / 4096;
        var2 = (((int32_t)(pressure / 4)) * ((int32_t)calib_data[ch].p8)) / 8192;
        pressure = (uint32_t)((int32_t)pressure + ((var1 + var2 + calib_data[ch].p7) / 16));

						 if (pressure < pressure_min) pressure = pressure_min;
        else if (pressure > pressure_max) pressure = pressure_max;
    }
    else pressure = pressure_min;

    return pressure;
}

//8500/100=85.00C
int32_t compens_temp(uint8_t ch,uint32_t raw_temp){
		int32_t var1;
    int32_t var2;
    int32_t temperature;
    int32_t temperature_min = -4000;
    int32_t temperature_max = 8500;

    var1 = (int32_t)((raw_temp / 8) - ((int32_t)calib_data[ch].t1 * 2));
    var1 = (var1 * ((int32_t)calib_data[ch].t2)) / 2048;
    var2 = (int32_t)((raw_temp / 16) - ((int32_t)calib_data[ch].t1));
    var2 = (((var2 * var2) / 4096) * ((int32_t)calib_data[ch].t3)) / 16384;
    calib_data[ch].t_fine = var1 + var2;
    temperature = (calib_data[ch].t_fine * 5 + 128) / 256;

    if 			(temperature < temperature_min)	temperature = temperature_min;
    else if (temperature > temperature_max) temperature = temperature_max;

    return temperature;
}

//102400/1000=102.400%
uint32_t compens_humi(uint8_t ch,uint32_t raw_humi)
{
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    uint32_t humidity;
    uint32_t humidity_max = 102400;

    var1 = calib_data[ch].t_fine - ((int32_t)76800);
    var2 = (int32_t)(raw_humi * 16384);
    var3 = (int32_t)(((int32_t)calib_data[ch].h4) * 1048576);
    var4 = ((int32_t)calib_data[ch].h5) * var1;
    var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
    var2 = (var1 * ((int32_t)calib_data[ch].h6)) / 1024;
    var3 = (var1 * ((int32_t)calib_data[ch].h3)) / 2048;
    var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
    var2 = ((var4 * ((int32_t)calib_data[ch].h2)) + 8192) / 16384;
    var3 = var5 * var2;
    var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
    var5 = var3 - ((var4 * ((int32_t)calib_data[ch].h1)) / 16);
    var5 = (var5 < 0 ? 0 : var5);
    var5 = (var5 > 419430400 ? 419430400 : var5);
    humidity = (uint32_t)(var5 / 4096);

    if (humidity > humidity_max)humidity = humidity_max;

    return humidity;
}

//-----------------------------------------------------------------------------
//get weather data
bool BME_280_read_weather(uint8_t ch, uint16_t* t, uint16_t* h, uint16_t* p){
	uint16_t e;
	uint8_t data[8];
	
		uint32_t data_xlsb;
    uint32_t data_lsb;
    uint32_t data_msb;
		uint32_t val;
	
	uint8_t slave;
	slave = ch2slave(ch);
	*t = 0xFFFF; *h=0; *p=0;
	if (calib_data[ch].en == FALSE) return FALSE;
			
	e = BME_280_send_reg(slave,BME280_REG_Weather);
		if (e) return FALSE;
	e = BME_280_read_val_array(slave,data,8);
		if (e) return FALSE;
	  
    //pressure
		data_msb = (uint32_t) data[0] << 12;
    data_lsb = (uint32_t) data[1] << 4;
    data_xlsb = (uint32_t)data[2] >> 4;
    val = data_msb | data_lsb | data_xlsb;
		val = compens_press(ch,val);
		*p = val-50000;
			
    //temperature
		data_msb = (uint32_t) data[3] << 12;
    data_lsb = (uint32_t) data[4] << 4;
    data_xlsb = (uint32_t)data[5] >> 4;
    val = data_msb | data_lsb | data_xlsb;
		val = compens_temp(ch,val);
		*t = val;
	
    //humidity
		data_msb = (uint32_t)data[6] << 8;
    data_lsb = (uint32_t)data[7];
    val = data_msb | data_lsb;
		val = compens_humi(ch,val)/10;
		*h = val;
				
	return TRUE;
}

bool BME280_Got(uint8_t ch, uint16_t* t, uint16_t* h, uint16_t* p){
	return BME_280_read_weather(ch,t,h,p);
}

//-----------------------------------------------------------------------------
