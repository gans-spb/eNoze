
#include "gd32f10x_gpio.h"
#include "data.h"
#include "config.h"
#include "flash.h"
#include "float.h"

extern void handler_memory(void);

static unsigned int registers[SIZE_REG];    // регистры устройства    // 


//////////////////// кладем полученное в массив registers[] ////////////////////
extern void registers_set(unsigned int value, unsigned int num)
{
  if(num<SIZE_REG) {registers[num]=value;}
}
//////////////////// забираем параметры из массива registers[] /////////////////
extern unsigned int registers_get(unsigned int num)
{
  if(num<SIZE_REG) {return registers[num];}
  return 0;
}
/////////////////////////////////////////////////////////////////////////////////
extern unsigned int change_bit(unsigned int data,unsigned char bit, unsigned char state)
{
	unsigned int temp;
	
	temp=data;
	if(bit<=15)
	{
		if (state)temp|=(1<<bit);
		else temp&=~(1<<bit);
	}
	
	return temp;
}

////////////////////////////////////////////////////////////////////////////////
extern float Int32_to_Float( unsigned int msb, unsigned int lsb)
{
  float f=0; 
	unsigned long longdata=0;
	 
	longdata=((lsb&0x0000FF00)>>8);
	longdata|=((lsb&0x000000FF)<<8);
  longdata|=((msb&0x0000FF00)<<8);
	longdata|=((msb&0x000000FF)<<24);

  f = *(float*)&longdata;           // Note that precision will be lost

  return f;
}
////////////////////////////////////////////////////////////////////////////////
extern unsigned char* Float_to_Byte(float f)
{
  unsigned long longdata = 0;
  static unsigned char byte[4];

  longdata = *(unsigned long*)&f;           // Note that precision will be lost
  byte[0] = (longdata & 0xFF000000) >> 24;
  byte[1] = (longdata & 0x00FF0000) >> 16;
  byte[2] = (longdata & 0x0000FF00) >> 8;
  byte[3] = (longdata & 0x000000FF);

  return byte;
}


////////////////////////////////////////////////////////////////////////////////
void Keys_Init(void)
{
	// DWN
  KEY_DWN0_OFF();
	KEY_DWN1_OFF();
	KEY_DWN2_OFF();
	KEY_DWN3_OFF();
	KEY_DWN4_OFF();
	KEY_DWN5_OFF();
	// UP
	KEY_UP0_OFF();
	KEY_UP1_OFF();
	KEY_UP2_OFF();
	KEY_UP3_OFF();
	KEY_UP4_OFF();
	KEY_UP5_OFF();
	KEY_UP6_OFF();
	KEY_UP7_OFF();
	KEY_UP8_OFF();
	KEY_UP9_OFF();
	KEY_UP10_OFF();
	KEY_UP11_OFF();
	KEY_UP12_OFF();
	KEY_UP13_OFF();
	KEY_UP14_OFF();
	KEY_UP15_OFF();
	KEY_UP16_OFF();
	KEY_UP17_OFF();
	KEY_UP18_OFF();
	KEY_UP19_OFF();
	// MID
	KEY_MID0_OFF();
	KEY_MID1_OFF();
	KEY_MID2_OFF();
	KEY_MID3_OFF();
	KEY_MID4_OFF();
	KEY_MID5_OFF();
	KEY_MID6_OFF();
	KEY_MID7_OFF();
	KEY_MID8_OFF();
	KEY_MID9_OFF();
	KEY_MID10_OFF();
	KEY_MID11_OFF();
	KEY_MID12_OFF();
	KEY_MID13_OFF();
	KEY_MID14_OFF();
	KEY_MID15_OFF();
	KEY_MID16_OFF();
	KEY_MID17_OFF();
	KEY_MID18_OFF();
	KEY_MID19_OFF();
}

