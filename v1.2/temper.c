#include "gd32f10x_gpio.h"
#include "data.h"
#include "config.h"
#include "temper.h"

#include <math.h>
#include "float.h"

#define DEF_TBOST 60	//boost on/off threshould at V1 algo

extern float Int32_to_Float( unsigned int msb, unsigned int lsb);
extern unsigned char* Float_to_Byte(float f);


extern void handler_memory(void);

uint16_t adc_value[ADC_CH_QNT];
float store_integ_T1=0,store_integ_T2=0;                // хранение нев€зки интег.

///////////////////////////////////////////////////////////////////////////////
//extern void adc_channel_start(unsigned int adc, unsigned char channel);
//extern unsigned int adc_channel_data(unsigned int adc);

extern void adc_channel_start(uint32_t adc, uint8_t channel)
{
    /* ADC regular channel config */
    adc_regular_channel_config(adc, 0U, channel, ADC_SAMPLETIME_7POINT5);
    /* ADC software trigger enable */
    adc_software_trigger_enable(adc, ADC_REGULAR_CHANNEL);
}
///////////////////////////////////////////////////////////////////////////////
extern uint32_t adc_channel_data(uint32_t adc)
{
  // wait the end of conversion flag 
  if(adc_flag_get(adc, ADC_FLAG_EOC))
  {    
    adc_flag_clear(adc, ADC_FLAG_EOC);   // clear the end of conversion flag   
    return (adc_regular_data_read(adc)); // return regular channel sample value 
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
static int pid_temper_T1(unsigned int current, unsigned int target)
{ 
 	 float Kp, Ki, shim,shim_p,shim_i,err,integ;
   
	 Kp= Int32_to_Float(registers_get(DATA_TMP_Kp_MSB),registers_get(DATA_TMP_Kp_LSB)); 
	 Ki= Int32_to_Float(registers_get(DATA_TMP_Ki_MSB),registers_get(DATA_TMP_Ki_LSB));
	
   err=(float)target-(float)current;
	 err/=100.0; 
   ///////////////////// пропорциональна€ составл€юща€ //////////////////////////
   shim_p=(float)err*Kp;  
   ///////////////////// интегральна€ составл€юща€ /////////////////////////
	 integ=err+store_integ_T1;
	 if(integ>INTEG_MAX)integ=INTEG_MAX;
	 else if(integ<INTEG_MIN)integ=INTEG_MIN;
	 store_integ_T1=integ; 
	 shim_i=store_integ_T1*Ki;
   ///////////////////////////////  —”ћћј  ///////////////////////////////
   shim=shim_p+shim_i;
   
   return (int)shim;
}
////////////////////////////////////////////////////////////////////////////////
static int pid_temper_T2(unsigned int current, unsigned int target)
{ 
 	 float Kp, Ki, shim,shim_p,shim_i,err,integ;
   
	 Kp= Int32_to_Float(registers_get(DATA_TMP_Kp_MSB),registers_get(DATA_TMP_Kp_LSB)); 
	 Ki= Int32_to_Float(registers_get(DATA_TMP_Ki_MSB),registers_get(DATA_TMP_Ki_LSB));
	
   err=(float)target-(float)current;
	 err/=100.0; 
   ////////////////////// пропорциональна€ составл€юща€ ////////////////////////
   shim_p=(float)err*Kp;  
   ///////////////////////// интегральна€ составл€юща€ /////////////////////////
	 integ=err+store_integ_T2;
	 if(integ>INTEG_MAX)integ=INTEG_MAX;
	 else if(integ<INTEG_MIN)integ=INTEG_MIN;
	 store_integ_T2=integ; 
	 shim_i=store_integ_T2*Ki;
   /////////////////////////////////////  —”ћћј  ///////////////////////////////
   shim=shim_p+shim_i;
   
   return (int)shim;
}
////////////////////////////////////////////////////////////////////////////////

//V1 PWM algo
void temp_control_v1(void)
{
	static unsigned char timer=0;
	float trg_f;
	int r,b;
	
	r = registers_get(REG_MEAS_PRE);
	b = HEAT_ALL_Bit;
	if(  b&& r) HEAT_ALL_ON();
	if( !b&&!r) HEAT_ALL_OFF();
	
	if(registers_get(REG_MEAS_PRE)>0)
	{	
		if(++timer>=TIMER_PID_MAX)timer=0;	
		//
		trg_f=Int32_to_Float(registers_get(DATA_TMP_t_MSB),registers_get(DATA_TMP_t_LSB));
		trg_f*=100;
		
		//V2 boost heater
		if ( trg_f>DEF_TBOST*100 && !Heater_Boost_Bit)
			registers_set(1, V2_Cmd_Heat_Boost);
		else if 
			( trg_f<DEF_TBOST*100 && Heater_Boost_Bit)
			registers_set(0, V2_Cmd_Heat_Boost);
		
		// T1
		if(registers_get(REG_T1_GRAD)!=VAL_ALARM)
		{
		  if(pid_temper_T1(registers_get(REG_T1_GRAD),(unsigned int)trg_f)>timer){HEAT1_ON();}
		  else {HEAT1_OFF();}
	  }
		else {HEAT1_OFF();}
 	  
		// T2
		if(registers_get(REG_T2_GRAD)!=VAL_ALARM)
		{
		  if(pid_temper_T2(registers_get(REG_T2_GRAD),(unsigned int)trg_f)>timer){HEAT2_ON();}
		  else {HEAT2_OFF();}
	  }
		else {HEAT2_OFF();}		

  }
	else{
    timer=0;		
		
		store_integ_T1=0;
		store_integ_T2=0;
    registers_set(SHIM_MIN,DATA_SHIM_TRG);
	}
}


//V2 static algo
void temp_control_v2(void){
	int r,b;
	
	r = registers_get(REG_MEAS_PRE);
	b = HEAT_ALL_Bit;
	if(  b&& r) HEAT_ALL_ON();
	if( !b&&!r) HEAT_ALL_OFF();

	b = HEAT1_Bit;
	if(  b&& r) HEAT1_ON();
	if( !b&&!r) HEAT1_OFF();
	
	b = HEAT2_Bit;
	if(  b&& r) HEAT2_ON();
	if( !b&&!r) HEAT2_OFF();	
}


#define ADCMax  4096
#define ADCVref 2.500f
#define TVref   1.090f	//RVref at Rsens
#define TRser   81.00f	//Serial R through RVref
#define TKamp   10.44f 	//T amp Ku

//-------------------------------------

void GrapheneSensTemp(uint8_t ch){
	
	float adcd, adcv, Rt;
	float R00, R10, Ro, A, B, t;
	uint16_t rar[] = {V2_Heat_R1,V2_Heat_R2,REG_T1_OHM,REG_T2_OHM};
	uint16_t tar[] = {V2_Heat_T1,V2_Heat_T2,REG_T1_GRAD,REG_T2_GRAD};
	
	if (ch>1) return;
		
		adcd = (float) adc_channel_data(ADC1);;
		adcv = (adcd * ADCVref) / (TKamp * (float)ADCMax);
		Rt   = (adcv * TRser) / (TVref - adcv);
		registers_set((unsigned int)(Rt*10), rar[ch]);
		registers_set((unsigned int)(Rt*100),rar[ch+2]);
	
			R00 = registers_get(V2_Heat_0R0_OHM);
			R10 = registers_get(V2_Heat_0R1_OHM);
			Ro= Int32_to_Float(registers_get(DATA_TMP_R0_MSB),registers_get(DATA_TMP_R0_LSB)); //R at 0 C deg
			if (ch==1) Ro+=(R10-R00)/10;	//correct second R via V2 reg
	
			//GOST 6651-2009, Rt = R0(1 + (A=3,9083E-3)*t + (B=-5,775E-7)t^2)
			//Gold A 0.00428 -> 0.0031, B -0.00000062032 ->0 
			A = Int32_to_Float(registers_get(DATA_TMP_A_MSB), registers_get(DATA_TMP_A_LSB));
			B = Int32_to_Float(registers_get(DATA_TMP_B_MSB), registers_get(DATA_TMP_B_LSB));
			B = B;
	
				//tmp=A*A-4.0*B*(1.0-Rt/R0); t=sqrt(tmp); t-=A; t/=2*B;
				t = (Rt/Ro-1)/A;
				registers_set((short)(t*10), tar[ch]);
				registers_set((short)(t*100),tar[ch+2]);
}

//-------------------------------------
//LM60 sens
//Vo = (+6.25 mV/C x TC) + 424 mV
//Accuracy at 25C = ±3.0C
void intTempSens(){
	float madc, mmv;
	uint16_t tval;
	
	madc = (float) adc_channel_data(ADC1);
	mmv  = 1000 * madc * ADCVref / ADCMax;
	
	tval = (mmv>424.0) ? (mmv-424.0)/0.625f : 0;
	tval -=20; //corr -2 deg C
	
	registers_set(tval,V2_Pcb_Temp);
}

//-------------------------------------
// DS18B20 or DHT22 autodetect sens
// Temp3 Temp4 1wire (DS18) sens
#define	Sens1_PORT	GPIOC
#define	Sens1_PIN		GPIO_PIN_3
#define	Sens2_PORT	GPIOB
#define	Sens2_PIN		GPIO_PIN_0
// HMDT1 HMDT2 1wire (DHT22) sens OR rotator
#define	Sens3_PORT	GPIOA
#define	Sens3_PIN		GPIO_PIN_6
#define	Sens4_PORT	GPIOA
#define	Sens4_PIN		GPIO_PIN_0
// additional 1wire sens
#define	Sens5_PORT	GPIOC
#define	Sens5_PIN		GPIO_PIN_15
// i2c BME280 sens, two slaves avaliable
#define	SensI2C_PORT	GPIOA
#define	SensI2C_SDA		GPIO_PIN_9
#define	SensI2C_SCL		GPIO_PIN_10

extern uint16_t DS18B20_Got(uint32_t ds_port, uint32_t ds_pin);
extern uint32_t DHT22_Got  (uint32_t ds_port, uint32_t ds_pin);
extern bool     BME280_Got (uint8_t ch, uint16_t* t, uint16_t* h, uint16_t* p);

uint16_t DS18B20_Temp(uint8_t sens){
	switch (sens){
		case 0: return DS18B20_Got(Sens1_PORT,Sens1_PIN);
		case 1: return DS18B20_Got(Sens2_PORT,Sens2_PIN);
		case 2: return DS18B20_Got(Sens3_PORT,Sens3_PIN);
		case 3: return DS18B20_Got(Sens4_PORT,Sens4_PIN);
		case 4: return DS18B20_Got(Sens5_PORT,Sens5_PIN);
		default: return 0xfffc;
	}
}

uint32_t DHT22_Temp(uint8_t sens){
	switch (sens){
		case 0: return DHT22_Got(Sens1_PORT,Sens1_PIN);
		case 1: return DHT22_Got(Sens2_PORT,Sens2_PIN);
		case 2: return DHT22_Got(Sens3_PORT,Sens3_PIN);
		case 3: return DHT22_Got(Sens4_PORT,Sens4_PIN);
		case 4: return DHT22_Got(Sens5_PORT,Sens5_PIN);
		default: return 0xfffffffc;
	}
}

void extTempHumSens(uint8_t ch){
	unsigned short usv;
	unsigned int   uiv;
	
	uint16_t tar[] = {V2_1wSens1T, V2_1wSens2T, V2_1wSens3T, V2_1wSens4T, V2_1wSens5T};
	uint16_t har[] = {V2_1wSens1H, V2_1wSens2H, V2_1wSens3H, V2_1wSens4H, V2_1wSens5H};
	
	if (ch>4) return;
	
	//Rotator mode for additional channel 0x4 PC15
	if (registers_get(V2_Rotat) && ch==4 ) {
		registers_set( 0xFFFE, tar[ch]);
		registers_set( 0x0,    har[ch]);
		return;
	}

		usv = DS18B20_Temp(ch);
			if (usv == 0xffff){
				uiv = DHT22_Temp(ch);
					if (uiv<0xfffffffd){
						registers_set( (uint16_t)(uiv&0xFFFF), tar[ch]);
						registers_set( (uint16_t)(uiv>>16),    har[ch]);
					}
						else{
							registers_set( 0xFFFF, tar[ch]);
							registers_set( 0x0,    har[ch]);
						}
				}
			else{
				registers_set(usv, tar[ch]);
				registers_set(0x0, har[ch]);
			}
}

//pair of BME280 sensors
void extTempHumPresSens(uint8_t ch){
	uint16_t t, h, p;
	bool ret;
	
	if (ch>1) return;
	
	ret = BME280_Got(ch,&t,&h,&p);
	if (!ret){t=0xFFFF;h=0;p=0;}
	
				 if (ch==0){
			registers_set(t,V2_I2C_Sens1_T);
			registers_set(h,V2_I2C_Sens1_H);
			registers_set(p,V2_I2C_Sens1_P);
		}
		else if (ch==1){
			registers_set(t,V2_I2C_Sens2_T);
			registers_set(h,V2_I2C_Sens2_H);
			registers_set(p,V2_I2C_Sens2_P);
		}
}

//test mode, off extSens power
void temp_imitate(){
	static unsigned int te;
	signed int dt;
	
	dt= rand()%30 - 15;
	te = 250 + dt;
		
	registers_set( te+rand()%20-10, V2_Pcb_Temp);
	registers_set( te+rand()%10-5, V2_1wSens1T);
	registers_set( 0, V2_1wSens1H);
	registers_set( te+rand()%10-5, V2_1wSens2T);
	registers_set( te+150+rand()%10-5, V2_1wSens2H);
	registers_set( te+rand()%10-5, V2_1wSens3T);
	registers_set( 0, V2_1wSens3H);
	registers_set( te+rand()%10-5, V2_1wSens4T);
	registers_set( te+150+rand()%10-5, V2_1wSens4H);
	registers_set( te+rand()%10-5, V2_1wSens5T);
	registers_set( 0, V2_1wSens5H);

	registers_set( 2450 +rand()%30-15,   V2_I2C_Sens1_T);
	registers_set( 31000+rand()%100-50,  V2_I2C_Sens1_H);
	registers_set( 49600+rand()%200-100, V2_I2C_Sens1_P);

	registers_set( 2470 +rand()%32-16,   V2_I2C_Sens2_T);
	registers_set( 31100+rand()%120-60,  V2_I2C_Sens2_H);
	registers_set( 50100+rand()%240-120, V2_I2C_Sens2_P);
		
	registers_set( 55+rand()%6-3, V2_Heat_R1);
	registers_set( 50+rand()%6-3, V2_Heat_R2);
	registers_set( te+rand()%6-3, V2_Heat_T1);
	registers_set( te+rand()%6-3, V2_Heat_T2);
}

////////////////////////////////////////////////////////////////////////////////
extern void temp_meas(void)
{
	static unsigned char fase=STATE_NOP;
	static unsigned char test_step;
	
	if (registers_get(V2_Cmd_Test)>0){
		if ( !(++test_step%10)) temp_imitate();
	return;
	}
	
	switch(fase)
	{
	  //5
		case TEMP_ON:
			SENS_T_ON();
			if (registers_get(V2_Cmd_Heat_Still)==0) temp_control_v1();
																				  else temp_control_v2();
		break;
		
		//9
		case TEMP1_START:
			adc_channel_start(ADC1,ADC_CHANNEL_14);
		break;			               
		
		case TEMP1_MEAS: 		              
			GrapheneSensTemp(0);
			adc_channel_start(ADC1,ADC_CHANNEL_15);
		break;

		case TEMP2_MEAS: 
			GrapheneSensTemp(1);
			SENS_T_OFF();
			adc_channel_start(ADC1,ADC_CHANNEL_5);
		break;
		
		case V2_SENS0_MEAS: 
			intTempSens();
		break;
		
		case V2_SENS1_MEAS:
			extTempHumSens(0);
		break;
		
		case V2_SENS2_MEAS:
			extTempHumSens(1);
		break;
		
		case V2_SENS3_MEAS:
			extTempHumSens(2);
		break;
		
		case V2_SENS4_MEAS: 
			extTempHumSens(3);
		break;
		
		case V2_SENS5_MEAS: 
			extTempHumSens(4);
		break;
				
		case V2_SENS6_MEAS: 
			extTempHumPresSens(0);
		break;

		case V2_SENS7_MEAS: 
			extTempHumPresSens(1);
			fase=STATE_NOP;
		break;
		
		default: break;
	}
  ++fase;
}
