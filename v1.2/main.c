/* Graphene eNose project
 * Ioffe inst. May-2024
 * V1 - Pechkin lab
 * V2 - Igor Bocharov
*/

//ElNos_rev2_Scheme-23Feb24, GD32F103VBT 100pin, GD32 SPL 2020-09-30 V2.2.0
//Includes: ..\CMSIS;..\GD32F10x_SPL\Include;..\GD32F10x_SPL
//Compiler: ARM 'V5.06 update 7 (build 960)'
//Target options: C/C++ -> No auto includes, Debug -> ST-LinkV2


#include "rtos_v1.h"
#include "timer.h"

//-----------------------------------------------------------------------------
extern void RCC_Configuration(void);
extern void GPIO_Configuration(void);
extern void USART_Configuration(void);

extern void Pin_Init(void);
extern void Data_Init(void);

extern void ADC_Configuration(void);
extern void USART0_EndReceive(void);
extern void ModBus0_answers(void);

extern void meas_cycle(void);
extern void Meas_init(void); 
extern void temp_meas(void);
extern void flash_update(void);

extern void V2_Runner(void);
extern void LedBlink(void);

extern void I2C_Conf(void);

//-----------------------------------------------------------------------------
int main(void)
{ 
  RCC_Configuration();
  GPIO_Configuration();
	Pin_Init();
	
	ADC_Configuration();
	USART_Configuration();
	I2C_Conf();
	
	Data_Init();
	TIM1_Start();
	TIM2_Start();
	Meas_init();
	
  //V1
	RTOS_SetTask(meas_cycle,   RTOS_TIME_0S1, RTOS_TIME_0 );
	RTOS_SetTask(temp_meas,    RTOS_TIME_50MS, RTOS_TIME_1S);
	RTOS_SetTask(flash_update, RTOS_TIME_1S,  RTOS_TIME_0 );
	
	//V2
	RTOS_SetTask(V2_Runner,RTOS_TIME_50MS, RTOS_TIME_0);
	RTOS_SetTask(LedBlink, RTOS_TIME_1S,  RTOS_TIME_0);
	 
	while(1)
	{
		RTOS_Dispatch();    
    RTOS_timer(); 
		
		//OPC server, PA8/9, CH340G
		USART0_EndReceive();
		ModBus0_answers();
		
		//RS485, PA2/3, TD301M485
		//USART1_EndReceive();
		//ModBus1_answers();
	}  
}

//-----------------------------------------------------------------------------
