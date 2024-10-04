
#include "gd32f10x_usart.h"
#include "USART.h"
#include "timer.h"
#include "config.h"


DataUsart_TypeDef DataUsart0;
DataUsart_TypeDef DataUsart1;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// USART0 //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern unsigned char USART0_flag_get(void){ return DataUsart0.flag;}
////////////////////////////////////////////////////////////////////////////////
extern void USART0_flag_set(unsigned char value){ DataUsart0.flag=value;}
////////////////////////////////////////////////////////////////////////////////
extern unsigned char USART0_DataIn_get(unsigned int num)
{
  if (num<USART_SIZE_MSG) return DataUsart0.a_input[num];
	return 0;
}
////////////////////// получить указатель a_input[] ////////////////////////////
extern volatile unsigned char* USART0_PtrIn_get(void){ return DataUsart0.a_input;}
////////////////////////////////////////////////////////////////////////////////
extern unsigned int USART0_QntIn_get(void){ return DataUsart0.qnt_in;}
//////////////////////////////////// SEND  //////////////////////////////////////
extern void USART0_send(volatile unsigned char *msg, unsigned int size)
{
  unsigned int i;
	
  DataUsart0.qnt_out=size;
  for(i=0;i<size;i++) {DataUsart0.a_out[i]=msg[i];}
  DataUsart0.count_out=0;
  
  usart_data_transmit(USART0, DataUsart0.a_out[DataUsart0.count_out]);
	usart_interrupt_enable(USART0, USART_INT_TBE);	
	usart_interrupt_disable(USART0, USART_INT_RBNE);
	
	LED_SYS_OFF();	
}
////////////////////////////////////// RECEIVE /////////////////////////////////
extern void USART0_EndReceive(void)
{  
	 if (DataUsart0.flag&USART_RECEIVE_START)
	 {
		 if(get_time0(TIME_10MS)!=0)
		 {
		   DataUsart0.qnt_in=DataUsart0.count_in;
		   DataUsart0.count_in=0;    
		   DataUsart0.flag|=USART_RECEIVE_END;
		   DataUsart0.flag&=~USART_RECEIVE_START; 		 		 
		 }
	 }
}
////////////////////////////////// interrupt ///////////////////////////////////
void USART0_IRQHandler(void)
{
  // transmit
	if(usart_interrupt_flag_get(USART0, USART_INT_FLAG_TBE)!=RESET)
	{
		  usart_interrupt_flag_clear(USART0, USART_INT_FLAG_TBE);

		 if(DataUsart0.count_out<=(DataUsart0.qnt_out-1))
		 {
				if(++DataUsart0.count_out==(DataUsart0.qnt_out-1)) 	
				{
					usart_interrupt_disable(USART0, USART_INT_TBE);		
					usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
	        usart_interrupt_enable(USART0, USART_INT_RBNE);
				}					
				usart_data_transmit(USART0,DataUsart0.a_out[DataUsart0.count_out]);  			
		 }		
		 usart_data_transmit(USART0, DataUsart0.a_out[DataUsart0.count_out]);
	 }
  	// receive
	 if(usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)!=RESET)
	 {
		 usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
		 DataUsart0.flag|=USART_RECEIVE_START; 
  	 // receive data   
		 if(DataUsart0.count_in>(USART_SIZE_MSG-1))DataUsart0.count_in=0;
		 DataUsart0.a_input[DataUsart0.count_in]=usart_data_receive(USART0);  
		 DataUsart0.count_in++;
		 
		 get_time0(0);
		 LED_SYS_ON();
		 
		// DataUsart0.qnt_in=DataUsart0.count_in;    
   //  DataUsart0.flag|=USART_RECEIVE_END;
	 }
    
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// USART1 //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern unsigned char USART1_flag_get(void){ return DataUsart1.flag;}
////////////////////////////////////////////////////////////////////////////////
extern void USART1_flag_set(unsigned char value){ DataUsart1.flag=value;}
////////////////////////////////////////////////////////////////////////////////
extern unsigned char USART1_DataIn_get(unsigned int num)
{
  if (num<USART_SIZE_MSG) return DataUsart1.a_input[num];
	return 0;
}
////////////////////// получить указатель a_input[] ////////////////////////////
extern volatile unsigned char* USART1_PtrIn_get(void){ return DataUsart1.a_input;}
////////////////////////////////////////////////////////////////////////////////
extern unsigned int USART1_QntIn_get(void){ return DataUsart1.qnt_in;}
//////////////////////////////////// SEND  //////////////////////////////////////
extern void USART1_send(volatile unsigned char *msg, unsigned int size)
{
  unsigned int i;
  
  DataUsart1.qnt_out=size;
  for(i=0;i<size;i++) {DataUsart1.a_out[i]=msg[i];}
  DataUsart1.count_out=0;
  
  usart_data_transmit(USART1, DataUsart1.a_out[DataUsart1.count_out]);
	usart_interrupt_enable(USART1, USART_INT_TBE);	
	usart_interrupt_disable(USART1, USART_INT_RBNE);  
	
}
////////////////////////////////////// RECEIVE /////////////////////////////////
extern void USART1_EndReceive(void)
{  
	 if (DataUsart1.flag&USART_RECEIVE_START)
	 {
		 if(get_time1(TIME_10MS)!=0)
		 {
		   DataUsart1.qnt_in=DataUsart1.count_in;
		   DataUsart1.count_in=0;    
		   DataUsart1.flag|=USART_RECEIVE_END;
		   DataUsart1.flag&=~USART_RECEIVE_START; 	

   //    LED_SYS_ON(); // TEST	 		 
		 }
	 }
}
////////////////////////////////// interrupt ///////////////////////////////////
void USART1_IRQHandler(void)
{
  // transmit
	if(usart_interrupt_flag_get(USART1, USART_INT_FLAG_TBE)!=RESET)
	{
		  usart_interrupt_flag_clear(USART1, USART_INT_FLAG_TBE);

		 if(DataUsart1.count_out<=(DataUsart1.qnt_out-1))
		 {
				if(++DataUsart1.count_out==(DataUsart1.qnt_out-1)) 	
				{
					usart_interrupt_disable(USART1, USART_INT_TBE);		
					usart_interrupt_flag_clear(USART1, USART_INT_FLAG_RBNE);
	        usart_interrupt_enable(USART1, USART_INT_RBNE);
				}					
				usart_data_transmit(USART1,DataUsart1.a_out[DataUsart1.count_out]);  			
		 }		
		 usart_data_transmit(USART1, DataUsart1.a_out[DataUsart1.count_out]);
	 }
  	// receive
	 if(usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE)!=RESET)
	 {
		// LED_SYS_OFF(); // TEST
		 usart_interrupt_flag_clear(USART1, USART_INT_FLAG_RBNE);
		 DataUsart1.flag|=USART_RECEIVE_START; 
  	 // receive data   
		 if(DataUsart1.count_in>(USART_SIZE_MSG-1))DataUsart1.count_in=0;
		 DataUsart1.a_input[DataUsart1.count_in]=usart_data_receive(USART1);  
		 DataUsart1.count_in++;
		 
		 get_time1(0);
	 }
    
}
