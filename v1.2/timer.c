#include "gd32f10x.h"
#include "timer.h"

////////////////////////////////////////////////////////////////////////////////
extern void TIM1_Start(void)
{
	timer_parameter_struct timer_initpara;

	rcu_periph_clock_enable(RCU_TIMER1);

	timer_deinit(TIMER1);
	/* initialize TIMER init parameter struct */
	timer_struct_para_init(&timer_initpara);
	/* TIMER1 configuration */
	timer_initpara.prescaler         = PRESC1;
	timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;
	timer_initpara.period            = 0xffff;
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;	
	timer_init(TIMER1, &timer_initpara);

	timer_enable(TIMER1);
}


//V2
//GD32
extern void TIM2_Start()
{
	timer_parameter_struct timer_initpara;

	rcu_periph_clock_enable(RCU_TIMER2);

	timer_deinit(TIMER2);

	timer_struct_para_init(&timer_initpara);
	
	timer_initpara.prescaler         = 32;	//~1uS
	timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;
	timer_initpara.period            = 0xffff;
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;//fDTS=fTIMER_CK/1
	timer_init(TIMER2, &timer_initpara); 

	timer_enable(TIMER2);
}

void GD32_DelayTim2(unsigned short time){
	timer_counter_value_config(TIMER2, 0);
	while ( timer_counter_read(TIMER2) <= (time) ) ;
}


///////////////////////////////////////////////////
extern unsigned char get_time_sys(unsigned int time)
{
  static unsigned int t=0;
  unsigned int tcnt1;
   
  tcnt1=timer_counter_read(TIMER1);
  
  if(!time)
  {t=tcnt1;}
  
  if((tcnt1-t)>=time)
  {     
    t=tcnt1;
    return 1;
  }
	
  return 0;
}
///////////////////////////////////////////////////
extern unsigned char get_time0(unsigned int time)
{
  static unsigned int t=0;
  unsigned int tcnt1;
   
  tcnt1=timer_counter_read(TIMER1);
  
  if(!time) {t=tcnt1;}
  //
  if((tcnt1-t)>=time)
  {     
    t=tcnt1;
    return 1;
  }
	
  return 0;
}
///////////////////////////////////////////////////
extern unsigned char get_time1(unsigned int time)
{
  static unsigned int t=0;
  unsigned int tcnt1;
   
  tcnt1=timer_counter_read(TIMER1);
  
  if(!time) {t=tcnt1;}
  //
  if((tcnt1-t)>=time)
  {     
    t=tcnt1;
    return 1;
  }
	
  return 0;
}