void Pin_Init(void)
{
	LED_SYS_OFF();
	
	//ON all temp sensors but graphene
	HMDT_ON();
	//Screen_Pwr_On() //see bme280
	SENS_T_OFF();
	
	//Off all heater
	HEAT_ALL_OFF();
	HEAT1_OFF();
	HEAT2_OFF();
	Heater_Boost_Off()
	
	//Off all mechanics
	ValvePwr_Off();
	Valve_Off()
	VENT_OFF();
	
	//Init analog frontend switches
	Keys_Init();
}

//-----------------------------------------------------------------------------
void LedBlink(){
	static uint16_t wdc=0;
	static uint8_t  prun=0;
	
	registers_set(++wdc,V2_HeartBeat);
	if (!(++prun%3)) LED_SYS_ON();
}

#define PORT_ROT_CNT	GPIOC
#define PIN_ROT_CNT		GPIO_PIN_15

bool rot_old = FALSE;
bool pin_old = TRUE;
uint16_t rot_c;

void Rotator(){
	uint16_t r = registers_get(V2_Cmd_Motor);
	//stopped
	if (r==0){
		if (rot_old==TRUE) rot_old=FALSE; //manual stop 
	 return;
	}
	//start
	if (!rot_old) {
		rot_old = TRUE; 
		pin_old = TRUE;
		rot_c = registers_get(V2_Rotat);
		gpio_init(PORT_ROT_CNT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, PIN_ROT_CNT);
	}
	//falling edge
	if (!gpio_input_bit_get(PORT_ROT_CNT,PIN_ROT_CNT) && pin_old == TRUE){
		pin_old = FALSE;
		if (--rot_c==0) {//cnt stop!
			registers_set(0, V2_Cmd_Motor);
			rot_old = FALSE;
		}
	}
	//rising edge
	if (gpio_input_bit_get(PORT_ROT_CNT,PIN_ROT_CNT) && pin_old == FALSE) pin_old = TRUE;
}
	
extern void BME_280_initAll(void);
extern void temp_control_v2(void);
	
void V2_Runner(){
	int r,b;
	LED_SYS_OFF();

	//heat on - see temp_control_v1/v2
	/*r = registers_get(V2_Cmd_HEAT);
	b = HEAT_ALL_Bit;
	if(  b&& r) {HEAT_ALL_ON(); HEAT1_ON();  HEAT2_ON();}
	if( !b&&!r) {HEAT_ALL_OFF();HEAT1_OFF(); HEAT2_OFF();}*/
		
	//heat boost
	r = registers_get(V2_Cmd_Heat_Boost);
	b = Heater_Boost_Bit;
	if( !b&& r) Heater_Boost_On();
	if(  b&&!r) Heater_Boost_Off();
	
	//valve power and valve
	r = registers_get(V2_Cmd_Valve);
	b = ValvePwr_Bit;
	if(  b&& r) {ValvePwr_On(); Valve_On(); }
	if( !b&&!r) {ValvePwr_Off();Valve_Off();}	
	
	//motor
	r = registers_get(V2_Cmd_Motor);
	b = VENT_ON_Bit;
	if(  b&& r) VENT_ON();
	if( !b&&!r) VENT_OFF();

	//Rotator mode
	r = registers_get(V2_Rotat);
	if (r>0) Rotator();
	
	//test mode
	r = registers_get(V2_Cmd_Test);
	
	b = HMDT_ON_Bit;
	if( !b&&!r) HMDT_ON();
	if(  b&&r)  HMDT_OFF();
	
	b = Screen_Pwr_Bit;
	if( !b&&!r) {Screen_Pwr_On(); BME_280_initAll();}
	if(  b&&r)  Screen_Pwr_Of();
	
	if(r) registers_set(0, REG_MEAS_PRE);
	if(r) registers_set(0, V2_Cmd_HEAT);
	if(r) registers_set(0, V2_Cmd_Motor);
	if(r) registers_set(0, V2_Cmd_Valve);
	
	if(r) temp_control_v2();
}

