#include "gd32f10x_gpio.h"

#include "data.h"
#include "config.h"
#include "meas.h"
#include "timer.h"

extern float Int32_to_Float( unsigned int msb, unsigned int lsb);
extern unsigned char* Float_to_Byte(float f);

static unsigned char range_channel[QNT_CHANNEL];

////////////////////////////////////////////////////////////////////////////////
static void reset_IO(void)
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

////////////////////////////////////////////////////////////////////////////////
static unsigned char* Float_to_Byte(float f)
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
extern void meas_imitate(void)
{
    static float f;
    static long int value;
  	static unsigned int meas=0;

    static unsigned int lsb, msb;
    unsigned char* ptr;
    unsigned char i, byte_qnt=0;

    // R1...R38
    byte_qnt=0;
    for(i=0;i<38;i++)
    {
      value=rand() % 65535;
      value*=1525;
      f = (float)value/100.0+300.0;
      ptr=Float_to_Byte(f);

      msb=ptr[0];		
      msb|=(ptr[1]<<8); 		
      lsb=ptr[2];	
      lsb|=(ptr[3]<<8);
   
      registers_set(lsb,(REG_R1_LSB+byte_qnt));
      registers_set(msb,(REG_R1_MSB+byte_qnt));

      byte_qnt+=2;
    }
    //  NUM MEAS
    ++meas;
    registers_set(meas,REG_MEAS_NUM);
		registers_set(meas,REG_MEAS_NUM_COPY);
    // TIME MEAS
    value=rand() % 100;
    value+=200;
    registers_set(value,REG_MEAS_TIME);
		registers_set(value,REG_MEAS_TIME_COPY);
		
    // T1
    value=rand() % 100;
    value+=8000;
    registers_set(value,REG_T1_GRAD);
    // T2
    value=rand() % 100;
    value+=8000;
    registers_set(value,REG_T2_GRAD);
		// Uop
    value=rand() % 9;
    value+=330;
    registers_set(value,REG_Uref);

}
////////////////////////////////////////////////////////////////////////////////
static unsigned int meas_ADC(void)
{
	static unsigned int delay,i,value, meas;

  __disable_irq();
	
	// start
	ADC_CS_DN();
	delay=HALF_PERIOD; while(delay)--delay ;
	for (i=0;i<6;i++) 
	{
		ADC_CLK_DN();
		delay=HALF_PERIOD; while(delay)--delay ; 
		ADC_CLK_UP();
		delay=HALF_PERIOD; while(delay)--delay ;		
	}
	
	// read data
	meas=0; 
	for (i=15;i>0;i--) 
	{
		ADC_CLK_DN();
		delay=HALF_PERIOD/2; while(delay)--delay ;	
		if((GPIO_ISTAT(GPIOC)&GPIO_PIN_1)!=0)meas|=(1<<i);
		delay=HALF_PERIOD/2; while(delay)--delay ;
		ADC_CLK_UP();
		delay=HALF_PERIOD; while(delay)--delay ;	
	}
	
	// ending
	ADC_CS_UP();
	//
	for (i=0;i<3;i++) 
	{
	  ADC_CLK_DN();
    delay=HALF_PERIOD; while(delay)--delay ; 
	  ADC_CLK_UP();
	  delay=HALF_PERIOD; while(delay)--delay ;
	}
	
	// to mV 
	value=(meas>>4)&0x0FFF;
	
	__enable_irq();
	
	return value;
}
////////////////////////////////////////////////////////////////////////////////
static unsigned char convert_mV_to_R(unsigned int meas_mV, unsigned char divider, unsigned int reg_lsb)
{
	float meas_f, res_f;
	unsigned char* ptr;
	static unsigned int lsb, msb;
	
	unsigned char repeat;
	static unsigned char delay=0;
	
	repeat=0;
	
	meas_f = (float)meas_mV;
	res_f=Uref_mV/meas_f;
	res_f-=1.0f;
	
  switch(divider)
  {
	  case Rop_1k: res_f*=999.8f;  delay=0; break;
		case Rop_10k: res_f*=9999.8f;  delay=0; break;	
    case Rop_100k: res_f*=99800.0f; delay=0; break;	
    case Rop_1M: 
			           res_f*=980392.0f; 	             
								 if(++delay<REPEAT_1M) repeat=1;
             		 else delay=0;								 
	  break;
    case Rop_10M: 
			        //   if((int)res_f>CORRECT_LEVEL) res_f*=CORRECT_COEF;
			           res_f*=8333000.0f;                
								 if(++delay<REPEAT_10M) repeat=1;
								 else delay=0;
	  break;
    case Rop_50M: 
			         //  res_f*=CORRECT_COEF;
			           res_f*=50000000.0f;
                 if(res_f<50000000.0f)
								 {									 
		               if(++delay<REPEAT_50M) repeat=1;
								   else delay=0;    
								 }
								 else if(res_f<100000000.0f)
								 {									 
		               if(++delay<REPEAT_100M) repeat=1;
								   else delay=0;    
								 }
								 else
								 {
								   if(++delay<REPEAT_200M) repeat=1;
								   else delay=0; 
								 }
		break;		
	  default: break;
	}
  res_f-=4.0; // сопротивление ключей
	ptr=Float_to_Byte(res_f);
	msb=ptr[0];		
	msb|=(ptr[1]<<8); 		
	lsb=ptr[2];	
	lsb|=(ptr[3]<<8);
	 
	registers_set(lsb,reg_lsb);
	registers_set(msb,(reg_lsb+1));
	
	
	return repeat;
}
////////////////////////////////////////////////////////////////////////////////
static void set_reg_float(unsigned int reg_lsb, float f)
{
	
	unsigned char* ptr;
	static unsigned int lsb, msb;
	
	ptr=Float_to_Byte(1.0);
	msb=ptr[0];		
	msb|=(ptr[1]<<8); 		
	lsb=ptr[2];	
	lsb|=(ptr[3]<<8);
	 
	registers_set(lsb,reg_lsb);
	registers_set(msb,(reg_lsb+1));
}
////////////////////////////////////////////////////////////////////////////////
extern void Meas_init(void) 
{
  unsigned int i;
	
  for(i=0;i<QNT_CHANNEL;i++){ range_channel[i]=Rop_1k;}	
}
////////////////////////////////////////////////////////////////////////////////
static void range_set(unsigned char divider) 
{
  switch(divider)
  {
	  case Rop_1k:  
			            KEY_DWN0_ON();
									KEY_DWN1_OFF();
									KEY_DWN2_OFF();
									KEY_DWN3_OFF();
									KEY_DWN4_OFF();
	               	KEY_DWN5_OFF();									
		break;
		case Rop_10k:
									KEY_DWN0_OFF();
									KEY_DWN1_ON();
									KEY_DWN2_OFF();
									KEY_DWN3_OFF();
									KEY_DWN4_OFF();
	               	KEY_DWN5_OFF(); 			
		break;	
    case Rop_100k:
			            KEY_DWN0_OFF();
									KEY_DWN1_OFF();
									KEY_DWN2_ON();
									KEY_DWN3_OFF();
									KEY_DWN4_OFF();
	               	KEY_DWN5_OFF();
		break;	
    case Rop_1M: 
		            	KEY_DWN0_OFF();
									KEY_DWN1_OFF();
									KEY_DWN2_OFF();
									KEY_DWN3_ON();
									KEY_DWN4_OFF();
	               	KEY_DWN5_OFF();
		break;
    case Rop_10M:  
		            	KEY_DWN0_OFF();
									KEY_DWN1_OFF();
									KEY_DWN2_OFF();
									KEY_DWN3_OFF();
									KEY_DWN4_ON();
	               	KEY_DWN5_OFF();
		break;
    case Rop_50M:  
			            KEY_DWN0_OFF();
									KEY_DWN1_OFF();
									KEY_DWN2_OFF();
									KEY_DWN3_OFF();
									KEY_DWN4_OFF();
	               	KEY_DWN5_OFF();
		break;		
	  default:
                  KEY_DWN0_OFF();
									KEY_DWN1_OFF();
									KEY_DWN2_OFF();
									KEY_DWN3_OFF();
									KEY_DWN4_OFF();
	               	KEY_DWN5_OFF();			
			break;
	}
}
////////////////////////////////////////////////////////////////////////////////
static unsigned char range_check(unsigned char step, unsigned int value_mV)
{
	unsigned char flag=MEAS_NEXT;
//	static unsigned char count_repeat=0;	//V2
	
	if(value_mV<MIN_LVL_MEAS)
	{
		if(range_channel[step]<Rop_50M)
		{
			++range_channel[step];
			range_set(range_channel[step]);
			flag=MEAS_REPEAT;
		}
	}
	else if(value_mV>MAX_LVL_MEAS)	
	{
		if(range_channel[step]>Rop_1k)
		{
			--range_channel[step];
			range_set(range_channel[step]);
			flag=MEAS_REPEAT;
		}
	}	
	
  return flag; 
}
////////////////////////////////////////////////////////////////////////////////
static void channel_set(unsigned char channel)
{
	// reset_IO();
	
	switch(channel)
	{
	  case MEAS_R0:
			           KEY_MID0_ON();
								 KEY_UP0_ON();
	  break;
		case MEAS_R1:
			           KEY_MID0_ON();
								 KEY_UP0_OFF();
		             KEY_UP1_ON();
	  break;
		case MEAS_R2:
			          KEY_MID0_OFF();
								KEY_MID1_ON();
		            KEY_UP1_ON();
		break;
		case MEAS_R3:
			          KEY_MID1_ON();
								KEY_UP1_OFF();
		            KEY_UP2_ON();
		break;
		case MEAS_R4:
			          KEY_MID1_OFF();		
								KEY_MID2_ON();
		            KEY_UP2_ON();
		break;
		case MEAS_R5:
			          KEY_MID2_ON();
								KEY_UP2_OFF();
		            KEY_UP3_ON();
		break;
		case MEAS_R6:
			          KEY_MID2_OFF();		
								KEY_MID3_ON();
		            KEY_UP3_ON();
		break;
		case MEAS_R7:
			          KEY_MID3_ON();
								KEY_UP3_OFF();
		            KEY_UP4_ON();
		break;
		case MEAS_R8:
			          KEY_MID3_OFF();		
								KEY_MID4_ON();
		            KEY_UP4_ON();
		break;
		case MEAS_R9:
			          KEY_MID4_ON();
								KEY_UP4_OFF();
		            KEY_UP5_ON();
		break;
		case MEAS_R10:
			          KEY_MID4_OFF();		
								KEY_MID5_ON();
		            KEY_UP5_ON();
		break;
		case MEAS_R11:
			          KEY_MID5_ON();
								KEY_UP5_OFF();
		            KEY_UP6_ON();
		break;
		case MEAS_R12:
			          KEY_MID5_OFF();		
								KEY_MID6_ON();
		            KEY_UP6_ON();
		break;
		case MEAS_R13:
			          KEY_MID6_ON();
								KEY_UP6_OFF();
		            KEY_UP7_ON();
		break;
		case MEAS_R14:
			          KEY_MID6_OFF();		
								KEY_MID7_ON();
		            KEY_UP7_ON();
		break;
		case MEAS_R15:
			          KEY_MID7_ON();
								KEY_UP7_OFF();
		            KEY_UP8_ON();
		break;
		case MEAS_R16:
			          KEY_MID7_OFF();		
								KEY_MID8_ON();
		            KEY_UP8_ON();
		break;
		case MEAS_R17:
			          KEY_MID8_ON();
								KEY_UP8_OFF();
		            KEY_UP9_ON();
		break;
		case MEAS_R18:
			          KEY_MID8_OFF();		
								KEY_MID9_ON();
		            KEY_UP9_ON();
		break;
		case MEAS_R19:
			          KEY_MID9_ON();
								KEY_UP9_OFF();
		            KEY_UP10_ON();
		break;
		case MEAS_R20:
			          KEY_MID9_OFF();		
								KEY_MID10_ON();
		            KEY_UP10_ON();
		break;
		case MEAS_R21:
			          KEY_MID10_ON();
								KEY_UP10_OFF();
		            KEY_UP11_ON();
		break;
		case MEAS_R22:
			          KEY_MID10_OFF();		
								KEY_MID11_ON();
		            KEY_UP11_ON();
		break;
		case MEAS_R23:
			          KEY_MID11_ON();
								KEY_UP11_OFF();
		            KEY_UP12_ON();
		break;
		case MEAS_R24:
			          KEY_MID11_OFF();		
								KEY_MID12_ON();
		            KEY_UP12_ON();
		break;
		case MEAS_R25:
			          KEY_MID12_ON();
								KEY_UP12_OFF();
		            KEY_UP13_ON();
		break;
		case MEAS_R26:
			          KEY_MID12_OFF();		
								KEY_MID13_ON();
		            KEY_UP13_ON();
		break;
		case MEAS_R27:
			          KEY_MID13_ON();
								KEY_UP13_OFF();
		            KEY_UP14_ON();
		break;
		case MEAS_R28:
			          KEY_MID13_OFF();		
								KEY_MID14_ON();
		            KEY_UP14_ON();
		break;
		case MEAS_R29:
			          KEY_MID14_ON();
								KEY_UP14_OFF();
		            KEY_UP15_ON();
		break;
		case MEAS_R30:
			          KEY_MID14_OFF();		
								KEY_MID15_ON();
		            KEY_UP15_ON();
		break;
		case MEAS_R31:
			          KEY_MID15_ON();
								KEY_UP15_OFF();
		            KEY_UP16_ON();
		break;
		case MEAS_R32:
			          KEY_MID15_OFF();		
								KEY_MID16_ON();
		            KEY_UP16_ON();
		break;
		case MEAS_R33:
			          KEY_MID16_ON();
								KEY_UP16_OFF();
		            KEY_UP17_ON();
		break;
		case MEAS_R34:
			          KEY_MID16_OFF();		
								KEY_MID17_ON();
		            KEY_UP17_ON();
		break;
		case MEAS_R35:
			          KEY_MID17_ON();
								KEY_UP17_OFF();
		            KEY_UP18_ON();
		break;
		case MEAS_R36:
			          KEY_MID17_OFF();		
								KEY_MID18_ON();
		            KEY_UP18_ON();
		break;
		case MEAS_R37:
			          KEY_MID18_ON();
								KEY_UP18_OFF();
		            KEY_UP19_ON();
		break;
		case MEAS_R38:
			          KEY_MID18_OFF();		
								KEY_MID19_ON();
		            KEY_UP19_ON();
		break;
	}
}
////////////////////////////////////////////////////////////////////////////////
extern void meas_cycle(void)
{
  static unsigned char step=MEAS_NOP;
	unsigned int num_meas, value_mV;
	unsigned char flag_step;
	static unsigned int range;
	static unsigned char test_step;
	//static unsigned int flag_range_1K=VAL_RESET;
	
	int rn; //V2 channel num
	
	static unsigned int time=0, cmd=CMD_START;
	static unsigned int msk1, msk2, msk3;
	
	if (registers_get(V2_Cmd_Test)>0){
		if ( !(++test_step%10)) meas_imitate();
	return;
	}
	
	msk1=registers_get(DATA_TMP_MSK1);
  msk2=(registers_get(DATA_TMP_MSK2)<<16);
  msk3=registers_get(DATA_TMP_MSK3);
	

	//V2, modbus_set_coil_On = 0x00FF, not just 1
	if(registers_get(REG_MEAS_CMD)>0)
	{
		++time;
		//
		switch(step)
		{
			case MEAS_NOP: 	            
										 // for Uref
										 KEY_UP0_ON();  
										 KEY_MID19_ON();
										 range_set(Rop_10k);
			
										 step=MEAS_Uref;	
										 time=0;		
			break;
			case MEAS_Uref: 
										 registers_set((meas_ADC()<<1),REG_Uref);	               
										 KEY_MID19_OFF();	               
										 // for R0
										 KEY_MID0_ON();
										 KEY_UP0_ON();
			               
			               step=MEAS_R0;
			               channel_set(step);
										 range_set(range_channel[step]);
			break; 
			case MEAS_R0: 		
                      if(msk1&(1<<MEAS_R0))
											{												
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{										
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{
														++step;
														channel_set(step);
			  							      range_set(range_channel[step]);													
													}
												}	
										  }	
                      else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);											
												++step;
												channel_set(step);
												range_set(range_channel[step]);
											}												
			break;
			case MEAS_R1:   
				              if(msk1&(1<<MEAS_R1))
											{	
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);            
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{
													//	KEY_MID0_OFF();
														// for next meas
													//	KEY_MID1_ON();
														++step;
														channel_set(step);
														range_set(range_channel[step]);													
														
													}
												}
											}
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
									//		  KEY_MID0_OFF();
												// for next meas
								//				KEY_MID1_ON();
												++step;
												channel_set(step);
												range_set(range_channel[step]);													
												
											}
			break;
			case MEAS_R2:  
				              if(msk1&(1<<MEAS_R2))
											{	
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{                 
														KEY_UP1_OFF();	
														// for next meas
														KEY_UP2_ON();
														range_set(range_channel[(step+1)]);												
														++step;
													}
												}	
										 }
										 else
										 {
											  set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
											 
											  KEY_UP1_OFF();	
												// for next meas
												KEY_UP2_ON();
												range_set(range_channel[(step+1)]);												
												++step;
										 }
			break;
			case MEAS_R3:   			
                      if(msk1&(1<<MEAS_R3))
											{					
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{
														KEY_MID1_OFF();		
														// for next meas
														KEY_MID2_ON();
														range_set(range_channel[(step+1)]);												
														++step;
													}
												}
										  }
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
												KEY_MID1_OFF();		
												// for next meas
												KEY_MID2_ON();
												range_set(range_channel[(step+1)]);												
												++step;
											}
			break;
			case MEAS_R4:    
				              if(msk1&(1<<MEAS_R4))
											{
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{											
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{													
														KEY_UP2_OFF();	
														// for next meas
														KEY_UP3_ON();
														range_set(range_channel[(step+1)]);													
														++step;
													}
												}
										  }
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
											  KEY_UP2_OFF();	
												// for next meas
												KEY_UP3_ON();
												range_set(range_channel[(step+1)]);													
												++step;
											}
			break;
			case MEAS_R5:   
				              if(msk1&(1<<MEAS_R5))
											{
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{
														KEY_MID2_OFF();		
														// for next meas
														KEY_MID3_ON();
														range_set(range_channel[(step+1)]);													
														++step;
													}
												}
										  }
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
											  KEY_MID2_OFF();		
												// for next meas
												KEY_MID3_ON();
												range_set(range_channel[(step+1)]);													
												++step;
											}
			break;
			case MEAS_R6:   
				              if(msk1&(1<<MEAS_R6))
											{ 
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{											
														KEY_UP3_OFF();	
														// for next meas
														KEY_UP4_ON();
														range_set(range_channel[(step+1)]);													
														++step;
													}
												}
											}
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
											  KEY_UP3_OFF();	
												// for next meas
												KEY_UP4_ON();
												range_set(range_channel[(step+1)]);													
												++step;
											}
			break;
			case MEAS_R7:  
				              if(msk1&(1<<MEAS_R7))
											{ 
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{
														KEY_MID3_OFF();		
														// for next meas
														KEY_MID4_ON();
														range_set(range_channel[(step+1)]);											
														++step;
													}
												}
										  }
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
											  KEY_MID3_OFF();		
												// for next meas
												KEY_MID4_ON();
												range_set(range_channel[(step+1)]);											
												++step;
											}
			break;
			case MEAS_R8:   
				              if(msk1&(1<<MEAS_R8))
											{ 
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{                
														KEY_UP4_OFF();	
														// for next meas
														KEY_UP5_ON();
														range_set(range_channel[(step+1)]);												
														++step;
													}
												}		
											}
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
												KEY_UP4_OFF();	
												// for next meas
												KEY_UP5_ON();
												range_set(range_channel[(step+1)]);												
												++step;
											}	
			break;
			case MEAS_R9: 
				              if(msk1&(1<<MEAS_R9))
											{
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{
														KEY_MID4_OFF();		
														// for next meas
														KEY_MID5_ON();
														range_set(range_channel[(step+1)]);											
														++step;
													}
												}
											}
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
											  KEY_MID4_OFF();		
												// for next meas
												KEY_MID5_ON();
												range_set(range_channel[(step+1)]);											
												++step;
											}
			break;
			case MEAS_R10: 
				              if(msk1&(1<<MEAS_R10))
											{
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{                  
														KEY_UP5_OFF();	
														// for next meas
														KEY_UP6_ON();
														range_set(range_channel[(step+1)]);													
														++step;
													}
												}		
											}
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
											  KEY_UP5_OFF();	
												// for next meas
												KEY_UP6_ON();
												range_set(range_channel[(step+1)]);													
												++step;
											}
			break;
			case MEAS_R11: 
				              if(msk1&(1<<MEAS_R11))
											{
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{
														KEY_MID5_OFF();		
														// for next meas
														KEY_MID6_ON();
														 
														++step;
														range_set(range_channel[step]);																											
													}
												}
											}
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
											  KEY_MID5_OFF();		
												// for next meas
												KEY_MID6_ON();
												++step;
												range_set(range_channel[step]);														
												
											}
			break;
			case MEAS_R12: 
				              if(msk1&(1<<MEAS_R12))
											{											
												//
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{                 
														KEY_UP6_OFF();	
														// for next meas
														KEY_UP7_ON();
														++step;
														range_set(range_channel[step]);												
														
													}
												}		  
											}
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
											  KEY_UP6_OFF();	
												// for next meas
												KEY_UP7_ON();
												range_set(range_channel[(step+1)]);												
												++step;
											}
			break; 
			case MEAS_R13: 
				              if(msk1&(1<<MEAS_R13))
											{
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{
														KEY_MID6_OFF();		
														// for next meas
														KEY_MID7_ON();
														
														++step;
														range_set(range_channel[step]);																								
													}
												}
											}
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
												KEY_MID6_OFF();		
												// for next meas
												KEY_MID7_ON();
												++step;
												range_set(range_channel[step]);											
												
											}
			break;
			case MEAS_R14: 
				              if(msk1&(1<<MEAS_R14))
											{
												
													//
													value_mV=meas_ADC();
													flag_step=range_check(step,value_mV);
													// go to next step	
													if(flag_step==MEAS_NEXT)
													{
														if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
														{               
															KEY_UP7_OFF();	
															// for next meas
															KEY_UP8_ON();
															
															++step;
															range_set(range_channel[step]);														
														}
													}									
										  }		
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
												KEY_UP7_OFF();	
												// for next meas
												KEY_UP8_ON();
												++step;
												range_set(range_channel[step]);																								
											}	
			break;
			case MEAS_R15: 
				              if(msk1&(1<<MEAS_R15))
											{												
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{ 
														KEY_MID7_OFF();		
														// for next meas
														KEY_MID8_ON();
														range_set(range_channel[(step+1)]);													
														++step;
													}
												}													
											}
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
												KEY_MID7_OFF();		
												// for next meas
												KEY_MID8_ON();
												range_set(range_channel[(step+1)]);													
												++step;
											}
			break;
			case MEAS_R16: 
				              if(msk2&(1<<MEAS_R16))
											{
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{                 
														KEY_UP8_OFF();	
														// for next meas
														KEY_UP9_ON();
														range_set(range_channel[(step+1)]);												
														++step;
													}
												}		
											}
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
											  KEY_UP8_OFF();	
												// for next meas
												KEY_UP9_ON();
												range_set(range_channel[(step+1)]);												
												++step;
											}
			break;
			case MEAS_R17: 
				              if(msk2&(1<<MEAS_R17))
											{
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{ 
														KEY_MID8_OFF();		
														// for next meas
														KEY_MID9_ON();
														range_set(range_channel[(step+1)]);											
														++step;
													}
												}
											}
											else
											{
                        set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
												KEY_MID8_OFF();		
												// for next meas
												KEY_MID9_ON();
												range_set(range_channel[(step+1)]);											
												++step;
											}
			break;
			case MEAS_R18: 
				              if(msk2&(1<<MEAS_R18))
											{
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{               
														KEY_UP9_OFF();	
														// for next meas
														KEY_UP10_ON();
														range_set(range_channel[(step+1)]);														
														++step;
													}
												}	
											}
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
												KEY_UP9_OFF();	
												// for next meas
												KEY_UP10_ON();
												range_set(range_channel[(step+1)]);														
												++step;
											}	
			break;
			case MEAS_R19: 
				              if(msk2&(1<<MEAS_R19))
											{
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{ 
														KEY_MID9_OFF();		
														// for next meas
														KEY_MID10_ON();
														range_set(range_channel[(step+1)]);												
														++step;
													}
												}
											}
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
											  KEY_MID9_OFF();		
												// for next meas
												KEY_MID10_ON();
												range_set(range_channel[(step+1)]);												
												++step;
											}
			break;
			case MEAS_R20: 
				             if(msk2&(1<<MEAS_R20))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{            
														KEY_UP10_OFF();	
														// for next meas
														KEY_UP11_ON();
														range_set(range_channel[(step+1)]);												
														++step;
													}
												}		
										 }
										 else
										 {
											  set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
											 
										 	  KEY_UP10_OFF();	
												// for next meas
												KEY_UP11_ON();
												range_set(range_channel[(step+1)]);												
												++step;
										 }
			break;
			case MEAS_R21: 
				             if(msk2&(1<<MEAS_R21))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{ 
														KEY_MID10_OFF();		
														// for next meas
														KEY_MID11_ON();
														range_set(range_channel[(step+1)]);												
														++step;
													}
												}
										 }
										 else
										 {
											  set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
											 
											  KEY_MID10_OFF();		
												// for next meas
												KEY_MID11_ON();
												range_set(range_channel[(step+1)]);												
												++step;
										 }
			break;
			case MEAS_R22: 
				             if(msk2&(1<<MEAS_R22))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{                  
														KEY_UP11_OFF();	
														// for next meas
														KEY_UP12_ON();
														range_set(range_channel[(step+1)]);													
														++step;
													}
												}	
											}
										  else
										  {
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
											  KEY_UP11_OFF();	
												// for next meas
												KEY_UP12_ON();
												range_set(range_channel[(step+1)]);													
												++step;
									    }
			break;
			case MEAS_R23: 
				             if(msk2&(1<<MEAS_R23))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{ 
														KEY_MID11_OFF();		
														// for next meas
														KEY_MID12_ON();
														range_set(range_channel[(step+1)]);												
														++step;
													}
												}
											}
										  else
										  {
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
											  KEY_MID11_OFF();		
												// for next meas
												KEY_MID12_ON();
												range_set(range_channel[(step+1)]);												
												++step;
											}
			break;
			case MEAS_R24: 
				             if(msk2&(1<<MEAS_R24))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{                  
														KEY_UP12_OFF();	
														// for next meas
														KEY_UP13_ON();
														range_set(range_channel[(step+1)]);													
														++step;
													}
												}	
										 }
										 else
										 {
											  set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
											 
										    KEY_UP12_OFF();	
												// for next meas
												KEY_UP13_ON();
												range_set(range_channel[(step+1)]);													
												++step;
										 }
			break;
			case MEAS_R25: 
				             if(msk2&(1<<MEAS_R25))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{ 
														KEY_MID12_OFF();		
														// for next meas
														KEY_MID13_ON();
														range_set(range_channel[(step+1)]);													
														++step;
													}
												}
										 }
										 else
										 {
											  set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
											 
										    KEY_MID12_OFF();		
												// for next meas
												KEY_MID13_ON();
												range_set(range_channel[(step+1)]);													
												++step;
										 }
			break;
			case MEAS_R26: 
				             if(msk2&(1<<MEAS_R26))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{             
														KEY_UP13_OFF();	
														// for next meas
														KEY_UP14_ON();
														range_set(range_channel[(step+1)]);											
														++step;
													}
												}	
										 }	
										 else
										 {
											  set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
											 
											  KEY_UP13_OFF();	
												// for next meas
												KEY_UP14_ON();
												range_set(range_channel[(step+1)]);											
												++step;
										 }
			break;
			case MEAS_R27: 
				             if(msk2&(1<<MEAS_R27))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{ 
														KEY_MID13_OFF();		
														// for next meas
														KEY_MID14_ON();
														range_set(range_channel[(step+1)]);												
														++step;
													}
												}
										 }
										 else
										 {
											  set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
											 
										    KEY_MID13_OFF();		
												// for next meas
												KEY_MID14_ON();
												range_set(range_channel[(step+1)]);												
												++step;
 										 }
			break;
			case MEAS_R28: 
				             if(msk2&(1<<MEAS_R28))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{											
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{ 											
														KEY_UP14_OFF();	
														// for next meas
														KEY_UP15_ON();
														++step;
														range_set(range_channel[step]);													
                          	
													}
												}
									   }											
 										 else
										 {
											 set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
											 
										   KEY_UP14_OFF();	
											 // for next meas
											 KEY_UP15_ON();
											 ++step;
											 range_set(range_channel[step]);													 
										 }
			break;
			case MEAS_R29: 
				             if(msk2&(1<<MEAS_R29))
										 {										
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{ 
														KEY_MID14_OFF();		
														// for next meas
														KEY_MID15_ON();
														range_set(range_channel[(step+1)]);												
														++step;
													}
												}										 
										 }
										 else
										 {
											  set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
											 
										    KEY_MID14_OFF();		
												// for next meas
												KEY_MID15_ON();
												range_set(range_channel[(step+1)]);												
												++step;
										 }
										 
			break;
			case MEAS_R30: 
				             if(msk2&(1<<MEAS_R30))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{                   
														KEY_UP15_OFF();	
														// for next meas
														KEY_UP16_ON();
														range_set(range_channel[(step+1)]);														
														++step;
													}
												}
											}
											else
											{
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
												KEY_UP15_OFF();	
												// for next meas
												KEY_UP16_ON();
												range_set(range_channel[(step+1)]);														
												++step;
											}										 
			break;
			case MEAS_R31:
				             //if(msk2&(1<<MEAS_R31))	//!!! see warn
										 if(msk2&(0x1U<<MEAS_R31))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
												 if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{ 
														KEY_MID15_OFF();		
														// for next meas
														KEY_MID16_ON();
														range_set(range_channel[(step+1)]);												
														++step;
													}
												}
											}
										 else
										 {
											 set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
											 
										   KEY_MID15_OFF();		
 											 // for next meas
											 KEY_MID16_ON();
											 range_set(range_channel[(step+1)]);												
											 ++step;
										 }
			break;
			case MEAS_R32: 
				             if(msk3&(1<<(MEAS_R32-32)))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{                 
														KEY_UP16_OFF();	
														// for next meas
														KEY_UP17_ON();
														range_set(range_channel[(step+1)]);													
														++step;
													}
												}	
										 }
										 else
										 { 
											 set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
											 
										   KEY_UP16_OFF();	
											 // for next meas
											 KEY_UP17_ON();
											 range_set(range_channel[(step+1)]);													
											 ++step;
										 }
			break;
			case MEAS_R33: 
				             if(msk3&(1<<(MEAS_R33-32)))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{ 
														KEY_MID16_OFF();		
														// for next meas
														KEY_MID17_ON();
														range_set(range_channel[(step+1)]);														
														++step;
													}
												}
											}
										 else
										 {
											 set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
											 
											 KEY_MID16_OFF();		
											 // for next meas
											 KEY_MID17_ON();
											 range_set(range_channel[(step+1)]);														
											 ++step;
										 }
			break;
			case MEAS_R34: 
				             if(msk3&(1<<(MEAS_R34-32)))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{                
														KEY_UP17_OFF();	
														// for next meas
														KEY_UP18_ON();
														range_set(range_channel[(step+1)]);												
														++step;
													}
												}	
										 }
										 else
										 {
											 set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
											 
										   KEY_UP17_OFF();	
											 // for next meas
											 KEY_UP18_ON();
											 range_set(range_channel[(step+1)]);												
											 ++step;
										 }
			break;
			case MEAS_R35: 
				             if(msk3&(1<<(MEAS_R35-32)))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{
														KEY_MID17_OFF();		
														// for next meas
														KEY_MID18_ON();
														++step;
														range_set(range_channel[step]);													
														
													}
												}
											}
										 else
										 {
											  set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
											 
										    KEY_MID17_OFF();		
												// for next meas
												KEY_MID18_ON();
												++step;
											  range_set(range_channel[step]);													
												
										 }
			break;
			case MEAS_R36:
				             if(msk3&(1<<(MEAS_R36-32)))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{                
														KEY_UP18_OFF();	
														// for next meas
														KEY_UP19_ON();
														++step;
														range_set(range_channel[step]);												
														
													}
												}	
										 }
										 else
										 {
											  set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
											 
											  KEY_UP18_OFF();	
												// for next meas
												KEY_UP19_ON();
												++step;
											  range_set(range_channel[step]);														
										 }
			break;
			case MEAS_R37:
				             if(msk3&(1<<(MEAS_R37-32)))
										 {
												value_mV=meas_ADC();
												flag_step=range_check(step,value_mV);
												// go to next step	
												if(flag_step==MEAS_NEXT)
												{
													
													if(!convert_mV_to_R(value_mV,range_channel[step],(REG_R1_LSB+(step<<1))))
													{
														KEY_MID18_OFF();		
														// for next meas
														KEY_MID19_ON();
														++step;
														range_set(range_channel[step]);													
														
													}
												}
											}
										  else
										  {
												set_reg_float((REG_R1_LSB+(step<<1)),VAL_ALARM);
												
												KEY_MID18_OFF();		
												// for next meas
												KEY_MID19_ON();
												++step;
												range_set(range_channel[step]);													
												
											}
			break;		
			case MEAS_R38: 	
											//  NUM MEAS
											num_meas=registers_get(REG_MEAS_NUM);
											++num_meas;                
											registers_set(num_meas,REG_MEAS_NUM);
			registers_set(num_meas,REG_MEAS_NUM_COPY);
			
			
											// TIME
											registers_set(time*10,REG_MEAS_TIME);
			registers_set(time*10,REG_MEAS_TIME_COPY);
			                
											
											step=MEAS_RESET_IO;
			break;
			case MEAS_RESET_IO:
											reset_IO(); 
											step=MEAS_NOP;
		               		
			break;
			
			default: step=MEAS_NOP; break;
		}
		// LED
		//if(step%2) {LED_SYS_ON();}
		//else {LED_SYS_OFF();}
		// Num channel
		//V2
		
		if((step>=MEAS_R1)&&(step<=MEAS_R38)) 
			rn = step-MEAS_R1+1;
		else 
			rn = 0;
			
		registers_set(rn,REG_MEAS_CH);
		registers_set(rn,REG_MEAS_CH_COPY);
		
		//
		//VENT_ON();
		// RANGE of channel
		range=(range_channel[3]<<12)|(range_channel[2]<<8)|(range_channel[1]<<4)|range_channel[0];
		registers_set(range,REG_RANGE_1_4);
		range=(range_channel[7]<<12)|(range_channel[6]<<8)|(range_channel[5]<<4)|range_channel[4];
		registers_set(range,REG_RANGE_5_8);
		range=(range_channel[11]<<12)|(range_channel[10]<<8)|(range_channel[9]<<4)|range_channel[8];
		registers_set(range,REG_RANGE_9_12);
		range=(range_channel[15]<<12)|(range_channel[14]<<8)|(range_channel[13]<<4)|range_channel[12];
		registers_set(range,REG_RANGE_13_16);
		range=(range_channel[19]<<12)|(range_channel[18]<<8)|(range_channel[17]<<4)|range_channel[16];
		registers_set(range,REG_RANGE_17_20);
		range=(range_channel[23]<<12)|(range_channel[22]<<8)|(range_channel[21]<<4)|range_channel[20];
		registers_set(range,REG_RANGE_21_24);
		range=(range_channel[27]<<12)|(range_channel[26]<<8)|(range_channel[25]<<4)|range_channel[24];
		registers_set(range,REG_RANGE_25_28);
		range=(range_channel[31]<<12)|(range_channel[30]<<8)|(range_channel[29]<<4)|range_channel[28];
		registers_set(range,REG_RANGE_29_32);
		range=(range_channel[35]<<12)|(range_channel[34]<<8)|(range_channel[33]<<4)|range_channel[32];
		registers_set(range,REG_RANGE_33_36);
		range=(range_channel[37]<<4)|range_channel[36];
		registers_set(range,REG_RANGE_37_38);
  }
	else
	{
		//VENT_OFF();
		
	  //LED_SYS_ON();
		registers_set(0,REG_MEAS_CH);
		//
		if(cmd==CMD_START)
		{
	  	step=MEAS_NOP;
		  reset_IO(); 
			
		  // for Uref
		  KEY_UP0_ON();  
		  KEY_MID19_ON();
		  range_set(Rop_10k);
		}
		else registers_set((meas_ADC()<<1),REG_Uref);	
	}
	//
	cmd=registers_get(REG_MEAS_CMD);
}