////////////////////////////////////////////////////////////////////////////////
//IEEE754 40D5 1EB8, saved as D540 B81E, swapped!
#define DEF_TSET	45.0
#define DEF_R0		6.66
#define DEF_R0V2	66
#define GOLD_A 		0.0031
#define GOLD_B 		0.0

void Data_Init(void)
{
	static float f;
  static unsigned char* ptr;
	unsigned int lsb, msb, value;
	
	#pragma diag_suppress 2548
	//2EN-38-3.3-300R-300MV
  registers_set('Gr',DATA_NAME1);
	registers_set('ap',DATA_NAME2);
	registers_set('he',DATA_NAME3);
	registers_set('ne',DATA_NAME4);
	registers_set(' e',DATA_NAME5);
	registers_set('No',DATA_NAME6);
	registers_set('se',DATA_NAME7);
	registers_set('V2',DATA_NAME8);
	registers_set(' P',DATA_NAME9);
	registers_set('CB',DATA_NAME10);
		
	registers_set('20',DATA_VER1);
	registers_set('05',DATA_VER2);
	registers_set('20',DATA_VER3);
	registers_set('24',DATA_VER4);
	
	registers_set('Rv',DATA_SERIAL1);
	registers_set('2-',DATA_SERIAL2);
	registers_set('1.',DATA_SERIAL3);
	registers_set('24',DATA_SERIAL4);
	
	// t
	value=flash_read_32(ADR_FL_TMP_t,ADR_SITE_BASE);
	if(value!=0xFFFFFFFF) f=Int32_to_Float((value>>16),(value&0xffff));
	else
	{
		f=DEF_TSET;
	  ptr=Float_to_Byte(f);
		msb=ptr[0];		
		msb|=(ptr[1]<<8); 		
		lsb=ptr[2];	
		lsb|=(ptr[3]<<8);
		value=lsb|(msb<<16);
		flash_write_32bit(value,ADR_FL_TMP_t,ADR_SITE_BASE);
	}
	ptr=Float_to_Byte(f);
	msb=ptr[0];		
	msb|=(ptr[1]<<8); 		
	lsb=ptr[2];	
	lsb|=(ptr[3]<<8);
	registers_set(lsb,DATA_TMP_t_LSB);
	registers_set(msb,DATA_TMP_t_MSB);
	
	// R0
	value=flash_read_32(ADR_FL_R0,ADR_SITE_BASE);
	if(value!=0xFFFFFFFF) f=Int32_to_Float((value>>16),(value&0xffff));
	else
	{	
	  f=DEF_R0;
	  ptr=Float_to_Byte(f);
		msb=ptr[0];		
		msb|=(ptr[1]<<8); 		
		lsb=ptr[2];	
		lsb|=(ptr[3]<<8);
		value=lsb|(msb<<16);
		flash_write_32bit(value,ADR_FL_R0,ADR_SITE_BASE);
	}
	ptr=Float_to_Byte(f);
	msb=ptr[0];		
	msb|=(ptr[1]<<8); 		
	lsb=ptr[2];	
	lsb|=(ptr[3]<<8); 
	registers_set(lsb,DATA_TMP_R0_LSB);
	registers_set(msb,DATA_TMP_R0_MSB);
	
	//V2 0R0
	value=flash_read_32(V2_ADR_FL_0R,ADR_SITE_BASE);
	if(value==0xFFFFFFFF){
		value=DEF_R0V2;
		value=(value|(value<<16));
		flash_write_32bit( value, V2_ADR_FL_0R, ADR_SITE_BASE);
	}
	registers_set(value&0xffff, V2_Heat_0R0_OHM);	
	registers_set(value>>16,    V2_Heat_0R1_OHM);		
		
	// A
	value=flash_read_32(ADR_FL_A,ADR_SITE_BASE);
	if(value!=0xFFFFFFFF) f=Int32_to_Float((value>>16),(value&0xffff));
	else
	{	
	  f=GOLD_A; 
	  ptr=Float_to_Byte(f);
		msb=ptr[0];		
		msb|=(ptr[1]<<8); 		
		lsb=ptr[2];	
		lsb|=(ptr[3]<<8);
		value=lsb|(msb<<16);
		flash_write_32bit(value,ADR_FL_A,ADR_SITE_BASE);
	}
	ptr=Float_to_Byte(f);
	msb=ptr[0];		
	msb|=(ptr[1]<<8); 		
	lsb=ptr[2];	
	lsb|=(ptr[3]<<8); 
	registers_set(lsb,DATA_TMP_A_LSB);
	registers_set(msb,DATA_TMP_A_MSB);
	
	// B
	value=flash_read_32(ADR_FL_B,ADR_SITE_BASE);
	if(value!=0xFFFFFFFF) f=Int32_to_Float((value>>16),(value&0xffff));
	else
	{	
	  f=GOLD_B; 
	  ptr=Float_to_Byte(f);
		msb=ptr[0];		
		msb|=(ptr[1]<<8); 		
		lsb=ptr[2];	
		lsb|=(ptr[3]<<8);
		value=lsb|(msb<<16);
		flash_write_32bit(value,ADR_FL_B,ADR_SITE_BASE);
	}
	ptr=Float_to_Byte(f);
	msb=ptr[0];		
	msb|=(ptr[1]<<8); 		
	lsb=ptr[2];	
	lsb|=(ptr[3]<<8); 
	registers_set(lsb,DATA_TMP_B_LSB);
	registers_set(msb,DATA_TMP_B_MSB);
	
	// Kp
	value=flash_read_32(ADR_FL_Kp,ADR_SITE_BASE);
	if(value!=0xFFFFFFFF) f=Int32_to_Float((value>>16),(value&0xffff));
	else
	{	
	  f=0.3; 
	  ptr=Float_to_Byte(f);
		msb=ptr[0];		
		msb|=(ptr[1]<<8); 		
		lsb=ptr[2];	
		lsb|=(ptr[3]<<8);
		value=lsb|(msb<<16);
		flash_write_32bit(value,ADR_FL_Kp,ADR_SITE_BASE);
	}
	ptr=Float_to_Byte(f);
	msb=ptr[0];		
	msb|=(ptr[1]<<8); 		
	lsb=ptr[2];	
	lsb|=(ptr[3]<<8); 
	registers_set(lsb,DATA_TMP_Kp_LSB);
	registers_set(msb,DATA_TMP_Kp_MSB);
	
	// Ki
	value=flash_read_32(ADR_FL_Ki,ADR_SITE_BASE);
	if(value!=0xFFFFFFFF) f=Int32_to_Float((value>>16),(value&0xffff));
	else
	{	
	  f=0.01; 
	  ptr=Float_to_Byte(f);
		msb=ptr[0];		
		msb|=(ptr[1]<<8); 		
		lsb=ptr[2];	
		lsb|=(ptr[3]<<8);
		value=lsb|(msb<<16);
		flash_write_32bit(value,ADR_FL_Ki,ADR_SITE_BASE);
	}
	ptr=Float_to_Byte(f);
	msb=ptr[0];		
	msb|=(ptr[1]<<8); 		
	lsb=ptr[2];	
	lsb|=(ptr[3]<<8); 
	registers_set(lsb,DATA_TMP_Ki_LSB);
	registers_set(msb,DATA_TMP_Ki_MSB);
	// CH1
	value=flash_read_32(ADR_FL_MSK1,ADR_SITE_BASE);
  registers_set(value,DATA_TMP_MSK1);
  // CH2
	value=flash_read_32(ADR_FL_MSK2,ADR_SITE_BASE);
  registers_set(value,DATA_TMP_MSK2);	
	// CH3
	value=flash_read_32(ADR_FL_MSK3,ADR_SITE_BASE);
  registers_set(value,DATA_TMP_MSK3);
}
